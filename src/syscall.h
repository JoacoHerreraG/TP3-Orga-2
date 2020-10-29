/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/
#include "i386.h"

#ifndef __SYSCALL_H__
    #define __SYSCALL_H__

    #include "stdint.h"

    #define LS_INLINE static __inline __attribute__((always_inline))

    /*
     * Syscalls
     */

    LS_INLINE uint32_t syscall_usePortalGun(int32_t x, int32_t y, uint32_t cross, uint32_t withMorty) {
        int32_t ret;
      __asm__ volatile (
        "int $137"         /* make the request to the OS */
        : "=a" (ret)       /* return value in eax ("a") */
        : "a"  (x),        /* pass in ebx ("a") arg "x"*/
          "b"  (y),        /* pass in ebx ("b") arg "y"*/
          "c"  (cross),    /* pass in ecx ("c") arg "c"*/
          "d"  (withMorty) /* pass in edx ("d") arg "func"*/
        : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
        return ret;
    }

    LS_INLINE void syscall_IamRick(uint32_t code) {
      __asm__ volatile (
        "int $138"         /* make the request to the OS */
        :
        : "a"  (code)    /* pass in eax ("a") */
        : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
    }

    LS_INLINE void syscall_whereIsMorty(int32_t* x, int32_t* y) {
        int xx,yy;
      __asm__ volatile (
        "int $139"        /* make the request to the OS */
        : "=a" (xx),      /* return value in eax ("x") */
          "=b" (yy)       /* return value in ebx ("y") */
        :
        : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
        *x = xx;
        *y = yy;
    }

#endif  /* !__SYSCALL_H__ */
