/* MIT License
 * 
 * Copyright (c) [2020] [Ryan Wendland]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <assert.h>
#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "lv_sdl_drv_display.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
static lv_color_t buf_2[LV_HOR_RES_MAX * 10];

#ifdef NXDK
#include <hal/video.h>
static void xbox_fb_flush(lv_disp_drv_t *disp_drv,
                          const lv_area_t *area,
                          lv_color_t *color_p)
{
    VIDEO_MODE xmode = XVideoGetMode();
    void *fbp = (void *)XVideoGetFB();
    int bytespp = (xmode.bpp + 7) / 8;
    int width = xmode.width;

    fbp += (area->y1 * width * bytespp);
    for (int32_t y = area->y1; y <= area->y2; y++)
    {
        fbp += area->x1 * bytespp;
        for (int32_t x = area->x1; x <= area->x2; x++)
        {
            if (xmode.bpp == 15 || xmode.bpp == 16)
                *(uint16_t *)fbp = *((uint16_t *)(color_p));
            else
                *(uint32_t *)fbp = *((uint32_t *)(color_p));

            fbp += bytespp;
            color_p++;
        }
        fbp -= (area->x2 + 1) * bytespp;
        fbp += (width * bytespp);
    }
    lv_disp_flush_ready(disp_drv);
}

#if (LV_USE_GPU == 1)
static void xbox_gpu_fill_cb(lv_disp_drv_t *disp_drv,
                             lv_color_t *dest_buf,
                             const lv_area_t *dest_area,
                             const lv_area_t *fill_area,
                             lv_color_t color)
{
    //OPTIONAL: Fill a memory with a color (GPU only)
    assert(0); //Not implemented
}

static void xbox_gpu_blend_cb(lv_disp_drv_t *disp_drv,
                              lv_color_t *dest,
                              const lv_color_t *src,
                              uint32_t length,
                              lv_opa_t opa)
{
    //OPTIONAL: Blend two memories using opacity (GPU only)
    assert(0); //Not implemented
}
#endif //LV_USE_GPU

#else //USE SDL BACKEND
#include <SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *framebuffer = NULL;
static void sdl_fb_flush(lv_disp_drv_t *disp_drv,
                         const lv_area_t *area,
                         lv_color_t *color_p)
{
    SDL_Rect r;
    r.x = area->x1;
    r.y = area->y1;
    r.w = area->x2 - area->x1 + 1;
    r.h = area->y2 - area->y1 + 1;

#if (LV_COLOR_DEPTH == 32)
    SDL_UpdateTexture(framebuffer, &r, (uint32_t *)color_p, r.w * sizeof(uint32_t));
#else
    SDL_UpdateTexture(framebuffer, &r, (uint16_t *)color_p, r.w * sizeof(uint16_t));
#endif

    SDL_RenderCopy(renderer, framebuffer, &r, &r);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}
#endif //NXDK

__attribute__((weak)) void display_wait_cb(lv_disp_drv_t *disp_drv)
{
    (void)disp_drv;
    //OPTIONAL: Called periodically while lvgl waits for an operation to be completed
    //          User can execute very simple tasks here or yield the task
}

lv_disp_t *lv_sdl_init_display(const char* win_name)
{
    //Setup the display buffer and driver
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.wait_cb = display_wait_cb;

#ifdef NXDK //For Xbox I write directly to the framebuffer and skip the SDL Video Library
    XVideoSetMode(LV_HOR_RES_MAX, LV_VER_RES_MAX, LV_COLOR_DEPTH, REFRESH_DEFAULT);
    disp_drv.flush_cb = xbox_fb_flush;
#if (LV_USE_GPU == 1)
    //TODO: Init GPU etc...
    disp_drv.gpu_fill_cb = xbox_gpu_fill_cb;
    disp_drv.gpu_blend_cb = xbox_gpu_blend_cb;
#endif //LV_USE_GPU
#else  //Otherwise use the SDL Video backend
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        printf("SDL_InitSubSystem failed: %s\n", SDL_GetError());
    disp_drv.flush_cb = sdl_fb_flush;

    //TODO Create SDL renderer, Window, Texture
    window = SDL_CreateWindow(win_name,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              LV_HOR_RES_MAX, LV_VER_RES_MAX, 0);
#if (LV_USE_GPU == 1)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#else
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
#endif //LV_USE_GPU

    framebuffer = SDL_CreateTexture(renderer,
                                    (LV_COLOR_DEPTH == 32) ? (SDL_PIXELFORMAT_ARGB8888) : (SDL_PIXELFORMAT_RGB565),
                                    SDL_TEXTUREACCESS_STATIC,
                                    LV_HOR_RES_MAX,
                                    LV_VER_RES_MAX);

    SDL_SetTextureBlendMode(framebuffer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0x44, 0x44, 0x44, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);

#endif
    return lv_disp_drv_register(&disp_drv);
}

void lv_sdl_deinit_display(void)
{
#ifndef NXDK
    SDL_DestroyTexture(framebuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
#endif
}
//TODO: Deinit function for SDL stuff?
