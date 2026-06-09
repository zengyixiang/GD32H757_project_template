add_library(cmsis_core INTERFACE)
target_include_directories(cmsis_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/Core/Include
)

add_library(gd32_device INTERFACE)
target_include_directories(gd32_device INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis/GD/GD32H7xx/Include
    ${CMAKE_SOURCE_DIR}/startup
    ${CMAKE_CURRENT_SOURCE_DIR}/gd32h7xx/Include
)
target_compile_definitions(gd32_device INTERFACE
    ${GD32_COMPILE_DEFINITIONS}
)
target_link_libraries(gd32_device INTERFACE
    cmsis_core
)

file(GLOB GD32_STDPERIPH_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/gd32h7xx/Source/*.c
)

add_library(gd32_stdperiph STATIC)
target_sources(gd32_stdperiph PRIVATE
    ${GD32_STDPERIPH_SOURCES}
)
target_include_directories(gd32_stdperiph PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gd32h7xx/Include
)
target_compile_definitions(gd32_stdperiph PUBLIC
    ${GD32_COMPILE_DEFINITIONS}
)
target_link_libraries(gd32_stdperiph PUBLIC
    gd32_device
)

add_library(drivers_lib INTERFACE)
target_link_libraries(drivers_lib INTERFACE
    gd32_stdperiph
)
