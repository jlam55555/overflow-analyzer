#include <string.h>

void f(char *dst, char *src) {
        strcpy(dst, src);
}

int main() {
        char b1[512], b2[4096];
        f(b1, b2);
}
