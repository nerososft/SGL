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
    float vibrance;
    float saturation;
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

// RGB to HSV conversion
vec3 rgbToHsv(vec3 rgb) {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    float maxVal = max(max(r, g), b);
    float minVal = min(min(r, g), b);
    float delta = maxVal - minVal;

    float h = 0.0;
    float s = 0.0;
    float v = maxVal;

    if (delta != 0.0) {
        s = delta / maxVal;

        if (r == maxVal) {
            h = (g - b) / delta;
        } else if (g == maxVal) {
            h = 2.0 + (b - r) / delta;
        } else {
            h = 4.0 + (r - g) / delta;
        }

        h *= 60.0;
        if (h < 0.0) {
            h += 360.0;
        }
    }

    return vec3(h, s, v);
}

// HSV to RGB conversion
vec3 hsvToRgb(vec3 hsv) {
    float h = hsv.x, s = hsv.y, v = hsv.z;
    float c = v * s;
    float x = c * (1.0 - abs(mod(h / 60.0, 2.0) - 1.0));
    float m = v - c;

    vec3 rgb;
    if (h < 60.0) {
        rgb = vec3(c, x, 0.0);
    } else if (h < 120.0) {
        rgb = vec3(x, c, 0.0);
    } else if (h < 180.0) {
        rgb = vec3(0.0, c, x);
    } else if (h < 240.0) {
        rgb = vec3(0.0, x, c);
    } else if (h < 300.0) {
        rgb = vec3(x, 0.0, c);
    } else {
        rgb = vec3(c, 0.0, x);
    }

    return rgb + vec3(m);
}

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) return;

    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 color = unpackColor(inputImage.pixels[index]);

    // 修正边界检查
    vec3 surroundingColors[4];
    if (coord.x >= 1 && coord.x < filterParams.width - 1 &&
    coord.y >= 1 && coord.y < filterParams.height - 1) {
        surroundingColors[0] = unpackColor(inputImage.pixels[index - 1]).rgb;
        surroundingColors[1] = unpackColor(inputImage.pixels[index + 1]).rgb;
        surroundingColors[2] = unpackColor(inputImage.pixels[index - (filterParams.bytesPerLine / 4)]).rgb;
        surroundingColors[3] = unpackColor(inputImage.pixels[index + (filterParams.bytesPerLine / 4)]).rgb;
    } else {
        surroundingColors = vec3[4](color.rgb, color.rgb, color.rgb, color.rgb);
    }

    vec3 hsv = rgbToHsv(color.rgb);
    hsv.x = mod(hsv.x, 360.0); // 色相循环修正

    // 计算平均饱和度
    float sumS = 0.0;
    for (int i = 0; i < 4; ++i) {
        sumS += rgbToHsv(surroundingColors[i]).y;
    }
    float avgS = sumS / 4.0;

    // 参数处理
    float vib = clamp(filterParams.vibrance * (filterParams.vibrance < 0.0 ? 2.3 : 1.0), -100.0, 100.0);
    float s = hsv.y;

    // 饱和度调整
    if (hsv.x >= 0.0 && hsv.x <= 40.0) { // 肤色处理
                                         s *= (1.0 + (vib * 0.25 + filterParams.saturation) / 100.0);
    } else {
        float factor = vib / 100.0 * (1.0 - s);
        if (s > 0.001 && avgS > 0.001) factor *= (avgS / s * 0.5); // 安全除法
        s *= (1.0 + factor + filterParams.saturation / 100.0);
    }

    // 饱和度限制
    s = clamp(s, 0.01 * clamp(filterParams.saturation, 0.0, 100.0), 1.0);

    outputImage.pixels[index] = packColor(vec4(hsvToRgb(vec3(hsv.x, s, hsv.z)), color.a));
}