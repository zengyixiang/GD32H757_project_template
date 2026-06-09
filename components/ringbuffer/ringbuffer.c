#include "ringbuffer.h"

void ringbuffer_init(ringbuffer_t *rb, unsigned char *buffer, unsigned int size)
{
    if(rb == 0) {
        return;
    }

    rb->buffer = buffer;
    rb->size = size;
    rb->read_index = 0U;
    rb->write_index = 0U;
}

unsigned int ringbuffer_write(ringbuffer_t *rb, const unsigned char *data, unsigned int size)
{
    (void)rb;
    (void)data;

    return size;
}

unsigned int ringbuffer_read(ringbuffer_t *rb, unsigned char *data, unsigned int size)
{
    (void)rb;
    (void)data;

    return size;
}
