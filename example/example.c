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
#include "lvgl.h"
#include "lv_examples.h"
#include "lv_sdl_drv_display.h"
#include "lv_sdl_drv_input.h"
#include "lv_if_drv_filesystem.h"
#include "input_test.h"

#ifdef NXDK
#include <hal/video.h>
#include <hal/debug.h>
#include <windows.h>
#define printf(fmt, ...) debugPrint(fmt, __VA_ARGS__)
#endif

#if (LV_USE_FILESYSTEM == 1)
const char *fs_id = "0:";
#ifdef NXDK
const char *dir_path = "D:\\";
#else
const char *dir_path = "./";
#endif
#endif

static int width = 640;
static int height = 480;

void lv_demo_filesystem(void);

int main(void)
{
    #ifdef NXDK
    XVideoSetMode(width, height, LV_COLOR_DEPTH, REFRESH_DEFAULT);
    #endif

    lv_init();
    lv_sdl_init_display("My lvgl example", width, height);
    lv_sdl_init_input();

#if (LV_USE_DEMO_FILESYSTEM == 1)
    #if (LV_USE_FILESYSTEM == 0)
    #error "You must enable LV_USE_FILESYSTEM to use LV_USE_DEMO_FILESYSTEM"
    #else
    lv_if_init_filesystem(fs_id);
    lv_demo_filesystem();
    #endif
#endif

    printf("Starting demo...\n");
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
#if (LV_USE_DEMO_FILESYSTEM == 1)
    lv_if_deinit_filesystem(fs_id);
#endif
    lv_deinit();
    return 0;
}

#if (LV_USE_DEMO_FILESYSTEM == 1 && LV_USE_FILESYSTEM == 1)
//All file paths must be prefixed by the fs_id so the fs knows what driver to use
static char *create_path(const char* fs_id, const char *path)
{
    static char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", fs_id, path);
    return full_path;
}

void lv_demo_filesystem(void)
{

    //Open a directory (Must use relate path)
    lv_fs_dir_t dir_p;
    lv_fs_dir_open(&dir_p, create_path(fs_id, dir_path));
    char fname[256];

    //Test directory listing
    //Read all files/folders in a directory and print them
    printf("*** Listing all files in \"%s\" ***\n", create_path(fs_id, dir_path));
    while (lv_fs_dir_read(&dir_p, fname) == LV_FS_RES_OK)
    {
        printf("%s\n", fname);
    }
    printf("\n\n");

    //Test writing a file, then reading the data back
    lv_fs_file_t file_p;
    const char *TEST_FILE = "my_test_file.txt"; //Need to prefix all with the fs_id
    const char *TEST_STRING = "Hello this is my lvgl project!\n";
    char readback_buf[256] = {0};
    uint32_t bytes_written = 0;
    uint32_t bytes_read = 0;
    do
    {
        printf("*** Testing write and readback to \"%s\" ***\n", TEST_FILE);
        //Create a new file, (overwrite if already exists)
        if (lv_fs_open(&file_p, create_path(fs_id, TEST_FILE), LV_FS_MODE_WR) != LV_FS_RES_OK)
            break;

        if (lv_fs_write(&file_p, TEST_STRING, strlen(TEST_STRING) + 1, &bytes_written) != LV_FS_RES_OK)
            break;

        //Close the file
        lv_fs_close(&file_p);

        //Now let's read back what we just wrote
        if (lv_fs_open(&file_p, create_path(fs_id, TEST_FILE), LV_FS_MODE_RD) != LV_FS_RES_OK)
            break;

        //Reading some data into buf
        lv_fs_read(&file_p, readback_buf, strlen(TEST_STRING), &bytes_read);

        //Close the file
        lv_fs_close(&file_p);

        //Print the data we just read
        printf("%s", readback_buf);
    } while (0);

    if (bytes_written == strlen(TEST_STRING))
        printf("WRITE SUCCESSFUL\n");
    else
        printf("WRITE FAILED\n");
    
    if (strcmp(readback_buf, TEST_STRING) == 0)
        printf("READBACK SUCCESSFUL\n");
    else
        printf("READBACK FAILED\n");

    #ifdef NXDK
    Sleep(2000);
    #endif

    lv_fs_dir_close(&dir_p);
}
#endif
