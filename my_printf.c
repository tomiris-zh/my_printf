#include<stdio.h> 
#include<unistd.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>

#define DEFAULT_INTEGER_BASE 10
#define BUFFER_INTEGER_SIZE 21
#define HEXADECIMAL_BASE 16
#define OCTAL_BASE 8
static const char BASE_SYMBOLS[] = "0123456789abcdef";

int flush_string(char const * format, int length) {
    return write(1, format, length);
}

int my_strlen(char const * string) {
    int length = 0;
    while (string[length] != 0) {
        ++length;
    }
    return length;
}

int long get_number_length(int long number, int base) {
    int length = 0;
    if (number == 0)
        return 1;
    while(number != 0) {
        length++;
        number /= base;
    }
    return length;
}

int long my_abs(int long number) {
    if (number < 0) return -number;
    return number;
}

int my_int_to_string(char * buffer, int number, int base) {
    int buffer_size = 0;
    int number_length = get_number_length(number, base);
    int index = number_length - 1;
    while(index >= 0) {
        buffer[index] = BASE_SYMBOLS[my_abs(number % base)];
        number /= base;
        --index;
    }
    return number_length;
}

unsigned int get_positive_part(int number, int base) {
    if(base == 10)
        my_abs(number);
    return number;
}

int handle_integer(int number, int base) {
    char buffer[BUFFER_INTEGER_SIZE];
    char *buffer_pointer = buffer;
    int buffer_size = 0;
    memset(buffer, 0, BUFFER_INTEGER_SIZE);
    buffer[0] = '-';
    unsigned int positive_part = get_positive_part(number, base);
    buffer_size = my_int_to_string(buffer + 1, number, base);
    if (base == 10 && number < 0) 
        ++buffer_size;
    else 
        ++buffer_pointer;
    flush_string(buffer_pointer, buffer_size);
    return buffer_size;
}

int my_unsigned_int_to_string(char * buffer, int number, int base) {
    int buffer_size = 0;
    int long num = number;
    if (number < 0) {
        num = number + 4294967296;
    }
    int long number_length = get_number_length(num, base);
    int index = number_length - 1;
    while(index >= 0) {
        buffer[index] = BASE_SYMBOLS[abs(num % base)];
        num /= base;
        --index;
    }
    return number_length;
}

unsigned int handle_unsigned_integer(int number, int base) {
    char buffer[BUFFER_INTEGER_SIZE];
    int buffer_size = 0;
    memset(buffer, 0, BUFFER_INTEGER_SIZE);
    buffer_size = my_unsigned_int_to_string(buffer, number, base);
    flush_string(buffer, buffer_size);
    return buffer_size;
}

unsigned int handle_unsigned_octal(int number, int base) {
    char buffer[BUFFER_INTEGER_SIZE];
    int buffer_size = 0;
    memset(buffer, 0, BUFFER_INTEGER_SIZE);
    buffer_size = my_int_to_string(buffer, number, base);
    flush_string(buffer, buffer_size);
    return buffer_size;
}

unsigned long my_hex_to_string(char * buffer, unsigned long number, int base) {
    int buffer_size = 0;
    int number_length = get_number_length(number, base);
    int index = number_length - 1;
    while(index >= 0) {
        buffer[index] = BASE_SYMBOLS[number % base];
        number /= base;
        --index;
    }
    return number_length;
}

unsigned long handle_unsigned_hex(unsigned long number, int base) {
    char buffer[BUFFER_INTEGER_SIZE];
    int buffer_size = 0;
    memset(buffer, 0, BUFFER_INTEGER_SIZE);
    buffer_size = my_hex_to_string(buffer, number, base);
    flush_string(buffer, buffer_size);
    return buffer_size;
}

int handle_format(char const * format, int *runner, va_list *ap) {
    int result_length = 0;
    int base = DEFAULT_INTEGER_BASE;
    ++(*runner);

    if (format[*runner] == 0)
        return 0;
    switch(format[*runner]) {
         case 'o': {
            base = OCTAL_BASE;
            int argument = va_arg(*ap, int);
            result_length += handle_unsigned_octal(argument, base);
            break;
        }
        case 'c': {
            char argument = va_arg(*ap, int);
            result_length += write(1, &argument, 1);
            break;
        }
        case 'p': {
            unsigned long argument = va_arg(*ap, unsigned long);
            write(1, "0x", 2);
            base = HEXADECIMAL_BASE;
            result_length += 2 + handle_unsigned_hex(argument, base);
            break;
        }
        case 'd': {
            int argument = va_arg(*ap, int);
            result_length += handle_integer(argument, base);
            break;
        }
        case 'x': {
            base = HEXADECIMAL_BASE;
            int argument = va_arg(*ap, int);
            result_length += handle_unsigned_hex(argument, base);
            break;
        }
        case 'u' : {
            int argument = va_arg(*ap, int);
                result_length += handle_unsigned_integer(argument, base);
            break;
        }
        case 's': {
            const char * argument = va_arg(*ap, char *);
            if (argument == 0)
		        argument = "(null)";
            result_length += write(1, argument, my_strlen(argument));
            break;
        }
        default:
            break;
    }
    return result_length;
}

int my_printf(char * restrict format, ...) {
    va_list ap;
    int result_length = 0;
    int runner = 0;
    int anchor = 0;
    va_start(ap, format);
    while (format[runner] != '\0') {
        if (format[runner] == '%') {
            result_length += flush_string(format + anchor, runner - anchor);
            result_length += handle_format(format, &runner, &ap);
            anchor = runner + 1;
        }
        ++runner;
    }
    result_length += flush_string(format + anchor, runner - anchor);
    va_end(ap);
    return result_length;
}
