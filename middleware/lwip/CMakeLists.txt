add_library(lwip_stub STATIC)
target_sources(lwip_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip_port.c
)
target_include_directories(lwip_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
