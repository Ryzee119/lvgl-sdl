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
#include <string.h>
#include "lv_ex_conf.h"
#include "lvgl.h"

#if LV_USE_DEMO_INPUT

static void msgbox_create(void);
static void indev_cb(struct _lv_indev_drv_t *indev, lv_event_t e);
static void msgbox_event_cb(lv_obj_t *msgbox, lv_event_t e);
static void main_list_cb(lv_obj_t *ta, lv_event_t e);

lv_indev_t *sdl_indev;
static lv_group_t *g;
static lv_obj_t *main_list;
static lv_obj_t *keyboard;
static lv_obj_t *tbox;
static lv_obj_t *t1;
static lv_obj_t *t2;
static lv_obj_t *t3;

struct
{
    lv_obj_t *btn;
    lv_obj_t *cb;
    lv_obj_t *slider;
    lv_obj_t *sw;
    lv_obj_t *spinbox;
    lv_obj_t *dropdown;
    lv_obj_t *roller;
    lv_obj_t *list;
} selector_objs;

struct
{
    lv_obj_t *ta1;
    lv_obj_t *ta2;
    lv_obj_t *kb;
} textinput_objs;

void lv_demo_sdl_input(void)
{
    //Create a object group
    g = lv_group_create();
    //lv_group_set_focus_cb(g, focus_cb);

    sdl_indev = lv_indev_get_next(NULL);
    sdl_indev->driver.feedback_cb = indev_cb;

    if (sdl_indev == NULL)
        return;

    lv_indev_set_group(sdl_indev, g);

    /* Create Main List on the Active Screen */
    main_list = lv_list_create(lv_scr_act(), NULL);
    //Align it
    lv_obj_set_height(main_list, lv_obj_get_height_fit(lv_scr_act()) - 40);
    lv_obj_align(main_list, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 20, 0);
    //Populate the list
    lv_list_add_btn(main_list, LV_SYMBOL_AUDIO, "Music");
    lv_list_add_btn(main_list, LV_SYMBOL_VIDEO, "Videos");
    lv_list_add_btn(main_list, LV_SYMBOL_LIST, "Applications");
    lv_list_add_btn(main_list, LV_SYMBOL_FILE, "Files");
    lv_list_add_btn(main_list, LV_SYMBOL_SETTINGS, "Settings");
    lv_list_add_btn(main_list, LV_SYMBOL_DOWNLOAD, "Downloads");
    lv_list_add_btn(main_list, LV_SYMBOL_POWER, "Power");
    //Register a Callback
    lv_obj_set_event_cb(main_list, main_list_cb);
    //Add it to the input group
    lv_group_add_obj(g, main_list);

    /* Create a text box widget */
    tbox = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_text(tbox, "");
    lv_textarea_set_cursor_hidden(tbox, true);
    lv_obj_set_size(tbox, 200, lv_obj_get_height_fit(lv_scr_act()) - 40);
    lv_obj_align(tbox, NULL, LV_ALIGN_IN_RIGHT_MID, -20, 0);
    //lv_group_add_obj(g, tbox);

    msgbox_create();
}

static void msgbox_create(void)
{
    lv_obj_t *mbox = lv_msgbox_create(lv_layer_top(), NULL);
    lv_msgbox_set_text(mbox, "Welcome to this input demo");
    lv_obj_set_event_cb(mbox, msgbox_event_cb);
    lv_group_add_obj(g, mbox);
    lv_group_focus_obj(mbox);
    lv_group_focus_freeze(g, true);

    static const char *btns[] = {"Ok", "Cancel", ""};
    lv_msgbox_add_btns(mbox, btns);
    lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_local_bg_opa(lv_layer_top(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);
    lv_obj_set_style_local_bg_color(lv_layer_top(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_obj_set_click(lv_layer_top(), true);
}

static void msgbox_event_cb(lv_obj_t *msgbox, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED)
    {
        uint16_t b = lv_msgbox_get_active_btn(msgbox);
        if (b == 0 || b == 1)
        {
            lv_obj_del(msgbox);
            lv_obj_reset_style_list(lv_layer_top(), LV_OBJ_PART_MAIN);
            lv_obj_set_click(lv_layer_top(), false);
            lv_event_send(main_list, LV_EVENT_REFRESH, NULL);
        }
    }
}

static void indev_cb(struct _lv_indev_drv_t *indev, lv_event_t e)
{
    char buf[64];
    if (e == LV_EVENT_KEY)
    {
        uint32_t key = lv_indev_get_key(sdl_indev);
        switch (key)
        {
            case LV_KEY_UP: strcpy(buf, "LV_KEY_UP\n"); break;
            case LV_KEY_DOWN: strcpy(buf, "LV_KEY_DOWN\n"); break;
            case LV_KEY_RIGHT: strcpy(buf, "LV_KEY_RIGHT\n"); break;
            case LV_KEY_LEFT: strcpy(buf, "LV_KEY_LEFT\n"); break;
            case LV_KEY_ESC: strcpy(buf, "LV_KEY_ESC\n"); break;
            case LV_KEY_DEL: strcpy(buf, "LV_KEY_DEL\n"); break;
            case LV_KEY_BACKSPACE: strcpy(buf, "LV_KEY_BACKSPACE\n"); break;
            case LV_KEY_ENTER: strcpy(buf, "LV_KEY_ENTER\n"); break;
            case LV_KEY_NEXT: strcpy(buf, "LV_KEY_NEXT\n"); break;
            case LV_KEY_PREV: strcpy(buf, "LV_KEY_PREV\n"); break;
            case LV_KEY_HOME: strcpy(buf, "LV_KEY_HOME\n"); break;
            case LV_KEY_END: strcpy(buf, "LV_KEY_END\n"); break;
            default: strcpy(buf, ""); break;
        }
        lv_textarea_add_text(tbox, buf);
    }
}

static void main_list_cb(lv_obj_t *ta, lv_event_t e)
{
    char buf[64];
    if (e == LV_EVENT_PRESSED)
    {
        lv_obj_t *btn = lv_list_get_btn_selected(ta);
        sprintf(buf, "Pressed %s\n", lv_list_get_btn_text(btn));
        lv_textarea_add_text(tbox, buf);
    }
}

#endif
