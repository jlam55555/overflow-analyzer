#include <string.h>

int f(char *d, char *s) {
        strcpy(d, s);
}

int main() {
        char b1[512], b2[512], b3[1024], b4[1024];
        f(b1, b2);
        f(b1, b4);
}
