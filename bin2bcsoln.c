#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "stdint.h"
#include "unistd.h"
#include "string.h"
#include "strings.h"
#include "ctype.h"
#include "errno.h"

#define GETB(var, offset) (var & (1ull << offset))
#define INDEXB(bit) (bit >> 6ull)

#define MAXVARS 4194304
#define SATSIZE (MAXVARS>>6)
#define LINESIZE 4096

int main(int argc, char **argv)
{
    char buffer[8];
    ssize_t result;
    memset(buffer, 0, 8*sizeof(char));
    if (argc != 2 ||
        (strncmp(argv[1], "--help", 6) == 0 || strncmp(argv[1], "-h", 2) == 0))
    {
        fprintf(stderr, "Usage: %s /path/to/problem.cnf < bin > bc-soln.json\n\n", argv[0]);
        return 0;
    }

    uint64_t cnf_max = 0;
    size_t map_size = SATSIZE;
    char ***map = calloc(map_size, sizeof(char**));
    uint32_t *lengths = calloc(map_size, sizeof(uint32_t));
    char *line = calloc(LINESIZE+2, sizeof(char));
    char *bcvar = NULL;
    size_t cnfvar = 0;
    assert(map != NULL);
    assert(lengths != NULL);
    assert(line != NULL);

    FILE *cnf = fopen(argv[1], "r");
    if (cnf == NULL) {
        return 1;
    }

    while (fgets(line, LINESIZE, cnf))
    {
        if (line[0] == 'c') {
            if (line[1] != ' ') {
                continue;
            }
            char *lindex = index(line, '<');
            char *rindex = index(line, '>');
            if (lindex == NULL || rindex == NULL) {
                continue;
            }
            if (lindex+2 != rindex) {
                continue;
            }
            lindex -= 1;
            rindex += 1;
            if (*lindex != ' ' || *rindex != ' ') {
                continue;
            }

            bcvar = calloc(lindex - (line), sizeof(char));
            assert(bcvar != NULL);
            strncpy(bcvar, line+2, lindex - (line+2));

            rindex += 1;
            cnfvar = 0;
            while (isdigit(*rindex)) {
                cnfvar = cnfvar*10 + (*rindex - '0');
                rindex += 1;
            }

            if (cnfvar > cnf_max) {
                cnf_max = cnfvar;
            }

            if (cnfvar > map_size) {
                map = realloc(map, cnfvar + 1000);
                lengths = realloc(map, cnfvar + 1000);
                for (; map_size < cnfvar + 1000; map_size++) {
                    map[map_size] = NULL;
                    lengths[map_size] = 0;
                }
            }

            map[cnfvar] = realloc(map[cnfvar], lengths[cnfvar]+1);
            map[cnfvar][lengths[cnfvar]] = bcvar;
            lengths[cnfvar] += 1;
        } else if (line[0] == 'p') {
            break;
        }
    }

    uint64_t max_var = 0;
    uint64_t pos = 0;
    uint64_t var = 0;
    uint64_t offset = 0;

    uint64_t map_pos = 0;
    uint64_t current_var = 0;

    result = read(STDIN_FILENO, buffer, 4);
    if (result != 4) {
        return 2;
    }
    max_var = (uint8_t)buffer[0];
    max_var = max_var << 8ull;
    max_var |= (uint8_t)buffer[1];
    max_var = max_var << 8ull;
    max_var |= (uint8_t)buffer[2];
    max_var = max_var << 8ull;
    max_var |= (uint8_t)buffer[3];
    if (max_var == 0) {
        printf("[{}]\n");
        return 20;
    }

    printf("[");
    int b_first = 1;

    while (result > 0) {
        if (!b_first) {
            printf(",");
        }
        b_first = 0;

        int first = 1;
        printf("{");
        for (pos = 0; pos <= INDEXB(max_var); pos++) {
            result = read(STDIN_FILENO, buffer, 8);
            if (result <= 0) {
                return 3;
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
                current_var = pos*64+offset;
                if (current_var > max_var || (pos == 0 && offset == 0) ||
                    current_var > cnf_max) {
                    continue;
                }

                for (map_pos = 0; map_pos < lengths[current_var]; map_pos++) {
                    if (!first) {
                        printf(",");
                    }
                    first = 0;

                    if (GETB(var, offset)) {
                        printf("\"%s\":\"T\"", map[current_var][map_pos]);
                    } else {
                        printf("\"%s\":\"F\"", map[current_var][map_pos]);
                    }
                }
            }
        }
        printf("}");

        result = read(STDIN_FILENO, buffer, 4);
        if (result != 4) {
            printf("]");
            return 10;
        }
        max_var = (uint8_t)buffer[0];
        max_var = max_var << 8;
        max_var |= (uint8_t)buffer[1];
        max_var = max_var << 8;
        max_var |= (uint8_t)buffer[2];
        max_var = max_var << 8;
        max_var |= (uint8_t)buffer[3];
        if (max_var == 0) {
            printf(",{}]");
            return 10;
        }
    }
    printf("]");
    return 10;
}
