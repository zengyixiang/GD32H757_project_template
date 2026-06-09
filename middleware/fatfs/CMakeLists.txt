add_library(fatfs_stub STATIC)
target_sources(fatfs_stub PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs_stub.c
)
target_include_directories(fatfs_stub PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
