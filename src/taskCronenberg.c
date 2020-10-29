/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h"
#include "i386.h"

void task() {
    while(1) {
        __asm __volatile("nop" : : ); // "no operation" = "nop"
    };
}
