#include "decoder.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    char line[203];
    int recovered = 0;
    int rejected = 0;

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t len = strlen(line);

        while (len > 0 &&
             (line[len - 1] == '\n' ||
                 line[len - 1] == '\r')) {
            len--;
            line[len] = '\0';
        }

        if (process_transmission(line)) {
            recovered++;
        } else {
            rejected++;
        }
    }

    printf("Recovered: %d | Rejected: %d\n", recovered, rejected);
    return 0;
}
