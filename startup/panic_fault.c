#include "panic_fault.h"

#include "board_uart.h"
#include "gd32h7xx.h"
#include "gd32h7xx_syscfg.h"

#include <stddef.h>
#include <stdint.h>

#define PANIC_FAULT_FPSCR_EXCEPTION_MASK 0x0000009FU

typedef struct {
    uint32_t mask;
    const char *name;
} panic_fault_bit_name_t;

static volatile uint32_t panic_fault_active;
static uint8_t panic_fault_unalign_buffer[8] __attribute__((aligned(4)));

static void panic_write(const char *text)
{
    int size = 0;

    if(text == NULL) {
        return;
    }

    while(text[size] != '\0') {
        size++;
    }

    board_uart_panic_write_buffer(text, size);
}

static void panic_write_hex32(uint32_t value)
{
    char text[10];
    uint32_t i;

    text[0] = '0';
    text[1] = 'x';
    for(i = 0U; i < 8U; i++) {
        uint32_t nibble = (value >> ((7U - i) * 4U)) & 0xFU;
        text[2U + i] = (char)((nibble < 10U) ? ('0' + nibble) : ('A' + nibble - 10U));
    }

    board_uart_panic_write_buffer(text, (int)sizeof(text));
}

static void panic_write_reg(const char *name, uint32_t value)
{
    panic_write("  ");
    panic_write(name);
    panic_write(" = ");
    panic_write_hex32(value);
    panic_write("\r\n");
}

static void panic_write_reg_pair(const char *name_a,
                                 uint32_t value_a,
                                 const char *name_b,
                                 uint32_t value_b)
{
    panic_write("  ");
    panic_write(name_a);
    panic_write(" = ");
    panic_write_hex32(value_a);
    panic_write("  ");
    panic_write(name_b);
    panic_write(" = ");
    panic_write_hex32(value_b);
    panic_write("\r\n");
}

static void panic_write_active_bits(const char *title,
                                    uint32_t value,
                                    const panic_fault_bit_name_t *bits,
                                    size_t count)
{
    size_t i;
    uint32_t printed = 0U;

    for(i = 0U; i < count; i++) {
        if((value & bits[i].mask) != 0U) {
            if(printed == 0U) {
                panic_write("  ");
                panic_write(title);
                panic_write(":");
            }
            panic_write(" ");
            panic_write(bits[i].name);
            printed = 1U;
        }
    }

    if(printed != 0U) {
        panic_write("\r\n");
    }
}

static void panic_write_exc_return(uint32_t exc_return)
{
    panic_write("  EXC_RETURN = ");
    panic_write_hex32(exc_return);
    panic_write("  stack=");
    panic_write(((exc_return & (1UL << 2U)) != 0U) ? "PSP" : "MSP");
    panic_write("  return=");
    panic_write(((exc_return & (1UL << 3U)) != 0U) ? "Thread" : "Handler");
    panic_write("  frame=");
    panic_write(((exc_return & (1UL << 4U)) != 0U) ? "basic" : "extended");
    panic_write("\r\n");
}

static void panic_write_stack_frame(const uint32_t *stack_frame)
{
    if(stack_frame == NULL) {
        panic_write("  stacked frame unavailable\r\n");
        return;
    }

    panic_write_reg_pair("R0 ", stack_frame[0], "R1 ", stack_frame[1]);
    panic_write_reg_pair("R2 ", stack_frame[2], "R3 ", stack_frame[3]);
    panic_write_reg_pair("R12", stack_frame[4], "LR ", stack_frame[5]);
    panic_write_reg_pair("PC ", stack_frame[6], "xPSR", stack_frame[7]);
}

static void panic_write_scb_status(uint32_t cfsr,
                                   uint32_t hfsr,
                                   uint32_t dfsr,
                                   uint32_t afsr,
                                   uint32_t mmfar,
                                   uint32_t bfar,
                                   uint32_t shcsr,
                                   uint32_t ccr,
                                   uint32_t icsr)
{
    static const panic_fault_bit_name_t cfsr_bits[] = {
        {SCB_CFSR_IACCVIOL_Msk, "IACCVIOL"},
        {SCB_CFSR_DACCVIOL_Msk, "DACCVIOL"},
        {SCB_CFSR_MUNSTKERR_Msk, "MUNSTKERR"},
        {SCB_CFSR_MSTKERR_Msk, "MSTKERR"},
        {SCB_CFSR_MLSPERR_Msk, "MLSPERR"},
        {SCB_CFSR_MMARVALID_Msk, "MMARVALID"},
        {SCB_CFSR_IBUSERR_Msk, "IBUSERR"},
        {SCB_CFSR_PRECISERR_Msk, "PRECISERR"},
        {SCB_CFSR_IMPRECISERR_Msk, "IMPRECISERR"},
        {SCB_CFSR_UNSTKERR_Msk, "UNSTKERR"},
        {SCB_CFSR_STKERR_Msk, "STKERR"},
        {SCB_CFSR_LSPERR_Msk, "LSPERR"},
        {SCB_CFSR_BFARVALID_Msk, "BFARVALID"},
        {SCB_CFSR_UNDEFINSTR_Msk, "UNDEFINSTR"},
        {SCB_CFSR_INVSTATE_Msk, "INVSTATE"},
        {SCB_CFSR_INVPC_Msk, "INVPC"},
        {SCB_CFSR_NOCP_Msk, "NOCP"},
        {SCB_CFSR_UNALIGNED_Msk, "UNALIGNED"},
        {SCB_CFSR_DIVBYZERO_Msk, "DIVBYZERO"},
    };
    static const panic_fault_bit_name_t hfsr_bits[] = {
        {SCB_HFSR_VECTTBL_Msk, "VECTTBL"},
        {SCB_HFSR_FORCED_Msk, "FORCED"},
        {SCB_HFSR_DEBUGEVT_Msk, "DEBUGEVT"},
    };
    static const panic_fault_bit_name_t dfsr_bits[] = {
        {SCB_DFSR_HALTED_Msk, "HALTED"},
        {SCB_DFSR_BKPT_Msk, "BKPT"},
        {SCB_DFSR_DWTTRAP_Msk, "DWTTRAP"},
        {SCB_DFSR_VCATCH_Msk, "VCATCH"},
        {SCB_DFSR_EXTERNAL_Msk, "EXTERNAL"},
    };
    static const panic_fault_bit_name_t shcsr_bits[] = {
        {SCB_SHCSR_MEMFAULTACT_Msk, "MEMFAULTACT"},
        {SCB_SHCSR_BUSFAULTACT_Msk, "BUSFAULTACT"},
        {SCB_SHCSR_USGFAULTACT_Msk, "USGFAULTACT"},
        {SCB_SHCSR_SVCALLACT_Msk, "SVCALLACT"},
        {SCB_SHCSR_MONITORACT_Msk, "MONITORACT"},
        {SCB_SHCSR_PENDSVACT_Msk, "PENDSVACT"},
        {SCB_SHCSR_SYSTICKACT_Msk, "SYSTICKACT"},
        {SCB_SHCSR_USGFAULTPENDED_Msk, "USGFAULTPENDED"},
        {SCB_SHCSR_MEMFAULTPENDED_Msk, "MEMFAULTPENDED"},
        {SCB_SHCSR_BUSFAULTPENDED_Msk, "BUSFAULTPENDED"},
        {SCB_SHCSR_SVCALLPENDED_Msk, "SVCALLPENDED"},
        {SCB_SHCSR_MEMFAULTENA_Msk, "MEMFAULTENA"},
        {SCB_SHCSR_BUSFAULTENA_Msk, "BUSFAULTENA"},
        {SCB_SHCSR_USGFAULTENA_Msk, "USGFAULTENA"},
    };

    panic_write_reg_pair("CFSR", cfsr, "HFSR", hfsr);
    panic_write_reg_pair("DFSR", dfsr, "AFSR", afsr);
    panic_write_reg_pair("SHCSR", shcsr, "CCR ", ccr);
    panic_write_reg("ICSR", icsr);
    panic_write_reg_pair("MMFSR", cfsr & 0x000000FFUL, "BFSR", (cfsr >> 8U) & 0x000000FFUL);
    panic_write_reg("UFSR", (cfsr >> 16U) & 0x0000FFFFUL);
    panic_write_active_bits("CFSR bits", cfsr, cfsr_bits, sizeof(cfsr_bits) / sizeof(cfsr_bits[0]));
    panic_write_active_bits("HFSR bits", hfsr, hfsr_bits, sizeof(hfsr_bits) / sizeof(hfsr_bits[0]));
    panic_write_active_bits("DFSR bits", dfsr, dfsr_bits, sizeof(dfsr_bits) / sizeof(dfsr_bits[0]));
    panic_write_active_bits("SHCSR bits", shcsr, shcsr_bits, sizeof(shcsr_bits) / sizeof(shcsr_bits[0]));

    if((cfsr & SCB_CFSR_MMARVALID_Msk) != 0U) {
        panic_write_reg("MMFAR", mmfar);
    }
    if((cfsr & SCB_CFSR_BFARVALID_Msk) != 0U) {
        panic_write_reg("BFAR", bfar);
    }
}

static void panic_write_fpu_status(void)
{
#if defined(__FPU_PRESENT) && (__FPU_PRESENT == 1U)
    panic_write_reg_pair("FPSCR", __get_FPSCR(), "FPCCR", FPU->FPCCR);
    panic_write_reg_pair("FPCAR", FPU->FPCAR, "FPDSCR", FPU->FPDSCR);
#endif
}

static void panic_fault_enable_traps(void)
{
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk |
                  SCB_SHCSR_BUSFAULTENA_Msk |
                  SCB_SHCSR_USGFAULTENA_Msk;
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk | SCB_CCR_UNALIGN_TRP_Msk;
    __DSB();
    __ISB();
}

static void panic_fault_clear_status(void)
{
    SCB->CFSR = 0xFFFFFFFFUL;
    SCB->HFSR = 0xFFFFFFFFUL;
    SCB->DFSR = 0xFFFFFFFFUL;
    SCB->AFSR = 0xFFFFFFFFUL;
}

void panic_fault_dump(const char *name,
                      const uint32_t *stack_frame,
                      uint32_t exc_return,
                      uint32_t msp,
                      uint32_t psp)
{
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;
    uint32_t dfsr = SCB->DFSR;
    uint32_t afsr = SCB->AFSR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t bfar = SCB->BFAR;
    uint32_t shcsr = SCB->SHCSR;
    uint32_t ccr = SCB->CCR;
    uint32_t icsr = SCB->ICSR;
    uint32_t control = __get_CONTROL();
    uint32_t ipsr = __get_IPSR();
    uint32_t primask = __get_PRIMASK();
    uint32_t basepri = __get_BASEPRI();
    uint32_t faultmask = __get_FAULTMASK();

    __disable_irq();

    if(panic_fault_active != 0U) {
        for(;;) {
            __NOP();
        }
    }
    panic_fault_active = 1U;

    panic_write("\r\n\r\n*** PANIC FAULT ***\r\n");
    panic_write("  exception = ");
    panic_write((name != NULL) ? name : "unknown");
    panic_write("\r\n");
    panic_write_exc_return(exc_return);
    panic_write_reg_pair("MSP", msp, "PSP", psp);
    panic_write_reg_pair("CONTROL", control, "IPSR", ipsr);
    panic_write_reg_pair("PRIMASK", primask, "BASEPRI", basepri);
    panic_write_reg("FAULTMASK", faultmask);
    panic_write_stack_frame(stack_frame);
    panic_write_scb_status(cfsr, hfsr, dfsr, afsr, mmfar, bfar, shcsr, ccr, icsr);
    panic_write_fpu_status();
    panic_write("  Use stacked PC with arm-none-eabi-addr2line to locate the fault site.\r\n");
    panic_write("*** SYSTEM HALTED ***\r\n");

    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_hard(void)
{
    panic_fault_clear_status();
    SCB->SHCSR &= ~(SCB_SHCSR_MEMFAULTENA_Msk |
                    SCB_SHCSR_BUSFAULTENA_Msk |
                    SCB_SHCSR_USGFAULTENA_Msk);
    __DSB();
    __ISB();
    __asm volatile (".short 0xDE00");
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_mem(void)
{
    volatile uint32_t value;

    panic_fault_clear_status();
    panic_fault_enable_traps();
    value = *(volatile uint32_t *)0x60000000UL;
    (void)value;
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_bus(void)
{
    volatile uint32_t value;

    panic_fault_clear_status();
    panic_fault_enable_traps();
    value = *(volatile uint32_t *)0xFFFFFFF0UL;
    (void)value;
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_usage(void)
{
    panic_fault_clear_status();
    panic_fault_enable_traps();
    __asm volatile (".short 0xDE00");
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_div0(void)
{
    panic_fault_clear_status();
    panic_fault_enable_traps();
    __asm volatile (
        "movs r0, #1\n"
        "movs r1, #0\n"
        "sdiv r0, r0, r1\n"
        :
        :
        : "r0", "r1");
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_unalign(void)
{
    panic_fault_clear_status();
    panic_fault_enable_traps();
    __asm volatile (
        "ldr r0, [%0]\n"
        :
        : "r"(&panic_fault_unalign_buffer[1])
        : "r0", "memory");
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_nmi(void)
{
    panic_fault_clear_status();
    SCB->ICSR = SCB_ICSR_NMIPENDSET_Msk;
    __DSB();
    __ISB();
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_debugmon(void)
{
    panic_fault_clear_status();
    CoreDebug->DEMCR |= CoreDebug_DEMCR_MON_EN_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_MON_PEND_Msk;
    __DSB();
    __ISB();
    for(;;) {
        __NOP();
    }
}

__attribute__((noinline, noreturn)) void panic_fault_trigger_fpu(void)
{
#if defined(__FPU_PRESENT) && (__FPU_PRESENT == 1U)
    volatile float numerator = 1.0F;
    volatile float denominator = 0.0F;
    volatile float result;

    panic_fault_clear_status();
    rcu_periph_clock_enable(RCU_SYSCFG);
    syscfg_fpu_interrupt_enable(SYSCFG_FPUINT_DIV0);
    nvic_irq_enable(FPU_IRQn, 5U, 0U);
    __set_FPSCR(__get_FPSCR() & ~PANIC_FAULT_FPSCR_EXCEPTION_MASK);
    result = numerator / denominator;
    (void)result;
    NVIC->ISPR[((uint32_t)FPU_IRQn >> 5UL)] = (uint32_t)(1UL << ((uint32_t)FPU_IRQn & 0x1FUL));
    __DSB();
    __ISB();
#endif
    for(;;) {
        __NOP();
    }
}
