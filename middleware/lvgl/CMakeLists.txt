add_library(lvgl_stub STATIC)
target_sources(lvgl_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lvgl_stub.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/lv_port_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/lv_port_indev.c
)
target_include_directories(lvgl_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/port
)
