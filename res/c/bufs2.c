#include <string.h>

int main() {
        char b1[8192];
        char *b2;
        char b3[512];

        b2 = b1;

        strcpy(b2, b3);
}
