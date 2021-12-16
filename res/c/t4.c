#include <stdio.h>
#include <stdlib.h>

char *get_input()
{
    size_t len = 16;
    char *b = calloc(len, sizeof(char));

    char byte;
    for (int i = 0; (byte = getchar()) != '\n'; i++) {
        if (i == len) {
            len += 16;
            b = realloc(b, len);
        }
        b[i] = byte;
    }

    return b;
}

int main()
{
    char *b = get_input();
    printf("%s\n", b);

    free(b);
    return 0;
}
