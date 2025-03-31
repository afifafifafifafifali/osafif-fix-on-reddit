#include "disk.h"
#include "str.h"
#include "port.h"
#include "ata.h"
#include "mallocate.h"
#include "print.h"
void getDiskBytes(unsigned char *target, uint32_t LBA, uint8_t sector_count) {
  uint32_t *read = (uint32_t *)malloc(sector_count * SECTOR_SIZE);
  if (!read) {
      printf("Memory allocation failed!\n");
      return;
  }

  read_sectors_ATA_PIO(read, LBA, sector_count);

  int writingCurr = 0;
  for (int i = 0; i < ((sector_count * SECTOR_SIZE) / 4); i++) {
      target[writingCurr] = read[i] & 0xFF;
      target[writingCurr + 1] = (read[i] >> 8) & 0xFF;
      target[writingCurr + 2] = (read[i] >> 16) & 0xFF;
      target[writingCurr + 3] = (read[i] >> 24) & 0xFF;
      writingCurr += 4;
  }

  free(read); // Free memory after use
}

  void putDiskBytes(const unsigned char *source, uint32_t LBA, uint8_t sector_count) {
    uint32_t write_buffer[SECTOR_SIZE / 4]; // Buffer for 32-bit writes

    int writingCurr = 0;
    for (int i = 0; i < sector_count; i++) {
        memset(write_buffer, 0, SECTOR_SIZE); // Ensure buffer is clean before writing

        for (int j = 0; j < SECTOR_SIZE / 4; j++) {
            write_buffer[j] = ((uint32_t)source[writingCurr]) |
                            (((uint32_t)(writingCurr + 1 < SECTOR_SIZE ? source[writingCurr + 1] : 0)) << 8) |
                            (((uint32_t)(writingCurr + 2 < SECTOR_SIZE ? source[writingCurr + 2] : 0)) << 16) |
                            (((uint32_t)(writingCurr + 3 < SECTOR_SIZE ? source[writingCurr + 3] : 0)) << 24);
            writingCurr += 4;
        }

        // Write the 32-bit buffer to the disk
        write_sectors_ATA_PIO(LBA + i, 1, write_buffer);
    }
}
