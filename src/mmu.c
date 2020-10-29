/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
  (o de la "memory managing unit" o "MMU")
*/

#include "mmu.h"
#include "defines.h"

// Variable global puntero a proxima pagina libres.
uint32_t proxima_pagina_libre;

void mmu_init() {
  proxima_pagina_libre = 0x100000;
}

uint32_t mmu_nextFreeKernelPage() {
  uint32_t pagina_libre = proxima_pagina_libre;
  proxima_pagina_libre = proxima_pagina_libre + 0x1000;
  return pagina_libre;
}

void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint32_t attrs) {
  // Se asume que cr3 apunta a un directorio de páginas (PD) ya inicializado en memoria.
  uint32_t pdAddr;
  uint32_t pdeIndex;
  pde_t *pd;
  pde_t pde;

  uint32_t ptAddr;
  uint32_t pteIndex;
  pte_t *pt;
  pte_t pte;

  uint32_t phyPageAddr;

  pdAddr = 0xFFFFF000 & cr3;   // Ya es fisica, gracias al mapeo lineal.
  pd = (pde_t *) pdAddr;

  pdeIndex = 0xFFC00000 & virtual;

  pdeIndex = pdeIndex >> 22;
  pde = pd[pdeIndex];
  if ( pde.present == 1 ) {
    ptAddr = ((uint32_t)pde.base << 12); //0xFFFFF000 & ((uint32_t)pde.base << 12);
    pt = (pte_t *) ptAddr;

  } else {

    pt = (pte_t *) mmu_nextFreeKernelPage();
    for (int i = 0; i < 1024; i++){
      pt[i] = (pte_t){0};
    }
    ptAddr = (uint32_t)pt;
    pde.base = ptAddr >> 12;
    pde.readwrite = (0x00000002 & attrs) >> 1;
    pde.present = 1;
    pde.usersupervisor = (0x00000004 & attrs) >> 2;
    pd[pdeIndex] = pde;
  }
  pteIndex = 0x003FF000 & virtual;
  pteIndex = pteIndex >> 12;
  phyPageAddr = 0xFFFFF000 & phy;
  pte.base = phyPageAddr >> 12;
  pte.readwrite = (0x00000002 & attrs) >> 1;
  pte.present = 1;
  pte.usersupervisor = (0x00000004 & attrs) >> 2;
  pt[pteIndex] = pte;
  tlbflush();
  return;
}

uint32_t mmu_unmapPage(uint32_t cr3, uint32_t virtual) {
  uint32_t pdAddr;
  uint32_t pdeIndex;
  pde_t *pd;
  pde_t pde;
  uint32_t ptAddr;
  uint32_t pteIndex;
  pte_t *pt;
  pdAddr = 0xFFFFF000 & cr3;
  pd = (pde_t *) pdAddr;
  pdeIndex = 0xFFC00000 & virtual;
  pdeIndex = pdeIndex >> 22;
  pde = pd[pdeIndex];
  if ( pde.present == 1 ) {
    ptAddr = ((uint32_t)pde.base << 12);
    pt = (pte_t *) ptAddr;
    pteIndex = 0x003FF000 & virtual;
    pteIndex = pteIndex >> 12;
    pt[pteIndex] = (pte_t){0};
    pt[pteIndex].present = 0;  // No hace falta... la instruccion anterior ya deja todo en 0.
  }
  tlbflush();
  return 0;
}

uint32_t mmu_initKernelDir() {
  pde_t *pd = (pde_t *) KERNEL_PAGE_DIR;       //0x27000;
  pte_t *pt_0 = (pte_t *) KERNEL_PAGE_TABLE_0; //0x28000;
  for (int i = 0; i < 1024; i++){
    pd[i] = (pde_t){0};
    pt_0[i] = (pte_t){0};
  }

  pd[0].base = KERNEL_PAGE_TABLE_0 >> 12;   //0x28;
  pd[0].usersupervisor = 0;
  pd[0].readwrite = 1;
  pd[0].present = 1;
  for (int i = 0; i < 1024; i++){
    pt_0[i].base = i;
    pt_0[i].usersupervisor = 0;
    pt_0[i].readwrite = 1;
    pt_0[i].present = 1;
  }
  return (uint32_t) pd;
}


void mmu_mapTaskCodeAndStack(uint32_t pd_address, uint32_t task_base_virtual_address, uint32_t task_base_physical_address) {
  mmu_mapPage(pd_address, task_base_virtual_address, task_base_physical_address, 6);                     //   r/w = 2   u/s = 4    2+4 = 6
  mmu_mapPage(pd_address, task_base_virtual_address + 0x1000, task_base_physical_address + 0x1000, 6);   //   r/w = 2   u/s = 4    2+4 = 6
  return;
}

void mmu_unmapTaskCodeAndStack(uint32_t pd_address, uint32_t task_base_virtual_address) {
  mmu_unmapPage(pd_address, task_base_virtual_address);
  mmu_unmapPage(pd_address, task_base_virtual_address + 0x1000);
  return;
}


uint32_t mmu_initTaskDir(uint32_t task_base_virtual_address, uint32_t task_base_physical_address) {
  pde_t *pd = (pde_t *) mmu_nextFreeKernelPage();
  pte_t *pt = (pte_t *) mmu_nextFreeKernelPage();
  // Se limpian los bloques de 4K con ceros.
  for (int i = 0; i < 1024; i++){
    pd[i] = (pde_t){0};
    pt[i] = (pte_t){0};
  }
  pd[0].base = (uint32_t) pt >> 12;
  pd[0].usersupervisor = 0;
  pd[0].readwrite = 1;
  pd[0].present = 1;
  for (int i = 0; i < 1024; i++){
    pt[i].base = i;
    pt[i].usersupervisor = 0;
    pt[i].readwrite = 1;
    pt[i].present = 1;
  }
  mmu_mapTaskCodeAndStack((uint32_t) pd, task_base_virtual_address, task_base_physical_address);
  return (uint32_t) pd;
}



void mmu_RelocateTaskPhyscally(uint32_t task_pd_addr, uint32_t virtual, uint32_t phy_old, uint32_t phy_new){

  mmu_mapTaskCodeAndStack(task_pd_addr, TASK_CODE + 0x2000, phy_old);
  tlbflush();
  mmu_unmapTaskCodeAndStack(task_pd_addr, TASK_CODE);
  tlbflush();
  mmu_mapTaskCodeAndStack(task_pd_addr, TASK_CODE, phy_new);
  tlbflush();
  uint32_t cr3_prev = rcr3();
  lcr3(task_pd_addr);
  uint8_t *src;
  uint8_t *dst;
  src = (uint8_t *) TASK_CODE + 0x2000; //direccion fisica   VIEJA.
  dst = (uint8_t *) TASK_CODE;      //direccion fisica   NUEVA.
  for ( int16_t i = 0; i < 0x2000; i++ ) {
    dst[i] = src[i];
  }
  lcr3(cr3_prev);
  /*mmu_unmapTaskCodeAndStack(task_pd_addr, TASK_CODE + 0x2000);
  lcr3(task_pd_addr);*/
  return;
}
