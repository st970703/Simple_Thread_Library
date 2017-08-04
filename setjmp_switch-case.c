// CS340_UOA - setjmp_switch_case.c
//
// Demonstrate use of setjmp/longjump as well as switch/case flow control
//
// Cam Smith, 2017-08-01
// Copyright and related rights waived via CC0


#include <stdio.h>
#include <setjmp.h>

void main() {
    int result;
    jmp_buf buffer;

    result = setjmp(buffer);

    switch (result) {
        case 0:
            printf("First time around\n");
            longjmp(buffer, 1);
            break;

        case 1:
            printf("Second time around\n");
            longjmp(buffer, 9001);
            break;

        case 9001:
            printf("Last time around\n");
            longjmp(buffer, -1);
            
            break;
    }
}
