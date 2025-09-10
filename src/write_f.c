#include "write_f.h"

//   {i} - int
//   {d} - double
//   {f} - float
//   {c} - char
//   {s} - char*
//   {&} - void*
// so like if main is
// "hello {s}, your birthday is in {d} minutes and you are currently {i} years old"

// instead of write from unistead I can do this 
long sys_write(int fd, const void *buf, unsigned long count) {
    long ret;
    asm volatile (
        "mov $1, %%rax\n"     // syscall number for write
        "mov %1, %%rdi\n"     // fd
        "mov %2, %%rsi\n"     // buf
        "mov %3, %%rdx\n"     // count
        "syscall\n"
        "mov %%rax, %0\n"     // return value
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(count)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
    return ret;
}


int _length (char* string) {
    int len = 0;
    while (string[len] != '\0') {
        len++;
    }
    return len;
}

// fn to reverse a string
void __reverse_string(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int __abs_int(int num) {
    if (num < 0) {
        return -num; // if neg return the positive equ
    } else {
        return num;  // if pos or zero, return as is
    }
}


// who knew that int would be one of the hardest things to inplement
void _int(void* adr) {
    int* received = (int*)adr;
    int value = *received;
    
    // max 10 digits
    char buffer[12]; 
    
    int i = 0;
    int is_negative = 0;

    // Handle 0 separately
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        sys_write(1, buffer, 1);
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = __abs_int(value);
    }

    // Extract digits in reverse order
    while (value != 0) {
        int rem = value % 10;
        buffer[i++] = rem + '0';
        value = value / 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    // add null terminator and reverse the string
    buffer[i] = '\0';
    __reverse_string(buffer, i);

    // then write it
    sys_write(1, buffer, i);
}

// helper for float and double
void __print_fraction(double frac, int precision) {
    for (int i = 0; i < precision; i++) {
        frac *= 10;
    }

    int int_part = (int)(frac + 0.5);  // rounding
    _int(&int_part);
}

// prints a float
void _float(void* adr) {
    float val = *((float*) adr);
    int int_part = (int) val;
    double frac = val - int_part;

    _int(&int_part);
    sys_write(1, ".", 1);
    __print_fraction(frac, 2);
}

// prints a double
void _double(void* adr) {
    double val = *((double*) adr);
    int int_part = (int) val;
    double frac = val - int_part;

    _int(&int_part);
    sys_write(1, ".", 1);
    __print_fraction(frac, 2);
}

//  print a individual char
void _char(void* adr) {
    char c = *((char*) adr);
    sys_write(1, &c, 1);
}

// print a char array or a string literal 
void _charP(void* adr) {
    char* str = (char*) adr;
    int len = _length(str);
    sys_write(1, str, len);
}

// helper for the print void pointer 
void __print_hex(unsigned long value) {
    char hex[17];
    int i = 0;

    if (value == 0) {
        sys_write(1, "0", 1);
        return;
    }

    while (value > 0) {
        int digit = value % 16;
        if (digit < 10) {
            hex[i++] = '0' + digit;
        } else {
            hex[i++] = 'a' + (digit - 10);
        }
        value /= 16;
    }

    hex[i] = '\0';
    __reverse_string(hex, i);
    sys_write(1, hex, i);
}

// print a void pointer {&}
void _voidP(void* adr) {
    sys_write(1, "0x", 2);
    __print_hex((unsigned long) adr);
}


void writef(char *main, void **vars) {
    void (*types[])(void*) = {
        _int,     // {i}
        _double,  // {d}
        _float,   // {f}
        _char,    // {c}
        _charP,   // {s}
        _voidP    // {&}
    };

    int var_index = 0;
    for (int i = 0; i < _length(main); i++) {
        if (main[i] == '{' && main[i+2] == '}') {
            char spec = main[i+1];
            switch (spec) {
                case 'i': types[0](vars[var_index++]); break;
                case 'd': types[1](vars[var_index++]); break;
                case 'f': types[2](vars[var_index++]); break;
                case 'c': types[3](vars[var_index++]); break;
                case 's': types[4](vars[var_index++]); break;
                case '&': types[5](vars[var_index++]); break;
                default:
                    sys_write(1, &main[i], 1); // Write '{'
                    continue;
            }
            i += 2; // skip over the full "{x}"
        } else {
            sys_write(1, &main[i], 1);
        }
    }
}
