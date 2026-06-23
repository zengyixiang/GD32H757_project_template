set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel")
set(FREERTOS_PLUS_CLI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Plus-CLI")

set(FREERTOS_PORT "GCC_ARM_CM7" CACHE STRING "FreeRTOS port name" FORCE)
set(FREERTOS_HEAP "4" CACHE STRING "FreeRTOS heap implementation" FORCE)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
    ${CMAKE_SOURCE_DIR}/config
)

add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

add_library(freertos_kernel_stub INTERFACE)
target_link_libraries(freertos_kernel_stub INTERFACE
    freertos_kernel
    freertos_config
)

add_library(freertos_cli STATIC)
target_sources(freertos_cli PRIVATE
    ${FREERTOS_PLUS_CLI_PATH}/FreeRTOS_CLI.c
)
target_include_directories(freertos_cli PUBLIC
    ${FREERTOS_PLUS_CLI_PATH}
)
target_link_libraries(freertos_cli PUBLIC
    freertos_kernel_stub
)
