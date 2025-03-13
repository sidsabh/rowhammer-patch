#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>


#define PAGE_SIZE 4096  // Memory page size

void flip_bit_and_execute(const char *filename) {
    srand(time(NULL));

    // Open the binary file
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open binary");
        exit(1);
    }

    // Get file size
    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // Map the binary file into memory
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(1);
    }
    close(fd);

    // Flip a random bit in the mapped memory
    int byte_offset = rand() % size;
    int bit_offset = rand() % 8;
    ((char *)mem)[byte_offset] ^= (1 << bit_offset);

    printf("Flipping bit %d in byte %d\n", bit_offset, byte_offset);

    // Write modified binary to a temporary file
    char temp_filename[] = "/tmp/poc_modified_XXXXXX";
    int temp_fd = mkstemp(temp_filename);
    if (temp_fd < 0) {
        perror("Failed to create temp file");
        munmap(mem, size);
        exit(1);
    }
    write(temp_fd, mem, size);
    close(temp_fd);
    munmap(mem, size);

    // Set executable permission
    chmod(temp_filename, 0755);

    // Fork and execute the modified binary
    pid_t pid = fork();
    if (pid == 0) {
        execl(temp_filename, temp_filename, NULL);
        perror("exec failed"); // Should never reach here
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status)) {
            printf("Process crashed with signal %d\n", WTERMSIG(status));
        } else {
            printf("Process exited normally\n");
        }
    }

    // Cleanup
    unlink(temp_filename);
}

int main() {
    printf("Running testbench on poc_rv\n");
    flip_bit_and_execute("./poc_rv");
    return 0;
}

