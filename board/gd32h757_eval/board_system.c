#include "board_system.h"

#include "gd32h7xx.h"

static void board_system_cache_enable(void);
static void board_system_interrupt_priority_init(void);
static void board_system_mpu_init(void);

void board_system_init(void)
{
    board_system_mpu_init();
    board_system_cache_enable();
    board_system_interrupt_priority_init();
}

static void board_system_cache_enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}

static void board_system_interrupt_priority_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
}

static void board_system_mpu_init(void)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    /* disable the MPU */
    ARM_MPU_Disable();
    ARM_MPU_SetRegion(0, 0);

    mpu_init_struct.region_base_address  = 0x0;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_4GB;
    mpu_init_struct.access_permission    = MPU_AP_NO_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_NON_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_NON_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER0;
    mpu_init_struct.subregion_disable    = 0x87; //0x87：对这几块区域生效 0x6000_0000 – 0x7FFF_FFFF 0x8000_0000 – 0x9FFF_FFFF 0xA000_0000 – 0xBFFF_FFFF 0xC000_0000 – 0xDFFF_FFFF
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_NOT_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    mpu_init_struct.region_base_address  = 0x24000000;
    mpu_init_struct.region_size          = MPU_REGION_SIZE_1MB;
    mpu_init_struct.access_permission    = MPU_AP_FULL_ACCESS;
    mpu_init_struct.access_bufferable    = MPU_ACCESS_BUFFERABLE;
    mpu_init_struct.access_cacheable     = MPU_ACCESS_CACHEABLE;
    mpu_init_struct.access_shareable     = MPU_ACCESS_NON_SHAREABLE;
    mpu_init_struct.region_number        = MPU_REGION_NUMBER1;
    mpu_init_struct.subregion_disable    = MPU_SUBREGION_ENABLE;
    mpu_init_struct.instruction_exec     = MPU_INSTRUCTION_EXEC_PERMIT;
    mpu_init_struct.tex_type             = MPU_TEX_TYPE0;
    mpu_region_config(&mpu_init_struct);
    mpu_region_enable();

    /* enable the MPU */
    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);   
}