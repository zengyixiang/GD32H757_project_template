target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_gd32h7xx.S
    ${CMAKE_CURRENT_SOURCE_DIR}/system_gd32h7xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gd32h7xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
