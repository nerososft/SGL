#version 450

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (std430, binding = 0) buffer InputImageStorageBuffer {
    uint pixels[];
} inputImage;

layout (std430, binding = 1) buffer OutputImageStorageBuffer {
    uint pixels[];
} outputImage;

layout (push_constant) uniform FilterParams {
    uint width;
    uint height;
    uint channels;
    uint bytesPerLine;
    uint targetWidth;
    uint targetHeight;
	uint type;
} filterParams;

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

// 修正后的三次卷积权重计算
float cubicWeight(float d) {
    d = abs(d);
    if (d < 1.0) return (4.0 + d * d * (3.0 * d - 6.0)) / 6.0;
    if (d < 2.0) return (8.0 + d * (-12.0 + d * (6.0 - d))) / 6.0;
    return 0.0;
}


// 双三次插值权重函数 (a = -0.5)
float cubicWeightNew(float x) {
    x = abs(x);
    if (x < 1.0) {
        return (-0.5 + 2.0) * x*x*x - (-0.5 + 3.0) * x*x + 1.0;
    } else if (x < 2.0) {
        return -0.5 * x*x*x + 5.0 * 0.5 * x*x - 8.0 * 0.5 * x + 4.0 * 0.5;
    }
    return 0.0;
}


void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.targetWidth, filterParams.targetHeight)))) return;

    vec2 uv = (vec2(coord) + 0.5) / vec2(filterParams.targetWidth, filterParams.targetHeight);
    vec2 srcCoord = uv * vec2(filterParams.width, filterParams.height);

    vec2 fractPart = fract(srcCoord);
    ivec2 base = ivec2(srcCoord - fractPart);

    vec4 finalColor = vec4(0);
    float totalWeight = 0.0;

    for (int y = -1; y <= 2; ++y) {
        for (int x = -1; x <= 2; ++x) {
            ivec2 samplePos = base + ivec2(x, y);
            vec2 d = vec2(x, y) - fractPart + 1.0; // 计算相对位置

            float wx = 0;
            float wy = 0;
			if(filterParams.type == 1){
				 wx = cubicWeight(d.x);
				 wy = cubicWeight(d.y);
			}else if(filterParams.type == 2){
				 wx = cubicWeightNew(d.x);
				 wy = cubicWeightNew(d.y);
			}
			
            float weight = wx * wy;

            samplePos = clamp(samplePos, ivec2(0), ivec2(filterParams.width - 1, filterParams.height - 1));
            uint pixelIndex = samplePos.y * (filterParams.bytesPerLine / 4) + samplePos.x;
            finalColor += unpackColor(inputImage.pixels[pixelIndex]) * weight;
            totalWeight += weight;
        }
    }

    finalColor /= max(totalWeight, 1e-6); // 防止除以零
    outputImage.pixels[coord.y * filterParams.targetWidth + coord.x] = packColor(clamp(finalColor, 0.0, 1.0));
}