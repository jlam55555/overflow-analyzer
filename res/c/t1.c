#include <string.h>

int f(char b1[], char b2[], size_t len) {
    int cpy = strncpy(b1, b2, len);
    return cpy;
}


int main()
{
    char b1[128];
    char b2[256]; 

    f(b1, b2, sizeof(b2));

    return 0;
}
