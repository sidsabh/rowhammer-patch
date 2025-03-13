#include <tcclib.h>

void vulnerable_function() {
    printf("Hello, RISC-V!\n");
}

int main() {
    vulnerable_function();
    return 0;
}

