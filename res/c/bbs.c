#include <stdio.h>

int func(int n) 
{
    return n + n;
}

int func2(int n) 
{
    return n*n;
}

int main(void) 
{
    int n = 2;
    printf("%d\n", n);
    if (n == 2) {
        n = func(n);
    } else {
        n = func2(n);
    }

    if (n == 2) {
        printf("%d\n", n);
    }

    return 0;
}