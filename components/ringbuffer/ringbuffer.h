#ifndef RINGBUFFER_H
#define RINGBUFFER_H

typedef struct {
    unsigned char *buffer;
    unsigned int size;
    unsigned int read_index;
    unsigned int write_index;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t *rb, unsigned char *buffer, unsigned int size);
unsigned int ringbuffer_write(ringbuffer_t *rb, const unsigned char *data, unsigned int size);
unsigned int ringbuffer_read(ringbuffer_t *rb, unsigned char *data, unsigned int size);

#endif
