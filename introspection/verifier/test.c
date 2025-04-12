#include <unistd.h>
#include <stdio.h>

int main() {
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        printf("This binary is: %s\n", path);
    } else {
        perror("readlink");
    }
    return 0;
}

