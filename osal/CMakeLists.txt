add_library(osal_lib STATIC)

target_sources(osal_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/osal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/osal_baremetal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/osal_freertos.c
)

target_include_directories(osal_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/config
)
target_link_libraries(osal_lib PRIVATE
    drivers_lib
)
