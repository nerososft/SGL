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
    mat4 transform;
} filterParams;

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    uint pixel = inputImage.pixels[index];

    vec4 transformed = filterParams.transform * vec4(coord.x, coord.y, 0.0, 0.0f);
    uint transformedIndex = uint(transformed.y) * (filterParams.bytesPerLine / 4) + uint(transformed.x);

    outputImage.pixels[transformedIndex] = pixel;
}
