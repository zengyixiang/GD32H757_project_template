add_library(freertos_kernel_stub STATIC)
target_sources(freertos_kernel_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos_kernel.c
)
target_include_directories(freertos_kernel_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
