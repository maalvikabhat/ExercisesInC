/* Example code for Exercises in C.

Copyright 2014 Allen Downey
License: Creative Commons Attribution-ShareAlike 3.0

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void free_anything(int *p) {
    free(p);
}

int read_element(int *array, int index) {
    int x = array[index];
    return x;
}

int main()
{
    int never_allocated;
    int *free_twice = malloc(sizeof (int));
    int *use_after_free = malloc(sizeof (int));
    int *never_free = malloc(sizeof (int));
    // int array1[100];
    int *array2 = malloc(100 * sizeof (int));

    // valgrind does not bounds-check static arrays
    // read_element(array1, -1);
    // read_element(array1, 100);

    // // but it does bounds-check dynamic arrays
    // read_element(array2, -1);
    // read_element(array2, 99);

    // // and it catches use after free
    free(use_after_free);
    // *use_after_free = 17;

    // // never_free is definitely lost
    *never_free = 17;
    free(never_free);

    // // the following line would generate a warning
    // free(&never_allocated);

    // // but this one doesn't
    // free_anything(&never_allocated);

    free(free_twice);
    // free(free_twice);

    free(array2);
    return 0;
}

/*
And here's the final valgrind output:
==10461== Memcheck, a memory error detector
==10461== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==10461== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==10461== Command: ./mem
==10461== 
==10461== 
==10461== HEAP SUMMARY:
==10461==     in use at exit: 0 bytes in 0 blocks
==10461==   total heap usage: 4 allocs, 4 frees, 412 bytes allocated
==10461== 
==10461== All heap blocks were freed -- no leaks are possible
==10461== 
==10461== For counts of detected and suppressed errors, rerun with: -v
==10461== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
 */
