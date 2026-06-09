set(PROJECT_BOARD "gd32h757_eval" CACHE STRING "Selected board: gd32h757_eval or my_product_v1")
set_property(CACHE PROJECT_BOARD PROPERTY STRINGS gd32h757_eval my_product_v1)

set(PROJECT_BOARD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_BOARD}")
if(NOT EXISTS "${PROJECT_BOARD_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Unknown PROJECT_BOARD='${PROJECT_BOARD}'. Expected one of: gd32h757_eval, my_product_v1")
endif()

string(MAKE_C_IDENTIFIER "${PROJECT_BOARD}" PROJECT_BOARD_ID)
set(PROJECT_BOARD_TARGET "board_${PROJECT_BOARD_ID}")

message(STATUS "Selected board: ${PROJECT_BOARD}")

add_subdirectory(${PROJECT_BOARD})

add_library(project_board INTERFACE)
target_link_libraries(project_board INTERFACE
    ${PROJECT_BOARD_TARGET}
)

if(PROJECT_BOARD STREQUAL "gd32h757_eval")
    target_compile_definitions(project_board INTERFACE
        PROJECT_BOARD_NAME=\"gd32h757_eval\"
    )
elseif(PROJECT_BOARD STREQUAL "my_product_v1")
    target_compile_definitions(project_board INTERFACE
        PROJECT_BOARD_NAME=\"my_product_v1\"
    )
endif()
