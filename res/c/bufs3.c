#include <string.h>
#include <stdlib.h>

int main() {
        char src1[8192], src2[512], *src, dst[512];

        if (rand() % 2) {
                src = src1;
        } else {
                src = src2;
        }

        strcpy(dst, src);
}



/* BB1: */


/* char b1[8], *b2; */
/* b2 = b1; */

/* %b1 = alloca [8 x i8]*;         [i8]* */
/* %b2 = alloca i8**;              i8** */
/* %0 = getelementptr b1, 0, 0;    i8* */
/* store %0, %b2;                  void */

/* BB2 */
/* I4      {&b1}     {&b1}     {b1} */

/* BB3 */
/* I4      {&b1}     {&b2}     {b2} */

/* BB4 */
/* I4      {&b1}     {&b1,&b2}     {b1,b2} */


/*         b1        b2        0 */
/* beg     {}        {}        {} */
/* I1      {&b1}     {}        {} */
/* I2      {&b1}     {&b2}     {} */
/* I3      {&b1}     {&b2}     {b1} */
/* I4      {&b1}     {&b1}     {b1} */

/* [i8]* %bi = alloca [arr]*;                 bi = {&bi}; */
/* i8** %bi - alloca i8**;                   bi = {&bipp}; */
/* i8 %bi = getelementptr i* %bj, 0, 0;       bi = map * sigma(bj) */
/* i8 %bi = load i8* %bj; */
/* store i8 %bj, i8* %bi; /\* from, to *\/       bi = map & sigma(bj) */

/* call strcpy; */

/*     BB1 */
/*    /   \ */
/*  BB2   BB3 */
/*     \ / */
/*     BB4 */


