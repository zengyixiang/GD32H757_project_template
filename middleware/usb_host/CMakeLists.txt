add_library(usb_host_stub STATIC)
target_sources(usb_host_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/usb_host.c
)
target_include_directories(usb_host_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
