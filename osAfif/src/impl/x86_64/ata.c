#include "ata.h"
#include <stdint.h>
#include "port.h"

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01


static void ATA_wait_BSY();
static void ATA_wait_DRQ();
static void insl(uint16_t port, void *addr, uint32_t cnt);
static void outsl(uint16_t port, const void *addr, uint32_t cnt);

// INSL function
static void insl(uint16_t port, void *addr, uint32_t cnt) {
    asm volatile("cld; rep insl" : "=D"(addr), "=c"(cnt) : "d"(port), "0"(addr), "1"(cnt) : "memory", "cc");
}
static void outsl(uint16_t port, const void *addr, uint32_t cnt) {
    asm volatile("cld; rep outsl" : "=S"(addr), "=c"(cnt) : "d"(port), "0"(addr), "1"(cnt) : "memory", "cc");
}

void read_sectors_ATA_PIO(uint32_t target_address, uint32_t LBA, uint8_t sector_count) {
    ATA_wait_BSY();
    outb(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
    outb(0x1F5, (uint8_t)(LBA >> 16));
    outb(0x1F7, 0x20);

    for (int i = 0; i < sector_count; i++) {
        ATA_wait_BSY();
        ATA_wait_DRQ();
        insl(0x1F0, target_address, 128);
        target_address += 512;
    }
}

void write_sectors_ATA_PIO(uint32_t LBA, uint8_t sector_count, uint32_t *bytes) {
    ATA_wait_BSY();
    outb(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
    outb(0x1F2, sector_count);
    outb(0x1F3, (uint8_t)LBA);
    outb(0x1F4, (uint8_t)(LBA >> 8));
    outb(0x1F5, (uint8_t)(LBA >> 16));
    outb(0x1F7, 0x30);

    for (int i = 0; i < sector_count; i++) {
        ATA_wait_BSY();
        ATA_wait_DRQ();
        outsl(0x1F0, bytes, 128);
        bytes += 128;
    }
}

static void ATA_wait_BSY() {
    while (inb(0x1F7) & STATUS_BSY);
}

static void ATA_wait_DRQ() {
    while (!(inb(0x1F7) & STATUS_DRQ));
}