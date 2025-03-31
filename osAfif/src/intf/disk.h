#pragma once

#include <stdint.h>

#define SECTOR_SIZE 512 // we assume each sector is exactly 512 bytes long

void getDiskBytes(unsigned char *target, uint32_t LBA, uint8_t sector_count);
void putDiskBytes(const unsigned char *source, uint32_t LBA,
                  uint8_t sector_count);


                  