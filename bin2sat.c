#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "string.h"

#define GETB(var, offset) (var & (1ull << offset))
#define INDEXB(bit) (bit >> 6ull)

int main(int argc, char **argv)
{
    char buffer[8];
    ssize_t result;
    memset(buffer, 0, 8*sizeof(char));
    if (argc == 2 &&
        (strncmp(argv[1], "--help", 6) || strncmp(argv[1], "-h", 2)))
    {
        fprintf(stderr, "Usage: %s < bin > sat\n\n", argv[0]);
        return 0;
    }

    uint64_t max_var = 0;
    uint64_t pos = 0;
    uint64_t var = 0;
    uint64_t offset = 0;

    result = read(STDIN_FILENO, buffer, 4);
    if (result != 4) {
        return 1;
    }
    max_var = buffer[0];
    max_var = max_var << 8ull;
    max_var |= buffer[1];
    max_var = max_var << 8ull;
    max_var |= (uint8_t)buffer[2];
    max_var = max_var << 8ull;
    max_var |= (uint8_t)buffer[3];
    if (max_var == 0) {
        printf("s UNSATISFIABLE\n");
        return 20;
    }

    while (result > 0) {
        printf("s SATISFIABLE\n");
        for (pos = 0; pos <= INDEXB(max_var); pos++) {
            result = read(STDIN_FILENO, buffer, 8);
            if (result != 8) {
                return 2;
            }
            var = (uint8_t)buffer[0];
            var = var << 8ull;
            var |= (uint8_t)buffer[1];
            var = var << 8ull;
            var |= (uint8_t)buffer[2];
            var = var << 8ull;
            var |= (uint8_t)buffer[3];
            var = var << 8ull;
            var |= (uint8_t)buffer[4];
            var = var << 8ull;
            var |= (uint8_t)buffer[5];
            var = var << 8ull;
            var |= (uint8_t)buffer[6];
            var = var << 8ull;
            var |= (uint8_t)buffer[7];

            for (offset = 0; offset < 64; offset++) {
                if ((pos*64+offset) > max_var || (pos == 0 && offset == 0)) {
                    continue;
                }

                if (GETB(var, offset)) {
                    printf(" %lu", pos*64+offset);
                } else {
                    printf(" -%lu", pos*64+offset);
                }
            }
            printf("\n");
        }
        printf("v 0\n");

        result = read(STDIN_FILENO, buffer, 4);
        if (result != 4) {
            return 10;
        }
        max_var = buffer[0];
        max_var = max_var << 8;
        max_var |= buffer[1];
        max_var = max_var << 8;
        max_var |= buffer[2];
        max_var = max_var << 8;
        max_var |= buffer[3];
        if (max_var == 0) {
            printf("s UNSATISFIABLE\n");
            return 10;
        }
    }
    return 10;
}
