#pragma once

#include <stdint.h>


#define IS_LITTLE_ENDIAN 0// ((*(uint8_t*)&(const uint16_t){1}) == 1)

/**
 * \brief Convert the given 2-byte sequence from little endian to the host byte order
 */
uint16_t le_to_host16(uint16_t bytes);

/**
 * \brief Convert the given 4-byte sequence from little endian to the host byte order
 */
uint32_t le_to_host32(uint32_t bytes);

/**
 * \brief Conver the given 8-byte sequence from little endian to the host byte order
 */
uint64_t le_to_host64(uint64_t bytes);
