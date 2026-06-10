#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>

#define TO_STR(x)  #x
#define XSTR(x)    TO_STR(x)

#define JOIN(a, b) a##b

#define FAST_TEXT   __attribute__((section(".fast_text")))
#define FAST_RODATA __attribute__((section(".fast_rodata")))

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define unused(x) (void)(x)

#define FOREACH_U8(i, max)  for(uint8_t i = 0; i < (max); i++)
#define FOREACH_U16(i, max) for(uint16_t i = 0; i < (max); i++)
#define FOREACH_U32(i, max) for(uint32_t i = 0; i < (max); i++)

#define U8_4_TO_U32_LE(x1, x2, x3, x4) \
    (((uint32_t)(x1) << 0) | ((uint32_t)(x2) << 8) | ((uint32_t)(x3) << 16) | ((uint32_t)(x4) << 24))

static inline uintptr_t align_up(uintptr_t x, uintptr_t a)
{
    return (x + a - 1U) & ~(a - 1U);
}

static inline uintptr_t align_down(uintptr_t x, uintptr_t a)
{
    return x & ~(a - 1U);
}

#endif
