add_subdirectory(freertos)
add_subdirectory(log)
add_subdirectory(fatfs)
add_subdirectory(cjson)
add_subdirectory(lwip)
add_subdirectory(lvgl)
add_subdirectory(usb_common)
add_subdirectory(usb_device)
add_subdirectory(usb_host)
add_subdirectory(micropython)

add_library(middleware_lib INTERFACE)
target_link_libraries(middleware_lib INTERFACE
    freertos_kernel_stub
    log_lib
    fatfs_stub
    cjson_stub
    lwip_stub
    lvgl_stub
    usb_common_stub
    usb_device_stub
    usb_host_stub
    micropython_stub
)
