add_compile_definitions(
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

add_compile_options(
    # EasyLogger uses __FILE__ when ELOG_FMT_USING_DIR is enabled.
    # Map the project root out at compile time so logs show app/foo.c instead
    # of the full host path, without changing the EasyLogger submodule.
    $<$<COMPILE_LANGUAGE:C>:-ffile-prefix-map=${CMAKE_SOURCE_DIR}/=>
)
