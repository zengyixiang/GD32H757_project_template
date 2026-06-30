/*!
    \file    gd32h7xx_it.c
    \brief   interrupt service routines

    \version 2026-02-04, V1.5.0, firmware for GD32H7xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32h7xx_it.h"

#include "board_uart.h"
#include "FreeRTOS.h"
#include "panic_fault.h"
#include "task.h"

void vPortSVCHandler(void);
void xPortPendSVHandler(void);
void xPortSysTickHandler(void);

#define DEFINE_PANIC_EXCEPTION_HANDLER(handler_name, c_handler_name, exception_name)             \
    static void c_handler_name(const uint32_t *stack_frame,                                      \
                               uint32_t exc_return,                                             \
                               uint32_t msp,                                                    \
                               uint32_t psp) __attribute__((used, noinline, noreturn));          \
    static void c_handler_name(const uint32_t *stack_frame,                                      \
                               uint32_t exc_return,                                             \
                               uint32_t msp,                                                    \
                               uint32_t psp)                                                     \
    {                                                                                            \
        panic_fault_dump(exception_name, stack_frame, exc_return, msp, psp);                     \
    }                                                                                            \
    __attribute__((naked)) void handler_name(void)                                               \
    {                                                                                            \
        __asm volatile (                                                                         \
            "tst lr, #4\n"                                                                       \
            "ite eq\n"                                                                           \
            "mrseq r0, msp\n"                                                                    \
            "mrsne r0, psp\n"                                                                    \
            "mov r1, lr\n"                                                                       \
            "mrs r2, msp\n"                                                                      \
            "mrs r3, psp\n"                                                                      \
            "b " #c_handler_name "\n");                                                         \
    }

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(NMI_Handler, panic_nmi_handler, "NMI")

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(HardFault_Handler, panic_hardfault_handler, "HardFault")

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(MemManage_Handler, panic_memmanage_handler, "MemManage")

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(BusFault_Handler, panic_busfault_handler, "BusFault")

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(UsageFault_Handler, panic_usagefault_handler, "UsageFault")

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(DebugMon_Handler, panic_debugmon_handler, "DebugMon")

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((naked)) void SVC_Handler(void)
{
    __asm volatile ("b vPortSVCHandler");
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile ("b xPortPendSVHandler");
}

/*!
    \brief      this function handles FPU exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
DEFINE_PANIC_EXCEPTION_HANDLER(FPU_IRQHandler, panic_fpu_handler, "FPU")

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

void USART0_IRQHandler(void)
{
    board_uart_irq_handler();
}

void USART2_IRQHandler(void)
{
    board_uart_irq_handler();
}

void DMA1_Channel7_IRQHandler(void)
{
    board_uart_dma_irq_handler();
}
