#ifndef DEBUG_H
#define DEBUG_H

//4K ROM 、1K RAM
#define DEBUG_VERSION "1.0.6"
/**
 * v1.0.6：优化内联和 MCU 配置项，修复 %p 指针大小转换 warning
*/
/**
 * v1.0.5：新增基础类型配置宏，用于适配不同芯片 SDK 的 uint32_t 底层类型
*/
/**
 * v1.0.4：新增 DEBUG_USE_LIBC，用于选择使用工具链自带的 libc 格式化和内存函数
*/
/**
 * v1.0.3：将打印总开关更改为 DEBUG_PRINTF
*/
/**
 * v1.0.2：修复了%f无法打印负数的问题
*/
/**
 * v1.0.1：修复了循环打印出错的问题
*/

#define LVL_ERROR   1
#define LVL_WARNING 2
#define LVL_INFO    3
#define LVL_DEBUG   4

//使用工具链自带的 libc 函数。默认关闭，保持无库依赖。
#ifndef DEBUG_USE_LIBC
#define DEBUG_USE_LIBC          0
#endif

#ifndef DEBUG_USE_COLOR
#define DEBUG_USE_COLOR         1
#endif

#ifndef DEBUG_PRINT_VERSION
#define DEBUG_PRINT_VERSION     1
#endif

#ifndef DEBUG_ENABLE_FLOAT
#define DEBUG_ENABLE_FLOAT      1
#endif

#ifndef DEBUG_FORCE_INLINE
#define DEBUG_FORCE_INLINE      0
#endif

#ifndef DEBUG_INLINE
#if DEBUG_FORCE_INLINE && (defined(__GNUC__) || defined(__clang__))
#define DEBUG_INLINE            static inline __attribute__((always_inline))
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(_MSC_VER)
#define DEBUG_INLINE            static __inline
#else
#define DEBUG_INLINE            static inline
#endif
#endif

//基础类型配置。默认不依赖 stdint.h；如果 SDK 的 uint32_t 是 unsigned long，
//可在工程编译宏中定义 DEBUG_U32_USE_UNSIGNED_LONG=1。
#ifndef DEBUG_U32_USE_UNSIGNED_LONG
#define DEBUG_U32_USE_UNSIGNED_LONG 0
#endif

#ifndef DEBUG_U8_TYPE
#define DEBUG_U8_TYPE           unsigned char
#endif

#ifndef DEBUG_U16_TYPE
#define DEBUG_U16_TYPE          unsigned short int
#endif

#ifndef DEBUG_U32_TYPE
#if DEBUG_U32_USE_UNSIGNED_LONG
#define DEBUG_U32_TYPE          unsigned long
#else
#define DEBUG_U32_TYPE          unsigned int
#endif
#endif

#ifndef DEBUG_INT_TYPE
#define DEBUG_INT_TYPE          int
#endif

#ifndef DEBUG_SIZE_TYPE
#define DEBUG_SIZE_TYPE         unsigned int
#endif

#ifndef DEBUG_PRINTF_LEN_TYPE
#define DEBUG_PRINTF_LEN_TYPE   int
#endif

#ifndef DEBUG_CRITICAL_TYPE
#define DEBUG_CRITICAL_TYPE     DEBUG_U32_TYPE
#endif

#ifndef DEBUG_TIME_TYPE
#define DEBUG_TIME_TYPE         DEBUG_U32_TYPE
#endif

typedef DEBUG_U8_TYPE debug_u8_t;
typedef DEBUG_U16_TYPE debug_u16_t;
typedef DEBUG_U32_TYPE debug_u32_t;
typedef DEBUG_INT_TYPE debug_int_t;
typedef DEBUG_SIZE_TYPE debug_size_t;
typedef DEBUG_PRINTF_LEN_TYPE debug_printf_len_t;
typedef DEBUG_CRITICAL_TYPE debug_critical_t;
typedef DEBUG_TIME_TYPE debug_time_t;

#ifndef DEBUG_FORMAT_CHECK
#define DEBUG_FORMAT_CHECK      DEBUG_USE_LIBC
#endif

#if DEBUG_FORMAT_CHECK && (defined(__GNUC__) || defined(__clang__))
#define DEBUG_PRINTF_FORMAT(fmt_index, first_arg) __attribute__((format(printf, fmt_index, first_arg)))
#else
#define DEBUG_PRINTF_FORMAT(fmt_index, first_arg)
#endif

#ifndef DEBUG_POINTER_LITTLE_ENDIAN
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define DEBUG_POINTER_LITTLE_ENDIAN 0
#else
#define DEBUG_POINTER_LITTLE_ENDIAN 1
#endif
#endif

//打印缓冲区大小,最小64字节，一般设置为128字节就够用了
#ifndef PRINTF_BUFFER_SIZE
#define PRINTF_BUFFER_SIZE      128
#endif
//打印缓冲区数量，一般设置为10就够用了
#ifndef PRINTF_BUFFER_NUM
#define PRINTF_BUFFER_NUM       10
#endif

//每次 debug_log_flush 最多输出几条日志；0 表示一次全部输出，保持旧行为。
#ifndef DEBUG_FLUSH_MAX_PER_CALL
#define DEBUG_FLUSH_MAX_PER_CALL 0
#endif

#if PRINTF_BUFFER_SIZE < 64
#error "PRINTF_BUFFER_SIZE must be at least 64"
#endif

#if PRINTF_BUFFER_SIZE > 65535
#error "PRINTF_BUFFER_SIZE must be less than or equal to 65535"
#endif

#if PRINTF_BUFFER_NUM < 2
#error "PRINTF_BUFFER_NUM must be at least 2"
#endif

#if PRINTF_BUFFER_NUM > 65535
#error "PRINTF_BUFFER_NUM must be less than or equal to 65535"
#endif

//颜色定义，必须是10~99之间的数字
#ifndef COLOR_DEBUG
#define COLOR_DEBUG   36   // 青色
#endif
#ifndef COLOR_INFO
#define COLOR_INFO    32   // 绿色
#endif
#ifndef COLOR_WARNING
#define COLOR_WARNING 33   // 黄色
#endif
#ifndef COLOR_ERROR
#define COLOR_ERROR   31   // 红色
#endif

#if DEBUG_USE_COLOR && \
    (COLOR_DEBUG < 10 || COLOR_DEBUG > 99 || \
    COLOR_INFO < 10 || COLOR_INFO > 99 || \
    COLOR_WARNING < 10 || COLOR_WARNING > 99 || \
    COLOR_ERROR < 10 || COLOR_ERROR > 99)
#error "Debug color values must be between 10 and 99"
#endif

#if DEBUG_USE_LIBC
#include <string.h>
#endif

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

DEBUG_INLINE char * dbg_strrchr(const char *s, int c) {
#if DEBUG_USE_LIBC
    return (char *)strrchr(s, c);
#else
    const char *last = 0;
    unsigned char uc = (unsigned char)c;
    while (*s) {
        if ((unsigned char)*s == uc) {
            last = s;
        }
        s++;
    }

    if (uc == '\0') {
        return (char *)s;
    }

    return (char *)last;
#endif
}

#define __FILENAME__ (dbg_strrchr(__FILE__, '/') ? dbg_strrchr(__FILE__, '/') + 1 : \
                      (dbg_strrchr(__FILE__, '\\') ? dbg_strrchr(__FILE__, '\\') + 1 : __FILE__))

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL LVL_WARNING
#endif

#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF    1
#endif

typedef void(*printf_str_func)(char *,debug_printf_len_t);
typedef debug_critical_t(*enter_critical_func)(void);
typedef void(*exit_critical_func)(debug_critical_t);
typedef debug_time_t(*get_time_ms_func)(void);

void debug_printf(const char *color, const char *color_end, const char * level, const char * file, const char * line, const char *format, ...) DEBUG_PRINTF_FORMAT(6, 7);
void debug_printf_hex(const char *color, const char *color_end, const char * level, const char * file, const char * line, const debug_u8_t * hex_buff, debug_size_t buff_len);
void debug_printf_raw(const char *format, ...) DEBUG_PRINTF_FORMAT(1, 2);

#if DEBUG_USE_COLOR
#define _DBG_COLOR(n)               "\033["STR(n)"m"
#define _DBG_LOG_X_END              "\033[0m\r\n"
#else
#define _DBG_COLOR(n)               ""
#define _DBG_LOG_X_END              "\r\n"
#endif

#define _DBG_LOG_COLOR_LEN         (sizeof(_DBG_COLOR(COLOR_ERROR))-1)
#define _DBG_LOG_X_END_LEN          (sizeof(_DBG_LOG_X_END)-1)

#define PRINTF_BUFFER_NUM_OVERFLOW_STR    _DBG_COLOR(COLOR_ERROR)"****** debug buffer overflow ******"_DBG_LOG_X_END
#define PRINTF_BUFFER_SIZE_OVERFLOW_STR    "...."

#define PRINTF_LINE(level,color,...)        do { debug_printf(_DBG_COLOR(color), _DBG_LOG_X_END, level , __FILENAME__, STR(__LINE__), __VA_ARGS__); } while (0)
#define PRINTF_HEX(level,color,data, len)   do { debug_printf_hex(_DBG_COLOR(color), _DBG_LOG_X_END, level , __FILENAME__, STR(__LINE__), data, len); } while (0)
#define PRINTF_RAW(...)                     do { debug_printf_raw(__VA_ARGS__); } while (0)

#if DEBUG_PRINTF
    #define DEBUG_RAW(...)    PRINTF_RAW(__VA_ARGS__)
    #if DEBUG_LEVEL >= LVL_ERROR
        #define DEBUG_E(...)    PRINTF_LINE("E",COLOR_ERROR,__VA_ARGS__)
        #define DEBUG_E_HEX(data, len) PRINTF_HEX("E",COLOR_ERROR,data, len)
    #else
        #define DEBUG_E(...)    do { } while (0)
        #define DEBUG_E_HEX(data, len) do { } while (0)
    #endif
    #if DEBUG_LEVEL >= LVL_WARNING
        #define DEBUG_W(...)    PRINTF_LINE("W",COLOR_WARNING,__VA_ARGS__)
        #define DEBUG_W_HEX(data, len) PRINTF_HEX("W",COLOR_WARNING,data, len)
    #else
        #define DEBUG_W(...)    do { } while (0)
        #define DEBUG_W_HEX(data, len) do { } while (0)
    #endif
    #if DEBUG_LEVEL >= LVL_INFO
        #define DEBUG_I(...)    PRINTF_LINE("I",COLOR_INFO,__VA_ARGS__)
        #define DEBUG_I_HEX(data, len) PRINTF_HEX("I",COLOR_INFO,data, len)
    #else
        #define DEBUG_I(...)    do { } while (0)
        #define DEBUG_I_HEX(data, len) do { } while (0)
    #endif
    #if DEBUG_LEVEL >= LVL_DEBUG
        #define DEBUG_D(...)    PRINTF_LINE("D",COLOR_DEBUG,__VA_ARGS__)
        #define DEBUG_D_HEX(data, len) PRINTF_HEX("D",COLOR_DEBUG,data, len)
    #else
        #define DEBUG_D(...)    do { } while (0)
        #define DEBUG_D_HEX(data, len) do { } while (0)
    #endif
#else
    #define DEBUG_RAW(...)       do { } while (0)
    #define DEBUG_E(...)         do { } while (0)
    #define DEBUG_E_HEX(data, len) do { } while (0)
    #define DEBUG_W(...)         do { } while (0)
    #define DEBUG_W_HEX(data, len) do { } while (0)
    #define DEBUG_I(...)         do { } while (0)
    #define DEBUG_I_HEX(data, len) do { } while (0)
    #define DEBUG_D(...)         do { } while (0)
    #define DEBUG_D_HEX(data, len) do { } while (0)
#endif

/**@brief 初始化debug模块
 *
 * @param[in]   func_str    打印输出函数，必须是同步打印，不可使用异步打印函数
 * @param[in]   enter_critical 进入临界区函数
 * @param[in]   exit_critical  退出临界区函数
 * @param[in]   func_get_time_ms 获取系统时间函数，单位毫秒
 */
void debug_init(printf_str_func func_str,enter_critical_func func_enter_critical, exit_critical_func func_exit_critical, get_time_ms_func func_get_time_ms);

/**@brief 主循环调用或直接调用，将缓存区的内容一次性全部打印并清空缓存区
 *
 */
void debug_log_flush(void);

#endif

