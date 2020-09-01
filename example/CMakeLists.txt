cmake_minimum_required(VERSION 3.7)
project(lvgl)

set(LVGL_DIR "${CMAKE_SOURCE_DIR}/../lvgl")
set(LVDRV_DIR "${CMAKE_SOURCE_DIR}/../lv_drv")
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O2")

add_definitions(-DMAKE_EXAMPLE)

# Add all the source files
file(GLOB LVGL_SOURCE_FILES
    # My lvgl driver source files
    "${LVDRV_DIR}/lv_sdl_drv_input.c"
    "${LVDRV_DIR}/lv_sdl_drv_display.c"
    "${LVDRV_DIR}/lv_if_drv_filesystem.c"

    # Main lvgl source files
    "${LVGL_DIR}/src/lv_core/*.c"
    "${LVGL_DIR}/src/lv_draw/*.c"
    "${LVGL_DIR}/src/lv_font/*.c"
    "${LVGL_DIR}/src/lv_gpu/*.c"
    "${LVGL_DIR}/src/lv_hal/*.c"
    "${LVGL_DIR}/src/lv_misc/*.c"
    "${LVGL_DIR}/src/lv_themes/*.c"
    "${LVGL_DIR}/src/lv_widgets/*.c"

    # My example project source files
    "${PROJECT_DIR}/example.c"
    "${PROJECT_DIR}/input_test.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_benchmark/*.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_stress/*.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_widgets/*.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_ex_get_started/*.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_ex_style/*.c"
    "${PROJECT_DIR}/lv_examples/src/lv_demo_ex_widgets/*.c"
    "${PROJECT_DIR}/lv_examples/assets/*.c"
)
add_executable(lvgl_example ${LVGL_SOURCE_FILES})

# Add SDL2 lib
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(lvgl_example ${SDL2_LIBRARY})

# My files includes
include_directories(${PROJECT_DIR})
include_directories(${PROJECT_DIR}/lv_examples/)

# My lvgl driver includes
include_directories(${LVDRV_DIR})

# Main lvgl lib includes
include_directories(${LVGL_DIR}/src/)
include_directories(${LVGL_DIR}/../)
