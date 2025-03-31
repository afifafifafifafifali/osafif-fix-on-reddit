#pragma once
#include <stdint.h>
#include <stddef.h>

// This is a simple interface for the port I/O operations.
// The implementation is in src/impl/kernel/port.c.

// Outport 
void outb(uint16_t port, uint8_t value);

// Inport
uint8_t inb(uint16_t port);

// Outport word
void outw(uint16_t port, uint16_t value);
// Inport word
uint16_t inw(uint16_t port);
// Outport long
void outl(uint16_t port, uint32_t value);
// Inport long
uint32_t inl(uint16_t port);
