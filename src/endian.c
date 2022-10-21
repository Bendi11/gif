#include "endian.h"

static inline uint16_t swizzle16(uint16_t bytes) {
    return ((bytes & 0xff00) >> 8) |
            ((bytes & 0x00ff) << 8);
}

static inline uint32_t swizzle32(uint32_t bytes) {
    return ((bytes & 0xff000000) >> 24) |
            ((bytes & 0x00ff0000) >> 8) |
            ((bytes & 0x0000ff00) << 8) |
            ((bytes & 0x000000ff) << 24);
}

static inline uint64_t swizzle64(uint64_t bytes) {
    return ((bytes & (0xffULL << 56)) >> 56) |
            ((bytes & (0xffULL << 48)) >> 40) |
            ((bytes & (0xffULL << 40)) >> 24) |
            ((bytes & (0xffULL << 32)) >> 8) |
            ((bytes & (0xffULL << 24)) << 8) |
            ((bytes & (0xffULL << 16)) << 24) |
            ((bytes & (0xffULL << 8)) << 40) |
            ((bytes & 0xffULL) << 56);
}

uint16_t le_to_host16(uint16_t bytes) {
    if(IS_LITTLE_ENDIAN) { return bytes; }
    else { return swizzle16(bytes); } 
}

uint32_t le_to_host32(uint32_t bytes) {
    if(IS_LITTLE_ENDIAN) { return bytes; }
    else { return swizzle32(bytes); }
}

uint64_t le_to_host64(uint64_t bytes) {
    if(IS_LITTLE_ENDIAN) { return bytes; }
    else { return swizzle64(bytes); }
}
