cmake_minimum_required(VERSION 3.14)

project(lvgl_example LANGUAGES C CXX)

find_package(PkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)

set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")
set(LVGL_DIR "${PROJECT_DIR}/../lvgl")
set(LVDRV_DIR "${PROJECT_DIR}/../lvgl_drv")
set(LV_CONF_PATH ${PROJECT_DIR}/../lv_conf.h CACHE STRING "" FORCE)
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
add_definitions(-DLV_CONF_PATH=${LV_CONF_PATH})
add_library(lvglsdl ${LVDRV_DIR}/lv_sdl_disp.c ${LVDRV_DIR}/lv_port_indev.c)
target_include_directories(lvglsdl PUBLIC ${SDL2_INCLUDE_DIRS})

file(GLOB LVGL_DRIVER_FILES
    "${PROJECT_DIR}/main.c"
    "${PROJECT_DIR}/example.c"
    "${PROJECT_DIR}/assets/*.c"
)
add_compile_options(-Wall -O2)
add_executable(lvgl_example ${LVGL_DRIVER_FILES})

include_directories(${LVGL_DIR})
include_directories(${LVDRV_DIR})
include_directories(${PROJECT_DIR})

include(${LVGL_DIR}/CMakeLists.txt)

target_include_directories(lvgl_example PUBLIC ${SDL2_INCLUDE_DIRS})
target_compile_options(lvgl_example PUBLIC ${SDL2_CFLAGS_OTHER})
target_link_libraries(lvgl_example ${SDL2_LIBRARIES} lvglsdl lvgl::lvgl SDL2)
