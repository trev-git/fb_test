/**
 * Copyright (c) 2024 trev-git
 * This program is published under the MIT license.
 * Refer to the LICENSE file for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

#include "bmp.h"

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t alpha;
} Color;

typedef struct {
    uint8_t *mem; /* Mapped framebuffer array */
    uint32_t bits_per_pixel; /* guess what */
    uint32_t width; /* Real width of the framebuffer */
    uint32_t height; /* Real height of the framebuffer */
    uint32_t line_length; /* Total length of a single line in bytes */
    uint32_t xoffset; /* The offset of the real framebuffer */
    uint32_t yoffset; /* from the virtual framebuffer */
    double aspect_ratio;
} Framebuffer;

void set_pixel(Framebuffer *fb, Vec2 coords, Color color);

int main(void)
{
    FILE *ffb = fopen("/dev/fb0", "r+");
    if (ffb == NULL)
    {
        fprintf(stderr, "Failed to open /dev/fb0!\n");
        exit(EXIT_FAILURE);
    }

    struct fb_var_screeninfo fb_vscreeninfo;
    struct fb_fix_screeninfo fb_fscreeninfo;

    if (ioctl(fileno(ffb), FBIOGET_VSCREENINFO, &fb_vscreeninfo) == -1)
    {
        fprintf(stderr, "Failed to get fb_var_screeninfo!\n");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fileno(ffb), FBIOGET_FSCREENINFO, &fb_fscreeninfo) == -1)
    {
        fprintf(stderr, "Failed to get fb_fix_screeninfo!\n");
        exit(EXIT_FAILURE);
    }

    uint8_t *fb_buf;
    fb_buf = mmap(NULL, fb_fscreeninfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(ffb), 0);
    if (fb_buf == NULL)
    {
        fprintf(stderr, "Failed to map /dev/fb0!");
        fclose(ffb);
        exit(EXIT_FAILURE);
    }

    Framebuffer fb;
    fb.mem = fb_buf;
    fb.bits_per_pixel = fb_vscreeninfo.bits_per_pixel;
    fb.width = fb_vscreeninfo.xres;
    fb.height = fb_vscreeninfo.yres;
    fb.line_length = fb_fscreeninfo.line_length;
    fb.xoffset = fb_vscreeninfo.xoffset;
    fb.yoffset = fb_vscreeninfo.yoffset;
    fb.aspect_ratio = (double)fb.width / fb.height;

    printf("Visible resolution: %dx%d\n", fb_vscreeninfo.xres, fb_vscreeninfo.yres);
    printf("Line length: %d\n", fb_fscreeninfo.line_length);
    printf("Bpp: %d\n", fb_vscreeninfo.bits_per_pixel);
    printf("Framebuffer size: %d\n", fb_fscreeninfo.smem_len);
    printf("Framebuffer offset: %d %d\n", fb_vscreeninfo.xoffset, fb_vscreeninfo.yoffset);

    system("clear");

    BMPImage *bmp = bmp_read("./nao_tomori.bmp");
    bmp_flip_x(bmp);
    BMPPixels **pixels = bmp->pixels;
    for (uint32_t i = 0; i < fb.width; i++)
    {
        for (uint32_t j = 0; j < fb.height; j++)
        {
            set_pixel(&fb, (Vec2){i, j}, (Color){pixels[j][i].r, pixels[j][i].g, pixels[j][i].b, 255});
        }
    }
    getchar();

    bmp_close(bmp);
    munmap(fb_buf, fb_fscreeninfo.smem_len);
    fclose(ffb);
}

void set_pixel(Framebuffer *fb, Vec2 coords, Color color)
{
    uint32_t location = (coords.x + fb->xoffset) * (fb->bits_per_pixel / 8) + (coords.y + fb->yoffset) * fb->line_length;
    (fb->mem)[location] = color.r;
    (fb->mem)[location+1] = color.g;
    (fb->mem)[location+2] = color.b;
    (fb->mem)[location+3] = color.alpha;
}
