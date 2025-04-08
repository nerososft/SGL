#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer KernelBuffer {
    int kernel[];
} kernelData;

layout (std430, binding = 2) buffer OutputImageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    int radius;
    int scale;
    int offset;
} params;

// ABGR
uint packColor(vec4 color) {
    return (
    uint(clamp(color.a, 0.0, 1.0) * 255.0) << 24) |
    (uint(clamp(color.b, 0.0, 1.0) * 255.0) << 16) |
    (uint(clamp(color.g, 0.0, 1.0) * 255.0) << 8) |
    (uint(clamp(color.r, 0.0, 1.0) * 255.0)
    );
}

// ABGR
vec4 unpackColor(uint color) {
    return vec4(
    float((color) & 0xFF) / 255.0f,
    float((color >> 8) & 0xFF) / 255.0f,
    float((color >> 16) & 0xFF) / 255.0f,
    float((color >> 24) & 0xFF) / 255.0f
    );
}


void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(params.width, params.height)))) {
        return;
    }

    uint index = coord.y * (params.bytesPerLine / 4) + coord.x;
    
	//vec4 color = unpackColor(inputImage.pixels[index]);
	
	//outputImage.pixels[index] = packColor(color);
	//return ;
	
	
    int kernelSize = 2 * params.radius + 1;
    int r_sum = 0;
    int g_sum = 0;
    int b_sum = 0;
    
    int kernelIndex = 0;
    for (int m = -params.radius; m <= params.radius; m++) {
        for (int n = -params.radius; n <= params.radius; n++) {
            int row = clamp(int(coord.y) + m, 0, int(params.height) - 1);
            int col = clamp(int(coord.x) + n, 0, int(params.width) - 1);
            
            uint neighborIndex = row * (params.bytesPerLine / 4) + col;
            vec4 neighborColor = unpackColor(inputImage.pixels[neighborIndex]);
            
            int kernelValue = kernelData.kernel[kernelIndex++];
            
            b_sum += int(neighborColor.b * 255.0) * kernelValue;
            g_sum += int(neighborColor.g * 255.0) * kernelValue;
            r_sum += int(neighborColor.r * 255.0) * kernelValue;
        }
    }
    
    // Apply scale and offset
    b_sum = max(b_sum, 0) / params.scale + params.offset;
    g_sum = max(g_sum, 0) / params.scale + params.offset;
    r_sum = max(r_sum, 0) / params.scale + params.offset;
    
    // Clamp to [0, 255]
    vec4 resultColor = vec4(
        float(clamp(r_sum, 0, 255)) / 255.0,
        float(clamp(g_sum, 0, 255)) / 255.0,
        float(clamp(b_sum, 0, 255)) / 255.0,
        1.0
    );
    
    outputImage.pixels[index] = packColor(resultColor);
}