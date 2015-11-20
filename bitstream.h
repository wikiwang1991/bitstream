#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef inline
#define inline __inline
#endif

typedef struct bitstream_t {
    uint8_t *data;
    uint8_t pos;
} bitstream_t;

static inline void bitstream_open(bitstream_t *t, void *data)
{
    t->data = data;
    t->pos = 0;
}

static inline void bitstream_seek_forward(bitstream_t *t, size_t bits)
{
    bits += t->pos;
    t->data += bits >> 3;
    t->pos = bits & 7;
}

static inline void bitstream_seek_back(bitstream_t *t, size_t bits)
{
    bits -= t->pos;
    uint8_t o = bits & 7;
    if (o) {
        t->data -= (bits >> 3) + 1;
        t->pos = 8 - o;
    } else {
        t->data -= bits >> 3;
        t->pos = 0;
    }
}

static inline uint8_t bitstream_peek_1(bitstream_t *t)
{
    uint8_t u = *t->data << t->pos;
    return u >> 7;
}

static inline uint8_t bitstream_read_1(bitstream_t *t)
{
    uint8_t u = bitstream_peek_1(t);
    bitstream_seek_forward(t, 1);
    return u;
}

static inline uint8_t bitstream_peek_most8(bitstream_t *t, uint8_t bits)
{
    uint8_t u = 0;
    uint8_t *p = t->data;
    uint8_t pos = t->pos;
    if (bits + pos > 8) {
        u |= *p++ << pos;
        u >>= 8 - bits;
        bits -= 8 - pos;
        pos = 0;
    }
    uint8_t b = *p << pos;
    return u | b >> 8 - bits;
}

static inline uint8_t bitstream_read_most8(bitstream_t *t, uint8_t bits)
{
    uint8_t u = bitstream_peek_most8(t, bits);
    bitstream_seek_forward(t, bits);
    return u;
}

uint32_t bitstream_read_most32(bitstream_t *t, uint8_t bits);

size_t bitstream_read_exp_golomb_ue_v(bitstream_t *t);

static inline void bitstream_set1(bitstream_t *t)
{
    *t->data |= 1 << (7 - t->pos);
}

static inline void bitstream_write1(bitstream_t *t)
{
    bitstream_set1(t);
    bitstream_seek_forward(t, 1);
}

static inline void bitstream_clear1(bitstream_t *t)
{
    *t->data &= ~(1 << (7 - t->pos));
}

static inline void bitstream_write0(bitstream_t *t)
{
    bitstream_clear1(t);
    bitstream_seek_forward(t, 1);
}

void bitstream_copy(bitstream_t *t, bitstream_t *src, size_t bits);

static inline void bitstream_write(bitstream_t *t, void *data, size_t bits)
{
    bitstream_t w;
    bitstream_open(&w, data);
    bitstream_copy(t, &w, bits);
}
