#include <string.h>

void g(char *s) {
        char buf[512];
        strcpy(buf, s);
}

void f(char *s) {
        g(s); 
}

int main() {
        char str[8192];
        f(str);
}
