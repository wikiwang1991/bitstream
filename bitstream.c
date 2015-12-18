#include "bitstream.h"

uint32_t bitstream_read_most32(bitstream_t *t, uint8_t bits)
{
    uint32_t u = 0;
    while (bits > 8) {
        bits -= 8;
        u |= bitstream_read_most8(t, 8) << bits;
    }
    u |= bitstream_read_most8(t, bits);
    return u;
}

size_t bitstream_read_exp_golomb_ue_v(bitstream_t *t)
{
    size_t u;
    uint8_t b;
    int8_t l = -1;
    for (b = 0; !b; ++l)
        b = bitstream_read_1(t);
    if (l) {
        u = bitstream_read_most8(t, l);
        u = (1 << l) - 1 + u;
        return u;
    } else return 0;
}

void bitstream_copy(bitstream_t *t, bitstream_t *src, size_t bits)
{
    uint8_t b;
    // @TODO: optimize this.
    while (bits--) {
        b = bitstream_read_1(src);
        if (b) bitstream_write1(t);
        else bitstream_write0(t);
    }
}
