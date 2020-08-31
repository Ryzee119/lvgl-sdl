XBE_TITLE = lvgl-xbox
GEN_XISO = $(XBE_TITLE).iso
NXDK_SDL = y

#Include my files
SRCS += \
	$(CURDIR)/example.c \
	$(CURDIR)/input_test.c \
	$(CURDIR)/../lv_drv/lv_sdl_drv_input.c \
	$(CURDIR)/../lv_drv/lv_sdl_drv_display.c \
	$(CURDIR)/../lv_drv/lv_if_drv_filesystem.c
CFLAGS += \
	-I$(CURDIR) \
	-I$(CURDIR)/../ \
	-I$(CURDIR)/../lv_drv \
	-O2

#Include lvgl main library
SRCS +=	\
	$(wildcard $(CURDIR)/../lvgl/src/lv_core/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_draw/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_font/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_gpu/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_hal/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_misc/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_themes/*.c) \
	$(wildcard $(CURDIR)/../lvgl/src/lv_widgets/*.c)
CFLAGS += \
	-I$(CURDIR)/../lvgl/src \

#Include lv_examples
SRCS +=	\
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_benchmark/*.c) \
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_stress/*.c) \
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_widgets/*.c) \
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_ex_get_started/*.c) \
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_ex_style/*.c) \
	$(wildcard $(CURDIR)/lv_examples/src/lv_demo_ex_widgets/*.c) \
	$(wildcard $(CURDIR)/lv_examples/assets/*.c)
CFLAGS += \
	-I$(CURDIR)/lv_examples

include $(NXDK_DIR)/Makefile
