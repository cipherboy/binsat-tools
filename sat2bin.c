#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "assert.h"
#include "ctype.h"

#define MAXVARS 4194304
#define SATSIZE (MAXVARS>>6)
#define LINESIZE 4096

#define GETB(src, offset) (src & (1ull << offset))
#define SETB(arr, bit) arr[bit >> 6ull] |= (1ull << (bit & 63ull))
#define INDEXB(bit) (bit >> 6ull)

int main(int argc, char **argv)
{
    uint64_t *sat = calloc(SATSIZE, sizeof(uint64_t));
    assert(sat != NULL);
    char *line = calloc(LINESIZE+2, sizeof(char));
    if (argc == 2 &&
        (strncmp(argv[1], "--help", 6) || strncmp(argv[1], "-h", 2)))
    {
        fprintf(stderr, "Usage: %s < sat > bin\n\n", argv[0]);
        free(sat);
        free(line);
        return 0;
    }

    int have_values = 0;
    uint32_t max_var = 0;
    uint64_t value = 1;
    uint64_t var = 0;
    size_t pos = 0;
    while (fgets(line, LINESIZE, stdin))
    {
        if (line[0] != 's' && line[0] != 'v')
        {
            if (have_values) {
                printf("%c%c%c%c",
                    (uint8_t)((max_var >> 24) & 0xFF),
                    (uint8_t)((max_var >> 16) & 0xFF),
                    (uint8_t)((max_var >> 8) & 0xFF),
                    (uint8_t)(max_var & 0xFF));
                for (pos = 0; pos <= INDEXB(max_var); pos++) {
                    printf("%c%c%c%c%c%c%c%c",
                        (uint8_t)((sat[pos] >> 56) & 0xFF),
                        (uint8_t)((sat[pos] >> 48) & 0xFF),
                        (uint8_t)((sat[pos] >> 40) & 0xFF),
                        (uint8_t)((sat[pos] >> 32) & 0xFF),
                        (uint8_t)((sat[pos] >> 24) & 0xFF),
                        (uint8_t)((sat[pos] >> 16) & 0xFF),
                        (uint8_t)((sat[pos] >> 8) & 0xFF),
                        (uint8_t)(sat[pos] & 0xFF));
                }
                have_values = 0;
                memset(sat, 0, sizeof(uint64_t)*SATSIZE);
            }
            continue;
        } else if (line[0] == 's') {
            if (line[1] == '\0' || line[2] == '\0') {
                /* Error processing: too short of line */
                return 1;
            } else if (line[2] == 'U' || line[2] == 'u') {
                /* UNSAT */
                printf("%c%c%c%c", 0, 0, 0, 0);
                if (max_var == 0) {
                    free(sat);
                    free(line);
                    return 10;
                }
                free(sat);
                free(line);
                return 20;
            }
            /* SAT */
        } else if (line[0] == 'v') {
            have_values = 1;
            var = 0;
            value = 1;
            pos = 2;
            if (line[1] == '\0') {
                /* Error processing: second character of line is empty */
                free(sat);
                free(line);
                return 3;
            }
            while (line[pos] != '\0') {
                if (line[pos] == '-') {
                    value = 0;
                } else if (isdigit(line[pos])) {
                    var = (var*10) + (line[pos] - '0');
                } else if (var != 0) {
                    if (value) {
                        SETB(sat, var);
                    }
                    if (var > max_var) {
                        max_var = var;
                    }
                    value = 1;
                    var = 0;
                } else {
                    var = 0;
                    value = 1;
                }
                pos += 1;
            }
        }
    }
    if (have_values) {
        printf("%c%c%c%c",
            (uint8_t)((max_var >> 24) & 0xFF),
            (uint8_t)((max_var >> 16) & 0xFF),
            (uint8_t)((max_var >> 8) & 0xFF),
            (uint8_t)(max_var & 0xFF));
        for (pos = 0; pos <= INDEXB(max_var); pos++) {
            printf("%c%c%c%c%c%c%c%c",
                (uint8_t)((sat[pos] >> 56) & 0xFF),
                (uint8_t)((sat[pos] >> 48) & 0xFF),
                (uint8_t)((sat[pos] >> 40) & 0xFF),
                (uint8_t)((sat[pos] >> 32) & 0xFF),
                (uint8_t)((sat[pos] >> 24) & 0xFF),
                (uint8_t)((sat[pos] >> 16) & 0xFF),
                (uint8_t)((sat[pos] >> 8) & 0xFF),
                (uint8_t)(sat[pos] & 0xFF));
        }
        have_values = 0;
    }
    free(sat);
    free(line);
    return 10;
}
