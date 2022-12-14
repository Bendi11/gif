#pragma once

#include <stddef.h>
#include <stdint.h>


#define IS_LITTLE_ENDIAN ((*(uint8_t*)&(const uint16_t){1}) == 1)

uint16_t le_to_host16(uint16_t bytes);

uint32_t le_to_host32(uint32_t bytes);

uint64_t le_to_host64(uint64_t bytes);

/** \brief Swizzle the bytes in `bytes` to the host's endianness */
void le_to_host(void *const bytes, size_t len);
