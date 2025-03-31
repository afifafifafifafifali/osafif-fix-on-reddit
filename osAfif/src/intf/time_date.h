#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "port.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

// Function prototypes
static inline uint8_t cmos_read(uint8_t reg);
void read_rtc(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *day, uint8_t *month, uint16_t *year);

// CMOS Read Function
static inline uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

// Read RTC Time & Date
void read_rtc(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *day, uint8_t *month, uint16_t *year) {
    *second = cmos_read(0x00);
    *minute = cmos_read(0x02);
    *hour = cmos_read(0x04);
    *day = cmos_read(0x07);
    *month = cmos_read(0x08);
    *year = cmos_read(0x09) + 2000;  // Assuming RTC gives year in 2-digit format
}

void sleep(int seconds) {
    uint32_t milliseconds = seconds * 1000;  // Convert seconds to milliseconds

    // Using inline assembly to implement the busy-wait for the specified time in ms
    asm volatile(
        "movl %[ms], %%eax;"           // Move the number of milliseconds to wait into eax
        "1:;"                          // Label for loop start
        "movl %%eax, %%ebx;"           // Copy the number of milliseconds to wait into ebx
        "cmpl $0, %%ebx;"              // Compare if the count is 0
        "jle 2f;"                      // Jump to end of the loop if count is <= 0
        // Here we would check if a timer interrupt has occurred (mocked as an example)
        "movl $0x1000, %%ecx;"         // For example, set some condition to simulate a timer interrupt check
        "cmpl $0, %%ecx;"              // Mocking timer register check, comparing with 0
        "je 1b;"                       // If no interrupt, jump to loop start to keep waiting

        "subl $1, %%eax;"              // Decrement the counter (1 ms elapsed)
        "jmp 1b;"                      // Jump back to check the timer again
        
        "2:;"                          // End of loop label
        :                               // No output operands
        : [ms] "r" (milliseconds)      // Input operand for milliseconds
        : "eax", "ebx", "ecx"          // Clobbered registers
    );
}
