#include "write_f.h"

int main()
{
    int num1 = 15;
    char name[10] = "russel";
    writef("Hello my name is {s} and I am {i} years old", (void*[]){ &name, &num1 });
    return 0;
}
