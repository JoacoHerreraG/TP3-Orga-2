/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "gdt.h"
#include "task.h"

uint32_t next_free_tss_table_index;
uint32_t next_free_gdt_index;
uint32_t user_task_pd[GDT_IDX_TSS_USER_LAST + 1];

tss tss_initial = {
  // Se inicializa en 0 y
  // se completa en tiempo de ejecucion con contexto actual del kernel
  // para en seguida saltar a otra tarea y nunca mas ser activada.
  .ptl = 0,
  .esp0 = 0,
  .ss0 = 0,
  .esp1 = 0,
  .ss1 = 0,
  .esp2 = 0,
  .ss2 = 0,
  .cr3 = 0,
  .eip = 0,
  .eflags = 0,
  .eax = 0,
  .ecx = 0,
  .edx = 0,
  .ebx = 0,
  .esp = 0,
  .ebp = 0,
  .esi = 0,
  .edi = 0,
  .cs = 0,
  .ds = 0,
  .es = 0,
  .fs = 0,
  .gs = 0,
  .ss = 0,
  .ldt = 0,
  .dtrap = 0,
  .iomap = 0,
  .unused0 = 0,
  .unused1 = 0,
  .unused2 = 0,
  .unused3 = 0,
  .unused4 = 0,
  .unused5 = 0,
  .unused6 = 0,
  .unused7 = 0,
  .unused8 = 0,
  .unused9 = 0,
  .unused10 = 0,
};
tss tss_idle = {
  .ptl = 0,

  .esp0 = 0x27000,        // Dado que la tarea idle es de nivel 0,  .esp es la que vale  y .esp0 nunca va a ser accedido.
  .ss0 = GDT_IDX_DS_RING_0 << 3,  // Dado que la tarea idle es de nivel 0,  .ss  es la que vale  y .ss0  nunca va a ser accedido.

  .esp1 = 0,
  .ss1 = 0,

  .esp2 = 0,
  .ss2 = 0,

  .cr3 = 0x27000,   // Mismo directorio de paginas (PD) del kernel.
  .eip = 0x0001A000,  // C'\odigo de la tarea Idle.
  .eflags = 0x202,  // Interrupciones habilitadas.

  .eax = 0,
  .ecx = 0,
  .edx = 0,
  .ebx = 0,
  .esp = 0x27000,
  .ebp = 0x27000,
  .esi = 0,
  .edi = 0,

  .cs = GDT_IDX_CS_RING_0 << 3,  // 0x08 << 3 = 0x40   // Entrada 8 de la gdt es descriptor de segmento de datos de nivel 0.

  .ds = GDT_IDX_DS_RING_0 << 3,  // 0x09 << 3 = 0x48   // Entrada 9 de la gdt es descriptor de segmento de datos de nivel 0.
  .es = GDT_IDX_DS_RING_0 << 3,  // 0x09 << 3 = 0x48   // Entrada 9 de la gdt es descriptor de segmento de datos de nivel 0.
  .fs = GDT_IDX_DS_RING_0 << 3,  // 0x09 << 3 = 0x48   // Entrada 9 de la gdt es descriptor de segmento de datos de nivel 0.
  .gs = GDT_IDX_DS_RING_0 << 3,  // 0x09 << 3 = 0x48   // Entrada 9 de la gdt es descriptor de segmento de datos de nivel 0.
  .ss = GDT_IDX_DS_RING_0 << 3,  // 0x09 << 3 = 0x48   // Entrada 9 de la gdt es descriptor de segmento de datos de nivel 0.

  .ldt   = 0,
  .dtrap = 0,
  .iomap = 0xFFFF,  //

  .unused0 = 0,
  .unused1 = 0,
  .unused2 = 0,
  .unused3 = 0,
  .unused4 = 0,
  .unused5 = 0,
  .unused6 = 0,
  .unused7 = 0,
  .unused8 = 0,
  .unused9 = 0,
  .unused10 = 0,
};

tss tss_table[24];

void tss_init() {
  // En al entrada de la gdt correspondiente a la tss_init
  // se carga la direccci'on base de la tss_init.
  gdt[GDT_IDX_TSS_INIT].base_0_15  = 0x0000FFFF & ((uint32_t) &tss_initial);
  gdt[GDT_IDX_TSS_INIT].base_23_16 = 0x00FF   & ((uint32_t) &tss_initial >> 16);
  gdt[GDT_IDX_TSS_INIT].base_31_24 =        ((uint32_t) &tss_initial >> 24);

  // En al entrada de la gdt correspondiente a la tss_init
  // se carga la direccci'on base de la tss_init.
  gdt[GDT_IDX_TSS_IDLE].base_0_15  = 0x0000FFFF & ((uint32_t) &tss_idle);
  gdt[GDT_IDX_TSS_IDLE].base_23_16 = 0x00FF   & ((uint32_t) &tss_idle >> 16);
  gdt[GDT_IDX_TSS_IDLE].base_31_24 =        ((uint32_t) &tss_idle >> 24);

  // Se inicia el indice para paginas
  next_free_tss_table_index = 0;

  // Se inicia el indice de desctiptores de segmentos de estados de tareas en la gdt
  // Se usan las entradas
  next_free_gdt_index = GDT_IDX_TSS_USER_FIRST;
}

uint32_t get_user_task_pd(uint32_t i){
  return user_task_pd[i];
}

uint32_t tss_nextFreeTssTableIndex(){
  uint32_t free_index     = next_free_tss_table_index;
  next_free_tss_table_index = next_free_tss_table_index + 1;
  return free_index;
}
uint32_t tss_nextFreeGdtIndex(){
  uint32_t free_index = next_free_gdt_index;
  next_free_gdt_index = next_free_gdt_index + 1;
  return free_index;
};

void tss_user_task_init(uint32_t task_base_virtual_address, uint32_t task_base_physical_address, uint32_t src_base_addr) {
  // Se realiza el ejercicio 6 f).

  uint32_t pd_addr;
  // Se llama a mmu_initTaskDir para
  // crear el PD de la tarea con
  // 1 PT para identity mapping del kernel (1024 paginas), y
  // 1 PT para mapear 2 paginas en area del mundo Cronenberg.

  pd_addr = mmu_initTaskDir(task_base_virtual_address, task_base_physical_address);

  // Se quiere copiar desde   src_base_addr
  // 8KB hacia        task_base_virtual_address.
  // Para poder escribir en las direcciones mayores o iguales a 0x08000000,
  // no alcanza con mapear solamente, tambien hay que activar el directorio
  // sobre el registro cr3, sino da excepcion 4.
  uint32_t cr3_prev = rcr3();
  lcr3(pd_addr);

  // Se copian 8KB (dos paginas)
  uint8_t *src; //= (uint8_t *) src_base_addr;
  uint8_t *dst; //= (uint8_t *) task_base_virtual_address;
  src = (uint8_t *) src_base_addr;
  dst = (uint8_t *) task_base_virtual_address;
  for ( int16_t i = 0; i < 0x2000; i++ ) {
    dst[i] = src[i];
  }
  //if (src_base_addr == 0x18000 ){
    // Aca es posible leer el codigo Cronenberg cargado en memoria.
    //breakpoint();
  //}
  // Se restaura cr3 a su valor previo.
  lcr3(cr3_prev);

  uint32_t nextFreeTssTableIndex = tss_nextFreeTssTableIndex();
  // Se genera una entrada de tss
  // sobre siguiente posicion libre del arreglo de tss 'tasks_tss'...
  tss_table[nextFreeTssTableIndex] = (tss) {
    .ptl = 0,

    .esp0 = mmu_nextFreeKernelPage() + 0x1000,
    .ss0 = GDT_IDX_DS_RING_0 << 3,

    .esp1 = 0,
    .ss1 = 0,

    .esp2 = 0,
    .ss2 = 0,

    .cr3 = pd_addr, //task_base_virtual_address & 0xFFC00000,
    .eip = task_base_virtual_address,
      .eflags = 0x202,  // Interrupciones habilitadas.

    .eax = 0,
    .ecx = 0,
    .edx = 0,
    .ebx = 0,
    .esp = task_base_virtual_address + 0x2000,
    .ebp = task_base_virtual_address + 0x2000,
    .esi = 0,
    .edi = 0,

    .cs = (GDT_IDX_CS_RING_3 << 3)|3,

    .ds = (GDT_IDX_DS_RING_3 << 3)|3,
    .es = (GDT_IDX_DS_RING_3 << 3)|3,
    .fs = (GDT_IDX_DS_RING_3 << 3)|3,
    .gs = (GDT_IDX_DS_RING_3 << 3)|3,
    .ss = (GDT_IDX_DS_RING_3 << 3)|3,

    .ldt = 0,
    .dtrap = 0,
    .iomap = 0xFFFF,

    .unused0 = 0,
    .unused1 = 0,
    .unused2 = 0,
    .unused3 = 0,
    .unused4 = 0,
    .unused5 = 0,
    .unused6 = 0,
    .unused7 = 0,
    .unused8 = 0,
    .unused9 = 0,
      .unused10 = 0,
  };

  uint32_t base = (((uint32_t) &tss_table) + (nextFreeTssTableIndex*104) );

  // Se genera un descriptor de segmento de estado de tarea (tss descriptor) en la gdt
  // siguiente posicion libre en la gdt.
  uint32_t nextFreeGdtIndex = tss_nextFreeGdtIndex();
  gdt[nextFreeGdtIndex] = (gdt_entry){
    .base_0_15 = 0x0000FFFF & base,
    .base_23_16 = (0x00FF0000 & base) >> 16,
    .base_31_24 = (0xFF000000 & base) >> 24,
    .limit_0_15 = 0x0067, // 0x0067 = 103
    .limit_16_19 = 0x00,
    .type = 0x9,  // 10B1  con  B = 0 (available)  o  B = 1 (busy)
    .s = 0,
    .dpl = 3,
    .p = 1,
    .db = 0,
    .g = 0,
    .avl = 0,
    .l = 0,
  };
  user_task_pd[nextFreeGdtIndex] = pd_addr;
  return;
}

void tss_user_tasks_init(){
  uint8_t *x;
  uint8_t *y;
  x = (uint8_t *) 0; // Se inicia en 0 para que compile sin warning tratado como error.
  y = (uint8_t *) 1; // Se inicia en 1 para que compile sin warning tratado como error.
  uint32_t task_base_virtual_address = TASK_CODE;
  uint32_t task_base_physical_address;

  uint16_t *video = (uint16_t *) 0xB8000;

  // Parametro 'task_number' indica el numero de bloque de 8KB
  // (2 paginas) a partir de la direccion 0x10000.
  // task_number = 0  genera tarea con   Codigo Rick  C-137   copiandolo desde 0x11000
  // task_number = 1  genera tarea con   Codigo Morty C-137   copiandolo desde 0x13000
  // task_number = 2  genera tarea con   Codigo Rick  D-248   copiandolo desde 0x15000
  // task_number = 3  genera tarea con   Codigo Morty D-248   copiandolo desde 0x17000
  // task_number = 4  genera tarea con   Codigo Cronenberg  copiandolo desde 0x19000
  uint32_t task_type;

  // Se cargan tareas   Rick & Morty C-137.
  for (int32_t i = 0; i < 2; i++) {
    // Se calcula la direccion desde donde copiar el codigo de la tarea.
    task_type = i;
    uint32_t src_base_addr = task_type * 2;
    src_base_addr = src_base_addr * 0x1000;
    src_base_addr = src_base_addr + 0x10000;

    getRandomPosition(i, x, y);
    task_base_physical_address = TASK_CODE_PHYS + (((  ( (uint32_t)SIZE_M) * (uint32_t)(*y) )  +  (uint32_t)(*x)  ) << 13);
    tss_user_task_init(task_base_virtual_address, task_base_physical_address, src_base_addr);
    // Se imprime en video solo por primera vez.   Luego lo realiza al atencion de interrupciones de reloj.
    video[((uint32_t)SIZE_M)  * (uint32_t)(*y) + (uint32_t)(*x) + SIZE_M] = 0x1F52 - 0x05*i;  //   1 = fondo azul    52 = "R"    4D = "M"
  }

  // Se cargan tareas   Rick & Morty D-248.
  for (int32_t i = 2; i < 4; i++) {
    // Se calcula la direccion desde donde copiar el codigo de la tarea.
    task_type = i;
    uint32_t src_base_addr = task_type * 2;
    src_base_addr = src_base_addr * 0x1000;
    src_base_addr = src_base_addr + 0x10000;
    getRandomPosition(i, x, y);
    task_base_physical_address = TASK_CODE_PHYS + (((  ( (uint32_t)SIZE_M) * (uint32_t)(*y) )  +  (uint32_t)(*x)  ) << 13);
    tss_user_task_init(task_base_virtual_address, task_base_physical_address, src_base_addr);
    // Se imprime en video solo por primera vez.   Luego lo realiza al atencion de interrupciones de reloj.
    video[((uint32_t)SIZE_M)  * (uint32_t)(*y) + (uint32_t)(*x) + SIZE_M] = 0x4F5C - 0x05*i;  //   4 = fondo rojo    52 = "R"    4D = "M"
  }

  // Se cargan 20 tareas Cronenberg.
  for (int32_t i = 4; i < 24; i++) {
    // Se calcula la direccion desde donde copiar el codigo de la tarea.
    task_type = 4;
    uint32_t src_base_addr = task_type * 2;
    src_base_addr = src_base_addr * 0x1000;
    src_base_addr = src_base_addr + 0x10000;

    getRandomPosition(i, x, y);
    // i se shiftea 12 bit a izquierda para incrementar paginas
    // y 1 bit mas para multiplicar por 2 (son 2 paginas por tarea)
    task_base_physical_address = (((  ( (uint32_t)SIZE_M) * (uint32_t)(*y) )  +  (uint32_t)(*x)  ) << 13);
    task_base_physical_address = task_base_physical_address + TASK_CODE_PHYS;
    tss_user_task_init(task_base_virtual_address, task_base_physical_address, src_base_addr);
    // Se imprime en video solo por primera vez.   Luego lo realiza al atencion de interrupciones de reloj.
    video[(((uint32_t)SIZE_M) * (uint32_t)(*y)) + (uint32_t)(*x) + SIZE_M] = 0x672B;
  }

  return;
}
