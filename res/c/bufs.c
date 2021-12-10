#include <string.h>

int main(int argc, char **argv) {
        char n1[8192] = {0};
        char n2[1024] = {1,2,3};

        char *buf = NULL;
        
        /* int a = 10; */
        /* double d[3]; */

        /* if (a == 3) { */
        /*         char f[512]; */

        /*         f[0] = 52; */
        /* } */

        /* n1[4]; */

        strcpy(n1, n2);

        buf = n2;
}
