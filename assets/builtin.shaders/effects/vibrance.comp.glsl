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
    float vibrance; // 饱和度调整参数
    float saturation; // 饱和度调整参数
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
    float((color) & 0xFF) / 255.0,
    float((color >> 8) & 0xFF) / 255.0,
    float((color >> 16) & 0xFF) / 255.0,
    float((color >> 24) & 0xFF) / 255.0
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
    if (any(greaterThanEqual(coord, uvec2(filterParams.width, filterParams.height)))) {
        return;
    }

    uint index = coord.y * (filterParams.bytesPerLine / 4) + coord.x;
    vec4 color = unpackColor(inputImage.pixels[index]);

    // 获取周围像素的颜色
    vec3 surroundingColors[4];
    if (coord.x > 3 && coord.x < filterParams.width - 3 && coord.y > 3 && coord.y < filterParams.height - 3) {
        uint leftIndex = index - 1;
        uint rightIndex = index + 1;
        uint upIndex = index - (filterParams.bytesPerLine / 4);
        uint downIndex = index + (filterParams.bytesPerLine / 4);

        surroundingColors[0] = unpackColor(inputImage.pixels[leftIndex]).rgb;
        surroundingColors[1] = unpackColor(inputImage.pixels[rightIndex]).rgb;
        surroundingColors[2] = unpackColor(inputImage.pixels[upIndex]).rgb;
        surroundingColors[3] = unpackColor(inputImage.pixels[downIndex]).rgb;
    } else {
        // 如果像素在边界上，使用当前像素的颜色
        surroundingColors[0] = color.rgb;
        surroundingColors[1] = color.rgb;
        surroundingColors[2] = color.rgb;
        surroundingColors[3] = color.rgb;
    }

    // 计算当前像素的HSV
    vec3 hsv = rgbToHsv(color.rgb);
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    // 计算周围像素的平均饱和度
    float sumSaturation = 0.0;
    for (int i = 0; i < 4; i++) {
        vec3 hsvSurrounding = rgbToHsv(surroundingColors[i]);
        sumSaturation += hsvSurrounding.y;
    }
    float avgSaturation = sumSaturation / 4.0;

    // 调整饱和度
    float saturationAdjusted = filterParams.vibrance;
    if (saturationAdjusted < 0.0) {
        saturationAdjusted *= 2.3;
    }

    if (h >= 0.0 && h <= 40.0) {
        // 肤色像素处理
        float factor = saturationAdjusted / 100.0 * 0.25;
        factor += filterParams.saturation / 100.0;
        s *= (1.0 + factor);
    } else {
        float factor = saturationAdjusted / 100.0 * (1.0 - s);
        if (avgSaturation > 0.0) {
            factor *= (avgSaturation / s * 0.5);
        }
        factor += filterParams.saturation / 100.0;
        s *= (1.0 + factor);
    }

    // 饱和度下限
    float saturationFloor = 0.01 * (filterParams.saturation + 1.0) / 255.0;
    if (s < saturationFloor) {
        s = saturationFloor;
    }

    // 限制饱和度范围
    s = clamp(s, 0.0, 1.0);

    // 转换回RGB
    vec3 adjustedColor = hsvToRgb(vec3(h, s, v));

    // 将结果写回输出缓冲区
    outputImage.pixels[index] = packColor(vec4(adjustedColor, color.a));
}