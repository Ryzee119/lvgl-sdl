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

#include <stdlib.h>
#include <stdio.h>
#include "lv_conf.h"
#include "lvgl/lvgl.h"
#include "lv_examples.h"
#include "lv_sdl_drv_display.h"
#include "lv_sdl_drv_input.h"
#include "lv_if_drv_filesystem.h"
#include "input_test.h"

#ifdef NXDK
#include <hal/debug.h>
#include <windows.h>
#define printf(fmt, ...) debugPrint(fmt, __VA_ARGS__)
#endif

void lv_demo_filesystem(void);

int main(void)
{
    lv_init();
    lv_sdl_init_display("My lvgl example");
    lv_sdl_init_input();
    

#if (LV_USE_DEMO_FILESYSTEM == 1)
    #if (LV_USE_FILESYSTEM == 0)
    #error "You must enable LV_USE_FILESYSTEM to use LV_USE_DEMO_FILESYSTEM"
    #else
    lv_if_init_filesystem('1');
    lv_demo_filesystem();
    #endif
#endif

#if (LV_USE_DEMO_BENCHMARK == 1)
    lv_demo_benchmark();
#elif (LV_USE_DEMO_STRESS == 1)
    lv_demo_stress();
#elif (LV_USE_DEMO_INPUT == 1)
    lv_demo_sdl_input();
#endif

    while (!get_quit_event())
    {
        lv_task_handler();
    }

    printf("Freeing resources and quitting\n");
    lv_sdl_deinit_input();
    lv_sdl_deinit_display();
    lv_deinit();
    return 0;
}

#if (LV_USE_DEMO_FILESYSTEM == 1 && LV_USE_FILESYSTEM == 1)
void lv_demo_filesystem(void)
{

    //Create a file system driver with an ID. (i.e. '1')
    lv_fs_drv_t *fs_drv = lv_fs_get_drv('1');
    void *file_p = malloc(sizeof(fs_drv->file_size));
    void *dir_p = malloc(sizeof(fs_drv->rddir_size));
    char fname[256];

    //Open a directory (Can use absolute path or relative)
    //Xbox must use absolute
    const char *dir_path = "./";
    fs_drv->dir_open_cb(fs_drv, dir_p, dir_path);

    //Test directory listing
    //Read all files/folders in a directory and print them
    printf("*** Listing all files in \"%s\" ***\n", dir_path);
    while (fs_drv->dir_read_cb(fs_drv, dir_p, fname) == LV_FS_RES_OK)
    {
        printf("%s\n", fname);
    }
    printf("\n\n");

    //Test writing a file, then reading the data back
    do
    {
        const char *TEST_FILE = "my_test_file.txt";
        const char *TEST_STRING = "Hello this is my lvgl project!\n";
        char readback_buf[256];

        printf("*** Testing write and readback to \"%s\" ***\n", TEST_FILE);
        //Create a new file, (overwrite if already exists)
        if (fs_drv->open_cb(fs_drv, file_p, TEST_FILE, LV_FS_MODE_WR) != LV_FS_RES_OK)
            break;

        uint32_t bytes_written; //Can also pass NULL for bytes_written
        if (fs_drv->write_cb(fs_drv, file_p, TEST_STRING, strlen(TEST_STRING), &bytes_written) != LV_FS_RES_OK)
            break;

        //Close the file
        fs_drv->close_cb(fs_drv, file_p);

        //Now let's read back what we just wrote
        if (fs_drv->open_cb(fs_drv, file_p, TEST_FILE, LV_FS_MODE_RD) != LV_FS_RES_OK)
            break;

        //Reading some data into buf
        uint32_t bytes_read; //Can also pass NULL for bytes_read
        fs_drv->read_cb(fs_drv, file_p, readback_buf, strlen(TEST_STRING), &bytes_read);

        //Close the file
        fs_drv->close_cb(fs_drv, file_p);

        //Print the data we just read
        printf("%s", readback_buf);

    } while (0);

    fs_drv->dir_close_cb(fs_drv, dir_p);
    free(file_p);
    free(dir_p);
}
#endif