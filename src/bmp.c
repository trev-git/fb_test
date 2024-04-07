/**
 * Copyright (c) 2024 trev-git
 * This program is published under the MIT license.
 * Refer to the LICENSE file for more information.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

#define BMP_IDENT 0x4d42 /* little-endian value, maybe make this code endianness-independent? */

BMPImage *bmp_read(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open BMP!");
        return NULL;
    }
    BitmapFileHeader *bfh = malloc(sizeof(BitmapFileHeader));

    /* Reading the bitmap file header */
    fread(&bfh->identifier, sizeof(bfh->identifier), 1, fp);
    if (bfh->identifier != BMP_IDENT)
    {
        fprintf(stderr, "Not a BMP file!");
        fclose(fp);
        return NULL;
    }
    fread(&bfh->file_size, sizeof(bfh->file_size), 1, fp);
    fread(&bfh->reserved1, sizeof(bfh->reserved1), 1, fp);
    fread(&bfh->reserved2, sizeof(bfh->reserved2), 1, fp);
    fread(&bfh->offset, sizeof(bfh->offset), 1, fp);

    /* Reading the DIB header */
    fread(&bfh->header_size, sizeof(bfh->header_size), 1, fp);
    fread(&bfh->width, sizeof(bfh->width), 1, fp);
    fread(&bfh->height, sizeof(bfh->height), 1, fp);
    fread(&bfh->planes, sizeof(bfh->planes), 1, fp);
    fread(&bfh->bpp, sizeof(bfh->bpp), 1, fp);
    fread(&bfh->compression_method, sizeof(bfh->compression_method), 1, fp);
    fread(&bfh->image_size, sizeof(bfh->image_size), 1, fp);
    fread(&bfh->x_res, sizeof(bfh->x_res), 1, fp);
    fread(&bfh->y_res, sizeof(bfh->y_res), 1, fp);
    fread(&bfh->colors, sizeof(bfh->colors), 1, fp);
    fread(&bfh->important_colors, sizeof(bfh->important_colors), 1, fp);

    /* We are immediately reading the pixel array, since the rest of the information isn't too important to us */
    fseek(fp, bfh->offset, SEEK_SET);

    /* The array of pixels is NOT going to have padding bytes */
    BMPPixels **pixels = malloc(sizeof(BMPPixels *) * bfh->height);
    for (int i = 0; i < bfh->height; i++)
    {
        pixels[i] = malloc(sizeof(BMPPixels) * bfh->width);
    }

    uint32_t row_size = (bfh->bpp*bfh->width+31)/32*4;
    uint32_t padding = row_size % 4;

    for (int i = 0; i < bfh->height; i++)
    {
        for (int j = 0; j < bfh->width; j++)
        {
            fread(&pixels[i][j], sizeof(uint8_t), 3, fp);
        }
        fseek(fp, padding, SEEK_CUR);
    }

    BMPImage *bmp = malloc(sizeof(BMPImage));
    bmp->header = bfh;
    bmp->pixels = pixels;

    fclose(fp);
    return bmp;
}

void bmp_close(BMPImage *bmp)
{
    for (int i = 0; i < bmp->header->height; i++)
        free(bmp->pixels[i]);
    free(bmp->pixels);
    free(bmp->header);
    free(bmp);
}

void bmp_flip_x(BMPImage *bmp)
{
    for (int i = 0; i < bmp->header->height / 2; i++)
    {
        BMPPixels *tmp = bmp->pixels[i];
        bmp->pixels[i] = bmp->pixels[bmp->header->height - i - 1];
        bmp->pixels[bmp->header->height - i - 1] = tmp;
    }
}
