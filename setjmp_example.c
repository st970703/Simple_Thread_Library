// CS340_UOA - setjmp_example.c
//
// Demonstrate use of setjmp/longjump
//
// Cam Smith, 2017-08-01
// Copyright and related rights waived via CC0


#include<stdio.h>
#include<setjmp.h>

void main() {
    jmp_buf buffer_a;
    int result;

    result = setjmp(buffer_a);
    
    if (result == 0) {
        printf("Hello World!\n");
        longjmp(buffer_a, 9001);
    }

    if (result == 9001) {
        printf("Goodbye World!\n");
    }
}
