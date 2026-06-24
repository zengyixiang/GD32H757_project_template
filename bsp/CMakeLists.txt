file(GLOB_RECURSE BSP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.c
)

add_library(bsp_lib STATIC)
target_sources(bsp_lib PRIVATE
    ${BSP_SOURCES}
)
target_include_directories(bsp_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/led
    ${CMAKE_CURRENT_SOURCE_DIR}/key
    ${CMAKE_CURRENT_SOURCE_DIR}/uart
    ${CMAKE_CURRENT_SOURCE_DIR}/spi
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c
    ${CMAKE_CURRENT_SOURCE_DIR}/adc
    ${CMAKE_CURRENT_SOURCE_DIR}/timer
    ${CMAKE_CURRENT_SOURCE_DIR}/usb
)
target_link_libraries(bsp_lib PUBLIC
    drivers_lib
    freertos_kernel_stub
)
