#include <stdarg.h>
#include "debug.h"

#if DEBUG_USE_LIBC
#include <stdio.h>
#include <string.h>
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

static printf_str_func printf_str = NULL;
static enter_critical_func enter_critical = NULL;
static exit_critical_func exit_critical = NULL;
static get_time_ms_func get_time_ms = NULL;
#if !DEBUG_USE_LIBC
static const char small_digits[] = "0123456789abcdef";
#endif
static const char large_digits[] = "0123456789ABCDEF";
static char *reserve_printf_buf(debug_u16_t *out_idx);

struct printf_buffer {
    char buffer[PRINTF_BUFFER_NUM][PRINTF_BUFFER_SIZE];
    debug_u16_t w_P,r_P;
    debug_u16_t len[PRINTF_BUFFER_NUM];
};
static struct printf_buffer pbuf;

DEBUG_INLINE debug_critical_t debug_enter_critical(void)
{
    if (enter_critical == NULL) {
        return 0;
    }

    return enter_critical();
}

DEBUG_INLINE void debug_exit_critical(debug_critical_t state)
{
    if (exit_critical != NULL) {
        exit_critical(state);
    }
}

DEBUG_INLINE debug_time_t debug_get_time_ms(void)
{
    if (get_time_ms == NULL) {
        return 0;
    }

    return get_time_ms();
}

#if DEBUG_USE_LIBC
DEBUG_INLINE void * dbg_memcpy(void *dest, const void *src, debug_size_t n) {
    return memcpy(dest, src, n);
}

DEBUG_INLINE char * dbg_strcpy(char *dest, const char *src) {
    dbg_memcpy(dest, src, (debug_size_t)strlen(src) + 1);
    return dest;
}

DEBUG_INLINE debug_size_t dbg_strlen(const char *str) {
    return (debug_size_t)strlen(str);
}

DEBUG_INLINE void * dbg_memset(void *s, int c, debug_size_t n) {
    return memset(s, c, n);
}

static int dbg_vsnprintf(char *buf, debug_size_t size, const char *fmt, va_list args)
{
    int ret = vsnprintf(buf, size, fmt, args);
    if (ret < 0) {
        if (size > 0) {
            buf[0] = '\0';
        }
        return 0;
    }

    return ret;
}
#else
DEBUG_INLINE void * dbg_memcpy(void *dest, const void *src, debug_size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    for (debug_size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

DEBUG_INLINE char * dbg_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

DEBUG_INLINE debug_size_t dbg_strlen(const char *str) {
    debug_size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
DEBUG_INLINE void * dbg_memset(void *s, int c, debug_size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

DEBUG_INLINE int dbg_abs(int n) {
    if (n < 0) {
        return -n;
    } else {
        return n;
    }
}

#endif

DEBUG_INLINE void printf_overflow(void)
{
    debug_critical_t r = debug_enter_critical();
    debug_u16_t l_idx = pbuf.w_P==0 ? PRINTF_BUFFER_NUM-1 : pbuf.w_P-1;
    dbg_memcpy(pbuf.buffer[l_idx], PRINTF_BUFFER_NUM_OVERFLOW_STR, sizeof(PRINTF_BUFFER_NUM_OVERFLOW_STR) - 1);
    pbuf.len[l_idx] = (debug_u16_t)(sizeof(PRINTF_BUFFER_NUM_OVERFLOW_STR) - 1);
    debug_exit_critical(r);
}

DEBUG_INLINE void commit_printf_buf(debug_u16_t idx, debug_size_t len)
{
    debug_critical_t r = debug_enter_critical();
    pbuf.len[idx] = (debug_u16_t)len;
    debug_exit_critical(r);
}

#if !DEBUG_USE_LIBC
#define _ISDIGIT(c)  ((unsigned)((c) - '0') < 10)


#ifdef USING_LONGLONG
DEBUG_INLINE int divide(unsigned long long *n, int base)
#else
DEBUG_INLINE int divide(unsigned long *n, int base)
#endif /* USING_LONGLONG */
{
    int res;

    /* optimized for processor which does not support divide instructions. */
#ifdef USING_LONGLONG
    res = (int)((*n) % (unsigned long long)base);
    *n = (*n) / (unsigned long long)base;
#else
    res = (int)((*n) % (unsigned long)base);
    *n = (*n) / (unsigned long)base;
#endif

    return res;
}

DEBUG_INLINE int skip_atoi(const char **s)
{
    int i = 0;
    while (_ISDIGIT(**s))
        i = i * 10 + *((*s)++) - '0';

    return i;
}

#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

static char *print_number(char *buf,
                          char *end,
#ifdef USING_LONGLONG
                          unsigned long long  num,
#else
                          unsigned long  num,
#endif /* USING_LONGLONG */
                          int   base,
                          int   qualifier,
                          int   s,
                          int   precision,
                          int   type)
{
    char c = 0, sign = 0;
#ifdef USING_LONGLONG
    char tmp[64] = {0};
#else
    char tmp[32] = {0};
#endif /* USING_LONGLONG */
    int precision_bak = precision;
    const char *digits = NULL;
    int i = 0;
    int size = 0;

    size = s;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
    {
        type &= ~ZEROPAD;
    }

    c = (type & ZEROPAD) ? '0' : ' ';

    /* get sign */
    sign = 0;
    if (type & SIGN)
    {
        switch (qualifier)
        {
        case 'h':
            if ((signed short int)num < 0)
            {
                sign = '-';
                num = (unsigned short int)(-(signed short int)num);
            }
            break;
        case 'L':
        case 'l':
            if ((long)num < 0)
            {
                sign = '-';
                num = (unsigned long)(-(long)num);
            }
            break;
        case 0:
        default:
            if ((signed int)num < 0)
            {
                sign = '-';
                num = (unsigned int)(-(signed int)num);
            }
            break;
        }

        if (sign != '-')
        {
            if (type & PLUS)
            {
                sign = '+';
            }
            else if (type & SPACE)
            {
                sign = ' ';
            }
        }
    }

    if (type & SPECIAL)
    {
        if (base == 2 || base == 16)
        {
            size -= 2;
        }
        else if (base == 8)
        {
            size--;
        }
    }

    i = 0;
    if (num == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
            tmp[i++] = digits[divide(&num, base)];
    }

    if (i > precision)
    {
        precision = i;
    }
    size -= precision;

    if (!(type & (ZEROPAD | LEFT)))
    {
        if ((sign) && (size > 0))
        {
            size--;
        }

        while (size-- > 0)
        {
            if (buf < end)
            {
                *buf = ' ';
            }

            ++ buf;
        }
    }

    if (sign)
    {
        if (buf < end)
        {
            *buf = sign;
        }
        -- size;
        ++ buf;
    }

    if (type & SPECIAL)
    {
        if (base == 2)
        {
            if (buf < end)
                *buf = '0';
            ++ buf;
            if (buf < end)
                *buf = 'b';
            ++ buf;
        }
        else if (base == 8)
        {
            if (buf < end)
                *buf = '0';
            ++ buf;
        }
        else if (base == 16)
        {
            if (buf < end)
            {
                *buf = '0';
            }

            ++ buf;
            if (buf < end)
            {
                *buf = type & LARGE ? 'X' : 'x';
            }
            ++ buf;
        }
    }

    /* no align to the left */
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf < end)
            {
                *buf = c;
            }

            ++ buf;
        }
    }

    while (i < precision--)
    {
        if (buf < end)
        {
            *buf = '0';
        }

        ++ buf;
    }

    /* put number in the temporary buffer */
    while (i-- > 0 && (precision_bak != 0))
    {
        if (buf < end)
        {
            *buf = tmp[i];
        }

        ++ buf;
    }

    while (size-- > 0)
    {
        if (buf < end)
        {
            *buf = ' ';
        }

        ++ buf;
    }

    return buf;
}

static char *print_pointer(char *buf,
                           char *end,
                           const void *ptr,
                           int field_width,
                           int precision,
                           int type)
{
    unsigned char bytes[sizeof(void *)];
    const char *digits = (type & LARGE) ? large_digits : small_digits;
    int nr_digits = (int)(sizeof(void *) * 2U);
    int prefix = (type & SPECIAL) ? 2 : 0;
    int size = field_width;

    if (precision < nr_digits) {
        precision = nr_digits;
    }

    size -= precision + prefix;
    if (!(type & LEFT) && !(type & ZEROPAD)) {
        while (size-- > 0) {
            if (buf < end) {
                *buf = ' ';
            }
            ++buf;
        }
    }

    if (type & SPECIAL) {
        if (buf < end) {
            *buf = '0';
        }
        ++buf;
        if (buf < end) {
            *buf = (type & LARGE) ? 'X' : 'x';
        }
        ++buf;
    }

    if (!(type & LEFT)) {
        while (size-- > 0) {
            if (buf < end) {
                *buf = (type & ZEROPAD) ? '0' : ' ';
            }
            ++buf;
        }
    }

    while (precision-- > nr_digits) {
        if (buf < end) {
            *buf = '0';
        }
        ++buf;
    }

    dbg_memcpy(bytes, &ptr, sizeof(ptr));
#if DEBUG_POINTER_LITTLE_ENDIAN
    for (int byte = (int)sizeof(void *) - 1; byte >= 0; byte--) {
#else
    for (int byte = 0; byte < (int)sizeof(void *); byte++) {
#endif
        unsigned char val = bytes[byte];
        if (buf < end) {
            *buf = digits[(val >> 4) & 0x0F];
        }
        ++buf;
        if (buf < end) {
            *buf = digits[val & 0x0F];
        }
        ++buf;
    }

    while (size-- > 0) {
        if (buf < end) {
            *buf = ' ';
        }
        ++buf;
    }

    return buf;
}

static int dbg_vsnprintf(char *buf, debug_size_t size, const char *fmt, va_list args)
{
#ifdef USING_LONGLONG
    unsigned long long num = 0;
#else
    unsigned long num = 0;
#endif /* USING_LONGLONG */
    int i = 0, len = 0;
    char *str = NULL, *end = NULL, c = 0;
    const char *s = NULL;

    unsigned char base = 0;            /* the base of number */
    unsigned char flags = 0;           /* flags to print number */
    char qualifier = 0;       /* 'h', 'l', or 'L' for integer fields */
    signed int field_width = 0;     /* width of output field */
    int precision = 0;      /* min. # of digits for integers and max for a string */
#if DEBUG_ENABLE_FLOAT
    float fv;
    int v;
#endif

    str = buf;
    end = buf + size;

    /* Make sure end is always >= buf */
    if (end < buf)
    {
        end  = ((char *) - 1);
        size = (debug_size_t)(end - buf);
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str < end)
            {
                *str = *fmt;
            }

            ++ str;
            continue;
        }

        /* process flags */
        flags = 0;

        while (1)
        {
            /* skips the first '%' also */
            ++fmt;
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '#') flags |= SPECIAL;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
        }

        /* get field width */
        field_width = -1;
        if (_ISDIGIT(*fmt))
        {
            field_width = skip_atoi(&fmt);
        }
        else if (*fmt == '*')
        {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++fmt;
            if (_ISDIGIT(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if (*fmt == '*')
            {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
            {
                precision = 0;
            }
        }

        qualifier = 0; /* get the conversion qualifier */

        if (*fmt == 'h' || *fmt == 'l' ||
#ifdef USING_LONGLONG
            *fmt == 'L' ||
#endif /* USING_LONGLONG */
            *fmt == 'z')
        {
            qualifier = *fmt;
            ++fmt;
#ifdef USING_LONGLONG
            if (qualifier == 'l' && *fmt == 'l')
            {
                qualifier = 'L';
                ++fmt;
            }
#endif /* USING_LONGLONG */
            if (qualifier == 'h' && *fmt == 'h')
            {
                qualifier = 'H';
                ++fmt;
            }
        }

        /* the default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str < end) *str = ' ';
                    ++ str;
                }
            }

            /* get character */
            c = (char)va_arg(args, int);
            if (str < end)
            {
                *str = c;
            }
            ++ str;

            /* put width */
            while (--field_width > 0)
            {
                if (str < end) *str = ' ';
                ++ str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s)
            {
                s = "(NULL)";
            }

            for (len = 0; (len != field_width) && (s[len] != '\0'); len++);

            if (precision > 0 && len > precision)
            {
                len = precision;
            }

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str < end) *str = ' ';
                    ++ str;
                }
            }

            for (i = 0; i < len; ++i)
            {
                if (str < end) *str = *s;
                ++ str;
                ++ s;
            }

            while (len < field_width--)
            {
                if (str < end) *str = ' ';
                ++ str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = (int)(sizeof(void *) * 2U);
                field_width += 2; /* `0x` prefix */
                flags |= SPECIAL;
                flags |= ZEROPAD;
            }
            str = print_pointer(str, end, va_arg(args, void *),
                                field_width, precision, flags);
            continue;

        case '%':
            if (str < end)
            {
                *str = '%';
            }
            ++ str;
            continue;

        /* integer number formats - set up the flags and "break" */
        case 'b':
            base = 2;
            break;
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
            /* fall through */
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

#if DEBUG_ENABLE_FLOAT
        case 'e':
        case 'E':
        case 'G':
        case 'g':
        case 'f':
        case 'F':
            fv = (float)va_arg(args, double);
            v = (int)fv;
            flags |= SIGN;
            str = print_number(str, end, (unsigned long)v,
                               10, qualifier, field_width, precision, flags);
            if (str < end)
            {
                *str = '.';
            }
            ++ str;
            v = (int)(fv * 1000);
            v = v % 1000;
            v = dbg_abs(v);
            str = print_number(str, end, (unsigned long)v,
                               10, qualifier, field_width, precision, flags);
            continue;
#endif
        default:
            if (str < end)
            {
                *str = '%';
            }
            ++ str;

            if (*fmt)
            {
                if (str < end)
                {
                    *str = *fmt;
                }
                ++ str;
            }
            else
            {
                -- fmt;
            }
            continue;
        }

#ifdef USING_LONGLONG
        if (qualifier == 'L')
        {
            num = va_arg(args, unsigned long long);
        }
        else
#endif
        if (qualifier == 'l')
        {
            num = va_arg(args, unsigned long);
        }
        else if (qualifier == 'H')
        {
            int arg = va_arg(args, int);
            if (flags & SIGN)
            {
#ifdef USING_LONGLONG
                num = (unsigned long long)(signed char)arg;
#else
                num = (unsigned long)(signed char)arg;
#endif
            }
            else
            {
                num = (unsigned long)(unsigned char)arg;
            }
        }
        else if (qualifier == 'h')
        {
            int arg = va_arg(args, int);
            if (flags & SIGN)
            {
#ifdef USING_LONGLONG
                num = (unsigned long long)(signed short int)arg;
#else
                num = (unsigned long)(signed short int)arg;
#endif
            }
            else
            {
                num = (unsigned long)(unsigned short int)arg;
            }
        }
        else if (qualifier == 'z')
        {
            num = (unsigned long)va_arg(args, debug_size_t);
            if (flags & SIGN)
            {
                num = (unsigned long)(debug_int_t)num;
            }
        }
        else if (flags & SIGN)
        {
            num = (unsigned long)va_arg(args, int);
        }
        else
        {
            num = (unsigned long)va_arg(args, unsigned int);
        }
        str = print_number(str, end, num, base, qualifier, field_width, precision, flags);
    }

    if (size > 0)
    {
        if (str < end)
        {
            *str = '\0';
        }
        else
        {
            end[-1] = '\0';
        }
    }

    /* the trailing null byte doesn't count towards the total
    * ++str;
    */
    return (int)(str - buf);
}
#endif

DEBUG_INLINE debug_size_t debug_time_to_str(char *buf, debug_time_t val)
{
    char tmp[20];
    debug_size_t len = 0;
    do {
        tmp[len++] = (char)('0' + (val % 10));
        val /= 10;
    } while(val && len < sizeof(tmp));
    for(debug_size_t i=0; i<len; i++) buf[i] = tmp[len-i-1];
    return len;
}

DEBUG_INLINE debug_size_t debug_size_to_str(char *buf, debug_size_t val)
{
    char tmp[20];
    debug_size_t len = 0;
    do {
        tmp[len++] = (char)('0' + (val % 10));
        val /= 10;
    } while(val && len < sizeof(tmp));
    for(debug_size_t i=0; i<len; i++) buf[i] = tmp[len-i-1];
    return len;
}

DEBUG_INLINE void dbg_append_data(char *buffer, debug_size_t *idx, const char *data, debug_size_t len)
{
    while (len-- > 0 && *idx < (PRINTF_BUFFER_SIZE - 1)) {
        buffer[*idx] = *data++;
        (*idx)++;
    }
}

DEBUG_INLINE void dbg_append_str(char *buffer, debug_size_t *idx, const char *str)
{
    while (*str != '\0' && *idx < (PRINTF_BUFFER_SIZE - 1)) {
        buffer[*idx] = *str++;
        (*idx)++;
    }
}

DEBUG_INLINE void dbg_append_char(char *buffer, debug_size_t *idx, char ch)
{
    if (*idx < (PRINTF_BUFFER_SIZE - 1)) {
        buffer[*idx] = ch;
        (*idx)++;
    }
}

DEBUG_INLINE void dbg_append_time(char *buffer, debug_size_t *idx, debug_time_t val)
{
    char tmp[20];
    debug_size_t len = debug_time_to_str(tmp, val);
    dbg_append_data(buffer, idx, tmp, len);
}

DEBUG_INLINE void dbg_append_size(char *buffer, debug_size_t *idx, debug_size_t val)
{
    char tmp[20];
    debug_size_t len = debug_size_to_str(tmp, val);
    dbg_append_data(buffer, idx, tmp, len);
}

DEBUG_INLINE void dbg_finish_line(char *buffer, debug_size_t *idx, const char *color_end)
{
    if((*idx + _DBG_LOG_X_END_LEN + 1) > PRINTF_BUFFER_SIZE) {
        dbg_strcpy(buffer + (PRINTF_BUFFER_SIZE - 1) - _DBG_LOG_X_END_LEN - (sizeof(PRINTF_BUFFER_SIZE_OVERFLOW_STR) - 1), PRINTF_BUFFER_SIZE_OVERFLOW_STR);
        dbg_strcpy(buffer + (PRINTF_BUFFER_SIZE - 1) - _DBG_LOG_X_END_LEN, color_end);
        *idx = PRINTF_BUFFER_SIZE - 1;
    } else {
        dbg_strcpy(buffer + *idx, color_end);
        *idx += _DBG_LOG_X_END_LEN;
    }
}


void debug_printf(const char *color, const char *color_end, const char * level, const char * file, const char * line, const char *format, ...) {
    debug_size_t i = 0;
    debug_u16_t idx = 0;
    char *buffer = reserve_printf_buf(&idx);
    if(buffer == NULL) {
        printf_overflow();
        return;
    }
    dbg_append_data(buffer, &i, color, _DBG_LOG_COLOR_LEN);
    dbg_append_char(buffer, &i, '(');
    dbg_append_time(buffer, &i, debug_get_time_ms());
    dbg_append_char(buffer, &i, ')');
    dbg_append_char(buffer, &i, '[');
    dbg_append_str(buffer, &i, level);
    dbg_append_char(buffer, &i, ':');
    dbg_append_str(buffer, &i, file);
    dbg_append_char(buffer, &i, ':');
    dbg_append_str(buffer, &i, line);
    dbg_append_char(buffer, &i, ']');
    dbg_append_char(buffer, &i, ':');
    va_list args;
    va_start(args, format);
    debug_size_t w_buff_size = PRINTF_BUFFER_SIZE - i;
    debug_size_t w_size = (debug_size_t)dbg_vsnprintf(buffer + i, w_buff_size, format, args);
    va_end(args);
    if(w_size >= w_buff_size) {
        if (w_buff_size > 0) {
            i += w_buff_size - 1;
        }
    } else {
        i += w_size;
    }
    dbg_finish_line(buffer, &i, color_end);
    commit_printf_buf(idx, i);
}

void debug_printf_hex(const char *color, const char *color_end, const char * level, const char * file, const char * line, const debug_u8_t * hex_buff, debug_size_t buff_len)
{
    debug_size_t i = 0;
    debug_size_t hex_len = 0;
    debug_u16_t idx = 0;
    char *buffer = reserve_printf_buf(&idx);
    if(buffer == NULL) {
        printf_overflow();
        return;
    }
    if (hex_buff != NULL) {
        hex_len = buff_len;
    }
    dbg_append_data(buffer, &i, color, _DBG_LOG_COLOR_LEN);
    dbg_append_char(buffer, &i, '(');
    dbg_append_time(buffer, &i, debug_get_time_ms());
    dbg_append_char(buffer, &i, ')');
    dbg_append_char(buffer, &i, '[');
    dbg_append_str(buffer, &i, level);
    dbg_append_char(buffer, &i, ':');
    dbg_append_str(buffer, &i, file);
    dbg_append_char(buffer, &i, ':');
    dbg_append_str(buffer, &i, line);
    dbg_append_char(buffer, &i, ']');
    dbg_append_char(buffer, &i, ':');
    dbg_append_str(buffer, &i, "HEX(");
    dbg_append_size(buffer, &i, hex_len);
    dbg_append_str(buffer, &i, "):");
    for (debug_size_t k=0; k<hex_len && i < PRINTF_BUFFER_SIZE-3; k++)
    {
        buffer[i++] = large_digits[(hex_buff[k] >> 4) & 0xF];
        buffer[i++] = large_digits[hex_buff[k] & 0xF];
        buffer[i++] = ' ';
    }
    dbg_finish_line(buffer, &i, color_end);
    commit_printf_buf(idx, i);
}

void debug_printf_raw(const char *format, ...) {
    debug_size_t i = 0;
    debug_u16_t idx = 0;
    char *buffer = reserve_printf_buf(&idx);
    if(buffer == NULL) {
        printf_overflow();
        return;
    }
    va_list args;
    va_start(args, format);
    debug_size_t w_size = (debug_size_t)dbg_vsnprintf(buffer, PRINTF_BUFFER_SIZE, format, args);
    va_end(args);
    if(w_size >= PRINTF_BUFFER_SIZE) {
        i += PRINTF_BUFFER_SIZE - 1;
    } else {
        i += w_size;
    }
    commit_printf_buf(idx, i);
}

DEBUG_INLINE char *reserve_printf_buf(debug_u16_t *out_idx)
{
    debug_critical_t r = debug_enter_critical();
    debug_u16_t idx = pbuf.w_P;
    if (pbuf.len[idx] != 0) {
        debug_exit_critical(r);
        return NULL;
    }

    pbuf.w_P = (debug_u16_t)((idx + 1) % PRINTF_BUFFER_NUM);
    debug_exit_critical(r);

    if (out_idx) *out_idx = idx;
    return pbuf.buffer[idx];
}

static void printf_debug_info(void)
{
#if DEBUG_PRINT_VERSION
    if (printf_str == NULL) {
        return;
    }

    //****** DEBUG_VERSION = x.x.x ******
    printf_str(_DBG_COLOR(COLOR_INFO), (debug_printf_len_t)(sizeof(_DBG_COLOR(COLOR_INFO))-1));
    printf_str("****** DEBUG_VERSION = ", (debug_printf_len_t)(sizeof("****** DEBUG_VERSION = ")-1));
    printf_str(DEBUG_VERSION, (debug_printf_len_t)(sizeof(DEBUG_VERSION)-1));
    printf_str(" ******", (debug_printf_len_t)(sizeof(" ******")-1));
    printf_str(_DBG_LOG_X_END, (debug_printf_len_t)(sizeof(_DBG_LOG_X_END)-1));
#endif
}

void debug_init(printf_str_func func_str,enter_critical_func func_enter_critical, exit_critical_func func_exit_critical, get_time_ms_func func_get_time_ms)
{
    dbg_memset(&pbuf,0,sizeof(pbuf));
    printf_str = func_str;
    enter_critical = func_enter_critical;
    exit_critical = func_exit_critical;
    get_time_ms = func_get_time_ms;
    printf_debug_info();
}

void debug_log_flush(void)
{
#if DEBUG_FLUSH_MAX_PER_CALL != 0
    debug_size_t flushed = 0;
#endif

    if(printf_str == NULL)
        return;
    while (1) {
        debug_critical_t r = debug_enter_critical();
        debug_u16_t idx = pbuf.r_P;
        debug_u16_t len = pbuf.len[idx];
        debug_exit_critical(r);

        if (len == 0) {
            break;
        }

        printf_str(pbuf.buffer[idx], (debug_printf_len_t)len);

        r = debug_enter_critical();
        pbuf.len[idx] = 0;
        if (pbuf.r_P == idx) {
            pbuf.r_P = (debug_u16_t)((pbuf.r_P + 1) % PRINTF_BUFFER_NUM);
        }
        debug_exit_critical(r);

#if DEBUG_FLUSH_MAX_PER_CALL != 0
        flushed++;
        if (flushed >= DEBUG_FLUSH_MAX_PER_CALL) {
            break;
        }
#endif
    }
}
