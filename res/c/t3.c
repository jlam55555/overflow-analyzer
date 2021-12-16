#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("usage: %s [input]\n", argv[0]);
    }

    char b1[32] = {0};
    char b2[16] = {0};
    size_t i;
    for (i = 0; argv[i]; i++) {
        b1[i] = argv[i];
    }

    if (i < sizeof(b1)) {
        printf("b: %s\n", b1);
    } else {
        strcpy(b1, b2);
    }

    return 0;
}
