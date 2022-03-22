#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "stdio.h"
#include "example.h"

#ifdef NXDK
#define ROOT_PATH "A:D:"
#define TEST_FILE "A:D:\\text.txt"
#else
#define ROOT_PATH "A:."
#define TEST_FILE "A:text.txt"
#endif

#include <stdio.h>
void _log_cb(const char *buf)
{
    printf("%s", buf);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    lv_init();
    lv_log_register_print_cb(_log_cb);
    lv_port_disp_init(640, 480);
    lv_port_indev_init(true);
    lv_fs_stdio_init();

    lv_fs_dir_t dir;
    lv_log("Listing files and folders with the executable folder\n");
    if (lv_fs_dir_open(&dir, ROOT_PATH) == LV_FS_RES_OK)
    {
        char fname[256];
        while (lv_fs_dir_read(&dir, fname) == LV_FS_RES_OK)
        {
            if (strlen(fname) == 0)
            {
                break;
            }
            lv_log("%s", fname);
            lv_log("\n");
        }
        lv_log("\n");
        lv_fs_dir_close(&dir);
    }
    else
    {
        lv_log("Couldnt open dir\n");
    }

    lv_fs_file_t file;
    do
    {
        const char *test_string = "hello world!\n";
        char read_back[256] = {0};
        uint32_t brw;
        if (lv_fs_open(&file, TEST_FILE, LV_FS_MODE_WR) != LV_FS_RES_OK)
        {
            lv_log("Could not open file for write\n");
            break;
        }

        if (lv_fs_write(&file, test_string, strlen(test_string), &brw) != LV_FS_RES_OK)
        {
            lv_log("Could not write to file\n");
            break;
        }

        lv_fs_close(&file);

        if (lv_fs_open(&file, TEST_FILE, LV_FS_MODE_RD) != LV_FS_RES_OK)
        {
            lv_log("Could not open file for read\n");
            break;
        }

        if (lv_fs_read(&file, read_back, strlen(test_string), &brw) != LV_FS_RES_OK)
        {
            lv_log("Could not read back file\n");
            break;
        }

        lv_log("Wrote: ");
        lv_log("%s", test_string);
        lv_log("\n");
        lv_log("Read back: ");
        lv_log("%s", read_back);
        lv_log("\n");
    } while (0);

    lv_demo_keypad_encoder();

    while (lv_get_quit() == LV_QUIT_NONE)
    {
        lv_task_handler();
    }

    lv_port_disp_deinit();
    lv_port_indev_deinit();
    lv_deinit();
}
