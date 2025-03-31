#include "print.h"
#include "str.h"
const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* buffer = (struct Char*) 0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        character: ' ',
        color: color,
    };

    for (size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void print_clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

void print_newline() {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (size_t row = 1; row < NUM_ROWS; row++) {
        for (size_t col = 0; col < NUM_COLS; col++) {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_COLS - 1);
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }

    if (character == '\b') {
        if (col > 0) {
            col--;  // Move cursor back
            buffer[col + NUM_COLS * row] = (struct Char) { ' ', color };  // Erase character
        }
        return;
    }

    if (col >= NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) { (uint8_t) character, color };
    col++;
}

void print_str(char* str) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) str[i];

        if (character == '\0') {
            return;
        }

        print_char(character);
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground + (background << 4);
}


char* ftoa(float value, char* buffer, int precision) {
    int int_part = (int)value;
    float fraction = value - (float)int_part;

    itoa(int_part, buffer, 10);

    int i = strlen(buffer);
    buffer[i++] = '.';  // Add decimal point

    for (int j = 0; j < precision; j++) {
        fraction *= 10;
        int digit = (int)fraction;
        buffer[i++] = '0' + digit;
        fraction -= digit;
    }

    buffer[i] = '\0';
    return buffer;
}

#include <stdarg.h>

void print_int(int num) {
    char buffer[16];
    itoa(num, buffer, 10);
    print_str(buffer);
}
void print_format(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    char* str = va_arg(args, char*);
                    print_str(str);
                    break;
                }
                case 'd': {
                    char buffer[16];
                    itoa(va_arg(args, int), buffer, 10);
                    print_str(buffer);
                    break;
                }
                case 'x': {
                    char buffer[16];
                    itoa(va_arg(args, int), buffer, 16);
                    print_str(buffer);
                    break;
                }
                case 'c': {
                    print_char((char)va_arg(args, int));
                    break;
                }
                case 'f': {  // Floating-point support
                    char buffer[32];
                    ftoa(va_arg(args, double), buffer, 6);  // Default precision: 6
                    print_str(buffer);
                    break;
                }
                case '%': {
                    print_char('%');
                    break;
                }
                default: {
                    print_char('%');
                    print_char(fmt[i]);
                    break;
                }
            }
        } else {
            print_char(fmt[i]);
        }
    }

    va_end(args);
}

