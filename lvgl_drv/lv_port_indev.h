
/**
 * @file lv_port_indev_templ.h
 *
 */

 /*Copy this file as "lv_port_indev.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "lvgl.h"
#ifdef NXDK
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    SDL_GameControllerButton sdl_map;
    lv_key_t lvgl_map;
} gamecontroller_map_t;

typedef struct
{
    SDL_Keycode sdl_map;
    lv_key_t lvgl_map;
} keyboard_map_t;
 
/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_indev_init(bool use_mouse_cursor);
void lv_port_indev_deinit(void);
bool lv_quit_event();
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*Disable/Enable content*/
