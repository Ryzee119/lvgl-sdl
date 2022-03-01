//SPDX-License-Identifier: MIT

#ifdef NXDK
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "lv_port_indev.h"
#include "lvgl.h"

static lv_indev_drv_t indev_drv_gamepad;
static lv_indev_drv_t indev_drv_mouse;
static lv_indev_t *indev_mouse;
static lv_indev_t *indev_keypad;
static lv_obj_t *mouse_cursor;
static SDL_GameController *pad = NULL;
static int mouse_x, mouse_y;
static bool quit_event = false;
static bool mouse_event = false;
static bool mouse_pressed = false;
#ifndef MOUSE_SENSITIVITY
#define MOUSE_SENSITIVITY 50 //pixels per input poll LV_INDEV_DEF_READ_PERIOD
#endif
#ifndef MOUSE_DEADZONE
#define MOUSE_DEADZONE 10 //Percent
#endif

bool lv_quit_event()
{
    return quit_event;
}

static void mouse_read(lv_indev_drv_t *indev_drv_gamepad, lv_indev_data_t *data)
{
    if (pad == NULL)
    {
        return;
    }

    data->state = (mouse_pressed) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    //Event for a USB mouse
    if (mouse_event)
    {
        uint32_t buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        data->point.x = mouse_x;
        data->point.y = mouse_y;
        data->state |= (buttons & SDL_BUTTON_LMASK) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        mouse_event = false;
    }
    //From gamecontroller
    else
    {
        int x = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
        int y = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);

        if (SDL_abs(x) > (MOUSE_DEADZONE * 32768) / 100)
        {
            mouse_x += (x * MOUSE_SENSITIVITY) / 32768;
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > 640) mouse_x = 640;
        }

        if (SDL_abs(y) > (MOUSE_DEADZONE * 32768) / 100)
        {
            mouse_y += (y * MOUSE_SENSITIVITY) / 32768;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > 640) mouse_y = 640;
        }

        data->point.x = (int16_t)mouse_x;
        data->point.y = (int16_t)mouse_y;
    }
}

static void keypad_read(lv_indev_drv_t *indev_drv_gamepad, lv_indev_data_t *data)
{
    data->key = 0;

    static SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        if(e.type == SDL_WINDOWEVENT)
        {
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                quit_event = true;
            }
        }

        //Parse some mouse event while we are here.
        if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
        {
            mouse_event = true;
        }

        if((e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) && e.button.button == SDL_BUTTON_LEFT)
        {
            mouse_event = true;
            mouse_pressed = (e.type == SDL_MOUSEBUTTONDOWN);
        }

        if((e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP) && e.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
        {
            mouse_pressed = (e.type == SDL_CONTROLLERBUTTONDOWN);
        }

        if (e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_KEYDOWN)
        {
            data->state = LV_INDEV_STATE_PRESSED;
        }
        if (e.type == SDL_CONTROLLERBUTTONUP || e.type == SDL_KEYUP)
        {
            data->state = LV_INDEV_STATE_RELEASED;
        }
        if (e.type == SDL_CONTROLLERDEVICEADDED)
        {
            SDL_GameController *new_pad = SDL_GameControllerOpen(e.cdevice.which);
            if (pad == NULL)
            {
                pad = new_pad;
            }
        }
        if (e.type == SDL_CONTROLLERDEVICEREMOVED)
        {
            if (pad == SDL_GameControllerFromInstanceID(e.cdevice.which))
            {
                pad = NULL;
            }
            SDL_GameControllerClose(SDL_GameControllerFromInstanceID(e.cdevice.which));
        }
        //Gamecontroller event
        if (e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP)
        {
            pad = (SDL_GameControllerFromInstanceID(e.cdevice.which));
            switch (e.cbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_A:
                data->key = LV_KEY_ENTER;
                break;
            case SDL_CONTROLLER_BUTTON_B:
                data->key = LV_KEY_ESC;
                break;
            case SDL_CONTROLLER_BUTTON_X:
                data->key = LV_KEY_BACKSPACE;
                break;
            case SDL_CONTROLLER_BUTTON_Y:
                data->key = LV_KEY_HOME;
                break;
            case SDL_CONTROLLER_BUTTON_BACK:
                data->key = LV_KEY_PREV;
                break;
            case SDL_CONTROLLER_BUTTON_START:
                data->key = LV_KEY_NEXT;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                data->key = LV_KEY_NEXT;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                data->key = LV_KEY_PREV;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                data->key = LV_KEY_NEXT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                data->key = LV_KEY_UP;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                data->key = LV_KEY_DOWN;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                data->key = LV_KEY_LEFT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                data->key = LV_KEY_RIGHT;
                break;
            }
        }

        //Keyboard event
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                data->key = LV_KEY_ESC;
                break;
            case SDLK_BACKSPACE:
                data->key = LV_KEY_BACKSPACE;
                break;
            case SDLK_HOME:
                data->key = LV_KEY_HOME;
                break;
            case SDLK_RETURN:
                data->key = LV_KEY_ENTER;
                break;
            case SDLK_PAGEDOWN:
                data->key = LV_KEY_PREV;
                break;
            case SDLK_TAB:
                data->key = LV_KEY_NEXT;
                break;
            case SDLK_PAGEUP:
                data->key = LV_KEY_NEXT;
                break;
            case SDLK_UP:
                data->key = LV_KEY_UP;
                break;
            case SDLK_DOWN:
                data->key = LV_KEY_DOWN;
                break;
            case SDLK_LEFT:
                data->key = LV_KEY_LEFT;
                break;
            case SDLK_RIGHT:
                data->key = LV_KEY_RIGHT;
                break;
            }
        }
    }
    data->continue_reading = (SDL_PollEvent(NULL) != 0);
}

void lv_port_indev_init(bool use_mouse_cursor)
{
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        SDL_GameControllerOpen(i);
    }

    //Register the gamepad as a keypad
    lv_indev_drv_init(&indev_drv_gamepad);
    indev_drv_gamepad.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_gamepad.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv_gamepad);

    //Register a mouse cursor
    if (use_mouse_cursor)
    {
        lv_indev_drv_init(&indev_drv_mouse);
        indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
        indev_drv_mouse.read_cb = mouse_read;
        indev_mouse = lv_indev_drv_register(&indev_drv_mouse);
        mouse_cursor = lv_img_create(lv_scr_act());
        lv_img_set_src(mouse_cursor, LV_SYMBOL_PLUS);
        lv_indev_set_cursor(indev_mouse, mouse_cursor);
    }
    quit_event = false;
}

void lv_port_indev_deinit(void)
{
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}
