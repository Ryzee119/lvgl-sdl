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
#include <SDL.h>
#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "lv_sdl_drv_display.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_color_t buf_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];

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

    SDL_UpdateTexture(framebuffer, &r, color_p, r.w * ((LV_COLOR_DEPTH + 7) / 8));
    SDL_RenderCopy(renderer, framebuffer, &r, &r);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}

__attribute__((weak)) void display_wait_cb(lv_disp_drv_t *disp_drv)
{
    (void)disp_drv;
    //OPTIONAL: Called periodically while lvgl waits for an operation to be completed
    //          User can execute very simple tasks here or yield the task
}

lv_disp_t *lv_sdl_init_display(const char* win_name)
{
    //Setup the display buffer and driver
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.wait_cb = display_wait_cb;
    disp_drv.flush_cb = sdl_fb_flush;

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        printf("SDL_InitSubSystem failed: %s\n", SDL_GetError());

    window = SDL_CreateWindow(win_name,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              LV_HOR_RES_MAX, LV_VER_RES_MAX, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    framebuffer = SDL_CreateTexture(renderer,
                                    (LV_COLOR_DEPTH == 32) ? (SDL_PIXELFORMAT_ARGB8888) :
                                                             (SDL_PIXELFORMAT_RGB565),
                                    SDL_TEXTUREACCESS_STREAMING,
                                    LV_HOR_RES_MAX,
                                    LV_VER_RES_MAX);

    return lv_disp_drv_register(&disp_drv);
}

void lv_sdl_deinit_display(void)
{
    SDL_DestroyTexture(framebuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
