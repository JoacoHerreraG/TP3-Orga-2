/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
    #define __MMU_H__

    #include "stdint.h"
    #include "defines.h"
    #include "i386.h"
    #include "tss.h"
    #include "game.h"

    /* Estructura (struct) entrada en directorio de paginas (PDE) */
    typedef struct str_pde {
        uint8_t  present:1;
        uint8_t  readwrite:1;
        uint8_t  usersupervisor:1;
        uint8_t  pwt:1;
        uint8_t  pcd:1;
        uint8_t  accessed:1;
        uint8_t  ignored:1;
        uint8_t  pagesize:1;
        uint8_t  global:1;
        uint8_t   available:3;
        uint32_t   base:20;
    } __attribute__((__packed__)) pde_t;

    /* Estructura (struct) entrada en tabla de paginas (PTE) */
    typedef struct str_pte {
        uint8_t  present:1;
        uint8_t  readwrite:1;
        uint8_t  usersupervisor:1;
        uint8_t  pwt:1;
        uint8_t  pcd:1;
        uint8_t  accessed:1;
        uint8_t  dirtybit:1;
        uint8_t  pagetableattibuteindex:1;
        uint8_t  global:1;
        uint8_t   available:3;
        uint32_t   base:20;
    } __attribute__((__packed__)) pte_t;


    void mmu_init();

    uint32_t mmu_nextFreeKernelPage();

    void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint32_t attrs);

    uint32_t mmu_unmapPage(uint32_t cr3, uint32_t virtual);

    uint32_t mmu_initKernelDir();

    void mmu_mapTaskCodeAndStack(uint32_t cr3, uint32_t task_initial_virtual_address, uint32_t task_initial_physical_address);
    void mmu_unmapTaskCodeAndStack(uint32_t cr3, uint32_t task_initial_virtual_address);
    uint32_t mmu_initTaskDir(uint32_t task_initial_virtual_address, uint32_t task_initial_physical_address);
    void mmu_RelocateTaskPhyscally(uint32_t task_pd_addr, uint32_t virtual, uint32_t phy_old, uint32_t phy_new);

#endif	/* !__MMU_H__ */
