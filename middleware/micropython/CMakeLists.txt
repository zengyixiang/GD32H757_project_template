add_library(micropython_stub STATIC)
target_sources(micropython_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/micropython_port.c
)
target_include_directories(micropython_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
