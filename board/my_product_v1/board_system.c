#include "board_system.h"

#include "gd32h7xx.h"
#include "gd32h7xx_fmc.h"

#define BOARD_SYSTEM_ITCM_SIZE_KB 64U
#define BOARD_SYSTEM_DTCM_SIZE_KB 128U

static void board_system_tcm_shared_ram_init(void);
static uint32_t board_system_itcm_ob_from_size(uint32_t size_kb);
static uint32_t board_system_dtcm_ob_from_size(uint32_t size_kb);
static void board_system_cache_enable(void);
static void board_system_interrupt_priority_init(void);
static void board_system_mpu_init(void);
static void board_system_fault_trap_init(void);

void board_system_init(void)
{
    board_system_tcm_shared_ram_init();
    board_system_mpu_init();
    board_system_fault_trap_init();
    board_system_cache_enable();
    board_system_interrupt_priority_init();
}

static void board_system_tcm_shared_ram_init(void)
{
    uint32_t current_itcm_ob = FMC_OBSTAT1_EFT & FMC_OBSTAT1_EFT_ITCM_SZ_SHRRAM;
    uint32_t current_dtcm_ob = FMC_OBSTAT1_EFT & FMC_OBSTAT1_EFT_DTCM_SZ_SHRRAM;
    uint32_t target_itcm_ob = board_system_itcm_ob_from_size(BOARD_SYSTEM_ITCM_SIZE_KB);
    uint32_t target_dtcm_ob = board_system_dtcm_ob_from_size(BOARD_SYSTEM_DTCM_SIZE_KB);

    if((current_itcm_ob == target_itcm_ob) && (current_dtcm_ob == target_dtcm_ob)) {
        return;
    }

    ob_unlock();
    if(ob_tcm_shared_ram_config(target_itcm_ob, target_dtcm_ob) == FMC_READY) {
        if(ob_start() == FMC_READY) {
            while((FMC_STAT & FMC_STAT_BUSY) != 0U) {
            }
            ob_lock();
            NVIC_SystemReset();
        }
    }
    ob_lock();
}

static uint32_t board_system_itcm_ob_from_size(uint32_t size_kb)
{
    switch(size_kb) {
    case 0U:
        return OB_ITCM_SHARED_RAM_0KB;
    case 64U:
        return OB_ITCM_SHARED_RAM_64KB;
    case 128U:
        return OB_ITCM_SHARED_RAM_128KB;
    case 256U:
        return OB_ITCM_SHARED_RAM_256KB;
    case 512U:
        return OB_ITCM_SHARED_RAM_512KB;
    default:
        return OB_ITCM_SHARED_RAM_64KB;
    }
}

static uint32_t board_system_dtcm_ob_from_size(uint32_t size_kb)
{
    switch(size_kb) {
    case 0U:
        return OB_DTCM_SHARED_RAM_0KB;
    case 64U:
        return OB_DTCM_SHARED_RAM_64KB;
    case 128U:
        return OB_DTCM_SHARED_RAM_128KB;
    case 256U:
        return OB_DTCM_SHARED_RAM_256KB;
    case 512U:
        return OB_DTCM_SHARED_RAM_512KB;
    default:
        return OB_DTCM_SHARED_RAM_128KB;
    }
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

static void board_system_fault_trap_init(void)
{
    // 如果不启用这些 Fault，很多错误会直接升级成 HardFault，不好区分具体原因
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk |   // 启用 MemManage Fault，比如 MPU 权限错误、非法内存区域访问
                  SCB_SHCSR_BUSFAULTENA_Msk |   // 启用 BusFault，比如访问外设/内存总线失败。
                  SCB_SHCSR_USGFAULTENA_Msk;    // 启用 UsageFault，比如非法指令、未定义状态、除零等。
    // SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;  // 启用 整数除以 0 触发异常，会触发 UsageFault。
    // SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk; // 启用非对齐访问陷阱，会触发 UsageFault，这个暂时不开不然USB无法使用因为官方的库就是非对其访问
    __DSB();
    __ISB();
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
