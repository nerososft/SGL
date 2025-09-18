import * as __sgl_image from './sgl_image';
export function gray(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number, g: number, b: number): __sgl_image.SglError;
export function threshold_split(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, bright: number): __sgl_image.SglError;
export function gaussian_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number): __sgl_image.SglError;
export function gaussian_blur_float(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number): __sgl_image.SglError;
export function surface_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number, th: number): __sgl_image.SglError;
export function distort_glass(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, scale: number, intensity: number, zoom: number): __sgl_image.SglError;
export function adjust_saturation(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, v: number, s: number): __sgl_image.SglError;
export function palette_knife(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number, s: number): __sgl_image.SglError;
export function hue_equal(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo): __sgl_image.SglError;
export function blur_edge(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, r: number, s: number, kernel_type: number): __sgl_image.SglError;
export function custom_kernel(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, k_i32ptr: number, radius: number, offset: number, scale: number): __sgl_image.SglError;
export function color_balance(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, adjustP_fptr: number, p_i32ptr: number, l: number): __sgl_image.SglError;
export function black_white(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, weight_fptr: number, wSize: number): __sgl_image.SglError;
export function scale(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, weight: number, height: number, type: number): __sgl_image.SglError;
export function color_separation(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, roff: number, goff: number, boff: number): __sgl_image.SglError;
export function mid_value(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, radius: number, threshold: number): __sgl_image.SglError;
export function path_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, vec_fptr: number, amount: number, width: number, height: number, startpos_fptr: number, endpos_fptr: number, startvec_fptr: number, endvec_fptr: number, num: number): __sgl_image.SglError;
export function crystallize(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, posx_fptr: number, posy_fptr: number, n: number): __sgl_image.SglError;
export function rotation_blur(in: __sgl_image.SglImageInfo, in2: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, x: number, y: number, a: number, b: number, ina: number, inb: number, strength: number, angle: number): __sgl_image.SglError;
export function facet(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, radius: number, intensitylevel: number): __sgl_image.SglError;
export function accented_edge(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, sobelx_i32ptr: number, sobely_i32ptr: number, size: number, type: number): __sgl_image.SglError;
export function iris_blur(in: __sgl_image.SglImageInfo, in2: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, x: number, y: number, a: number, b: number, ina: number, inb: number, angle: number): __sgl_image.SglError;
export function tilt_shift_blur(in: __sgl_image.SglImageInfo, in2: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, A_fptr: number, B_fptr: number, C_fptr: number, xoffset: number, yoffset: number, size: number): __sgl_image.SglError;
export function radial_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, sharpness: number, strength: number, xCenter: number, yCenter: number): __sgl_image.SglError;
export function rotational_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, angle: number, strength: number, x: number, y: number): __sgl_image.SglError;
export function minmax(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, radius: number, type: number): __sgl_image.SglError;
export function color_halftone(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, cyanAngle: number, yellowAngle: number, magentaAngle: number, radius: number, lookup_fptr: number): __sgl_image.SglError;
export function sharpen(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, kernel_i32ptr: number, size: number): __sgl_image.SglError;
export function polar_coordinates(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, type: number): __sgl_image.SglError;
export function clouds(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, permuteLookup_i32ptr: number, size: number, type: number): __sgl_image.SglError;
export function motion_blur(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, distance: number, angle: number, proportion: number): __sgl_image.SglError;
export function twirl_warp(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, angle: number): __sgl_image.SglError;
export function zigzag(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, wavelength: number, amplitude: number, type_wave: number, proportion: number): __sgl_image.SglError;
export function spherize(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, alpha: number, type: number): __sgl_image.SglError;
export function pinch(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, amount: number): __sgl_image.SglError;
export function wave(in: __sgl_image.SglImageInfo, out: __sgl_image.SglImageInfo, wavelength: number, amplitude: number, x_pro: number, y_pro: number, type: number, method: number): __sgl_image.SglError;
export interface SglBuffer {
    type: __sgl_image.SglBufferType;
    bufHandle_ptr: number;
    memHandle_ptr: number;
    data_ptr: number;
    bufferSize: number;
}
export interface SglImageGpuInfo {
    width: number;
    height: number;
    channels: number;
    bytesPerLine: number;
    gpuBuf: __sgl_image.SglBuffer;
}
export interface SglImageCpuInfo {
    width: number;
    height: number;
    channels: number;
    bytesPerLine: number;
    data_ptr: number;
}
export interface SglImageInfo {
    type: __sgl_image.SglImageType;
    info: __sgl_image.Info;
}
export enum SglError {
    SGL_SUCCESS = 0,
    SGL_FAILURE = 1,
    SGL_GPU_ERROR = 2,
    SGL_OUT_OF_MEMORY = 3,
    SGL_INVALID_ARGUMENT = 4,
}
export enum SglBufferType {
    SGL_BUFFER_TYPE_UNIFORM = 0,
    SGL_BUFFER_TYPE_STORAGE_LOCAL = 1,
    SGL_BUFFER_TYPE_STORAGE_SHARED = 2,
    SGL_BUFFER_TYPE_VERTEX = 3,
    SGL_BUFFER_TYPE_INDEX = 4,
    SGL_BUFFER_TYPE_UNKNOWN = 5,
}
export enum SglImageType {
    SGL_IMAGE_TYPE_CPU = 0,
    SGL_IMAGE_TYPE_GPU = 1,
    SGL_IMAGE_TYPE_UNKNOWN = 2,
}
export type Info = __sgl_image.SglImageGpuInfo | __sgl_image.SglImageCpuInfo;
