cmake_minimum_required(VERSION 3.22)

set(CMAKE_PROJECT_NAME C8_GCC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${CMAKE_PROJECT_NAME} LANGUAGES C ASM)
message("Build type: " ${CMAKE_BUILD_TYPE})

add_executable(${CMAKE_PROJECT_NAME})

include(cmake/gd32h757.cmake)
include(cmake/common_flags.cmake)
include(cmake/flash_jlink.cmake)

add_subdirectory(drivers)
add_subdirectory(startup)
add_subdirectory(osal)
add_subdirectory(bsp)
add_subdirectory(middleware)
add_subdirectory(components)
add_subdirectory(board)
add_subdirectory(services)
add_subdirectory(app)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    app_lib
    services_lib
    project_board
    bsp_lib
    components_lib
    middleware_lib
    osal_lib
    drivers_lib
    ${TOOLCHAIN_LINK_LIBRARIES}
)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating .hex and .bin files"
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)
gd32_add_flash_targets(${CMAKE_PROJECT_NAME})

if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
