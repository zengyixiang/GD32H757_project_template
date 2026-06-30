#ifndef PANIC_FAULT_H
#define PANIC_FAULT_H

#include <stdint.h>

void panic_fault_dump(const char *name,
                      const uint32_t *stack_frame,
                      uint32_t exc_return,
                      uint32_t msp,
                      uint32_t psp) __attribute__((noreturn));

void panic_fault_trigger_hard(void) __attribute__((noreturn));
void panic_fault_trigger_mem(void) __attribute__((noreturn));
void panic_fault_trigger_bus(void) __attribute__((noreturn));
void panic_fault_trigger_usage(void) __attribute__((noreturn));
void panic_fault_trigger_div0(void) __attribute__((noreturn));
void panic_fault_trigger_unalign(void) __attribute__((noreturn));
void panic_fault_trigger_nmi(void) __attribute__((noreturn));
void panic_fault_trigger_debugmon(void) __attribute__((noreturn));
void panic_fault_trigger_fpu(void) __attribute__((noreturn));

#endif /* PANIC_FAULT_H */
