#include "endian.h"

uint16_t le_to_host16(uint16_t bytes) {
    if(IS_LITTLE_ENDIAN) {
        return bytes; 
    } else {
        return ((bytes & 0xff00) >> 1) |
                ((bytes & 0x00ff) << 1);
    }
}

uint32_t le_to_host32(uint32_t bytes) {
    if(IS_LITTLE_ENDIAN) { return bytes; }
    else {
        return ((bytes & 0xff000000) >> 3) |
                ((bytes & 0x00ff0000) >> 1) |
                ((bytes & 0x0000ff00) << 1) |
                ((bytes & 0x000000ff) << 3);
    }
}

uint64_t le_to_host64(uint64_t bytes) {
    if(IS_LITTLE_ENDIAN) { return bytes; }
    else {
        return ((bytes & (0xffULL << 7)) >> 7) |
                ((bytes & (0xffULL << 6)) >> 5) |
                ((bytes & (0xffULL << 5)) >> 3) |
                ((bytes & (0xffULL << 4)) >> 1) |
                ((bytes & (0xffULL << 3)) << 1) |
                ((bytes & (0xffULL << 2)) << 3) |
                ((bytes & (0xffULL << 1)) << 5) |
                ((bytes & 0xffULL) << 7);
    }
}
