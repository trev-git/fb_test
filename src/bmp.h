/**
 * Copyright (c) 2024 trev-git
 * This program is published under the MIT license.
 * Refer to the LICENSE file for more information.
 */

#ifndef BMP_H
#define BMP_H

#include <stdint.h>

typedef struct __attribute__((packed)) BitmapFileHeader {
    /* Bitmap file header */
    uint16_t identifier; /* magic number 0x4d42 (ASCII: BM)*/
    uint32_t file_size; /* file size in bytes */
    uint16_t reserved1; /* reserved*/
    uint16_t reserved2;
    uint32_t offset; /* where is the pixel table from the beginning of the file */

    /* DIB header */
    uint32_t header_size; /* header size in bytes */
    int32_t width; /* image width */
    int32_t height; /* image height */
    uint16_t planes; /* color planes, always 1*/
    uint16_t bpp; /* bits per pixel */
    uint32_t compression_method; /* which compression method is being used, we don't really care */
    uint32_t image_size; /* size of the pixel table */
    int32_t x_res; /* horizontal resolution, pixel per metre */
    int32_t y_res; /* vertical resolution, pixel per metre */
    uint32_t colors; /* number of colors in palette */
    uint32_t important_colors; /* number of important colors, we don't really care */

    /* The DIB header has multiple versions, but this code only cares about the more common one BITMAPINFOHEADER, which is 40 bytes */
} BitmapFileHeader;

typedef struct __attribute__((packed)) BMPPixels {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} BMPPixels;

typedef struct __attribute__((packed)) BMPImage {
    BitmapFileHeader *header;
    BMPPixels **pixels;
} BMPImage;

BMPImage *bmp_read(const char *filename);
void bmp_close(BMPImage *bmp);
void bmp_flip_x(BMPImage *bmp);

#endif // BMP_H
