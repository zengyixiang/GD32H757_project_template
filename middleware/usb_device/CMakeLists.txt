add_library(usb_device_stub STATIC)
target_sources(usb_device_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_device.c
)
target_include_directories(usb_device_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
