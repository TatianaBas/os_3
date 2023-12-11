#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

auto main(int argc, char** argv) -> int {
    FILE* f = fopen("output.txt", "w");
    fprintf(f, "The sums are: ");

    int memoryd;
    memoryd = open("memory.txt", O_RDWR, 0666);
    char* buffer = (char*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, memoryd, 0);
    close(memoryd);

    sem_t* sem = sem_open(argv[1], O_CREAT, 0777, 0);  // Open semaphore from argument list

    if (sem == SEM_FAILED) {
        perror("Could not open semaphore");
        return -1;
    }

    int num = 0, sum = 0;
    size_t i = 0;

    sem_wait(sem);  // Wait for semaphore to unlock and then lock it

    while (buffer[i] != -1) {
        if (buffer[i] != ' ' && buffer[i] != '\n') {
            num *= 10;
            num += buffer[i] - '0';
        } else if (buffer[i] == ' ') {
            sum += num;
            num = 0;
        } else if (buffer[i] == '\n') {
            if (num != 0)
                sum += num;
            fprintf(f, "%d ", sum);
            sum = 0;
            num = 0;
        }
        ++i;
    }

    sem_close(sem);

    munmap(buffer, 1024);

    return 0;
}
