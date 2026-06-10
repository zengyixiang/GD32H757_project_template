file(GLOB BOARD_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

add_library(board_my_product_v1 STATIC)
target_sources(board_my_product_v1 PRIVATE
    ${BOARD_SOURCES}
)
target_include_directories(board_my_product_v1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(board_my_product_v1 PUBLIC
    bsp_lib
    components_lib
    middleware_lib
)
