add_library(cjson_stub STATIC)
target_sources(cjson_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cjson_stub.c
)
target_include_directories(cjson_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
