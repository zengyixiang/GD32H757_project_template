set(FLASH_JLINK_CFG "${CMAKE_SOURCE_DIR}/scripts/jlink-swd.cfg")
set(FLASH_OPENOCD_CFG "${CMAKE_SOURCE_DIR}/scripts/flash_openocd.cfg")

function(gd32_add_flash_targets firmware_target)
    add_custom_target(flash_openocd
        COMMAND openocd
                -f "${FLASH_JLINK_CFG}"
                -f "${FLASH_OPENOCD_CFG}"
                -c "program $<TARGET_FILE:${firmware_target}> verify reset exit"
        DEPENDS ${firmware_target}
        COMMENT "Flashing ${firmware_target} with OpenOCD"
    )
endfunction()
