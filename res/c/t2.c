#include <string.h>

int main()
{
    char b[16] = {0};
    for (int i = 0; i < 10; i++) {
        strcat(b, "oops");
    }

    return 0;
}
