/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales (o "global descriptor table", o "GDT")
*/

#include "gdt.h"

/* Las siguientes constantes se definen en 'define.h':
    #define GDT_COUNT 35
    #define GDT_IDX_NULL_DESC     0
    #define GDT_IDX_CS_RING_0			8
    #define GDT_IDX_DS_RING_0			9
    #define GDT_IDX_CS_RING_3			10
    #define GDT_IDX_DS_RING_3			11
    #define GDT_IDX_VM_RING_0			12
    #define GDT_IDX_TSS_INIT			13
    #define GDT_IDX_TSS_IDLE			14
*/

//#define GDT_IDX_TEST_DESC 4

gdt_entry gdt[GDT_COUNT] = {
  /* Descriptor nulo*/
  /* Offset = 0x00 */
  [GDT_IDX_NULL_DESC] = (gdt_entry) {
    (uint16_t)    0x0000,         /* limit[0:15]  */
    (uint16_t)    0x0000,         /* base[0:15]   */
    (uint8_t)     0x00,           /* base[23:16]  */
    (uint8_t)     0x00,           /* type         */
    (uint8_t)     0x00,           /* s            */
    (uint8_t)     0x00,           /* dpl          */
    (uint8_t)     0x00,           /* p            */
    (uint8_t)     0x00,           /* limit[16:19] */
    (uint8_t)     0x00,           /* avl          */
    (uint8_t)     0x00,           /* l            */
    (uint8_t)     0x00,           /* db           */
    (uint8_t)     0x00,           /* g            */
    (uint8_t)     0x00,           /* base[31:24]  */
  },

  // Se crean 4 segmentos de memoria, de misma extensi'on.
  // Tienen primer byte en la direcci'on 0,
  // y se extiende

  // Segmento de codigo nivel 0
  [GDT_IDX_CS_RING_0] = (gdt_entry){
    .base_0_15 = 0x0000,  // Parte baja  de diredcci'on de comienzo secci'on.
    .base_23_16 = 0x00, // Parte media de diredcci'on de comienzo secci'on.
    .base_31_24 = 0x00, // Parte alta  de diredcci'on de comienzo secci'on.
    .limit_0_15 = 0x88FF, // Parte baja de largo (en 'granos') 0x8900 - 1.
    .limit_16_19 = 0x0,     // Parte alta de largo (en 'granos') de secci'on.
    .type = 0xA,  // 10: Secci'on de C'ODIGO Execute/Read (manual 3 p. 100).
    .s = 1,       // 1: Codigo/Datos
    .dpl = 0,     // nivel de privilegio
    .p = 1,       // 1: esta presente
    .db = 1,      // 1: Operaciones por defecto tienen largo de 32 bit.
    .g = 1,       // 0: 'granos' de 1 byte. 1: 'granos' de 4 KB
    .avl = 0,     // 0 o 1 si es disponible para sistema
    .l = 0,       // 0 si est'a en 32 bit, 1 si esta en 64 bit
  },
  // Segmento de datos nivel 0
  [GDT_IDX_DS_RING_0] = (gdt_entry){
    .base_0_15 = 0x0000,
    .base_23_16 = 0x00,
    .base_31_24 = 0x00,
    .limit_0_15 = 0x88FF,
    .limit_16_19 = 0x0,
    .type = 0x2,    // 2: Secci'on de DATOS Read/Write (manual 3, p. 100)
    .s = 1,         // 1: Codigo/Datos
    .dpl = 0,       // nivel de privilegio
    .p = 1,         // 1: esta presente
    .db = 1,        // 1: si es de 32bits
    .g = 1,         // 0: bloques de 1 byte. 1: bloques de 4 kbytes
    .avl = 0,       // 0 o 1 si es disponible para sistema
    .l = 0,         // 0 o 1 si esta en 64bits
  },
  // Segmento de codigo nivel 3
  [GDT_IDX_CS_RING_3] = (gdt_entry){
    .base_0_15 = 0,
    .base_23_16 = 0,
    .base_31_24 = 0,
    .limit_0_15 = 0x88FF,
    .limit_16_19 = 0,
    .type = 0xA,    // 10: codigo Execute/Read (pag 100 del manual 3)
    .s = 1,         // 1: Codigo/Datos
    .dpl = 3,       // nivel de privilegio
    .p = 1,         // 1: esta presente
    .db = 1,        // 1: si es de 32bits
    .g = 1,         // 0: bloques de 1 byte. 1: bloques de 4 kbytes
    .avl = 0,       // 0 o 1 si es disponible para sistema
    .l = 0,         // 0 o 1 si esta en 64bits
  },
  // Segmento de datos nivel 3
  [GDT_IDX_DS_RING_3] = (gdt_entry){
    .base_0_15 = 0,
    .base_23_16 = 0,
    .base_31_24 = 0,
    .limit_0_15 = 0x88FF,
    .limit_16_19 = 0,
    .type = 0x2,    // 2: dato Read/Write (pag 100 del manual 3)
    .s = 1,         // 1: Codigo/Datos
    .dpl = 3,       // nivel de privilegio
    .p = 1,         // 1: esta presente
    .db = 1,        // 1: si es de 32bits
    .g = 1,         // 0: bloques de 1 byte. 1: bloques de 4 kbytes
    .avl = 0,       // 0 o 1 si es disponible para sistema
    .l = 0,         // 0 o 1 si esta en 64bits
  },

  // Segmento de datos de video nivel 0
  [GDT_IDX_DS_VIDEO_RING_0] = (gdt_entry){
    .base_0_15 = 0x8000,
    .base_23_16 = 0x0B,
    .base_31_24 = 0x00,
    .limit_0_15 = 0x1F3F,   // 80 x 50 x 2 -1 = 7999
    .limit_16_19 = 0,
    .type = 0x2,
    .s = 1,
    .dpl = 0,
    .p = 1,
    .db = 1,
    .g = 1,
    .avl = 0,
    .l = 0,
  },

  // Descriptor de TSS de la tarea inicial
  [GDT_IDX_TSS_INIT] = (gdt_entry){
    .base_0_15 = 0x0000, // Se determina en tss.c en tiempo de ejecuci'\on o compilacion.
    .base_23_16 = 0x00,
    .base_31_24 = 0x00,
    .limit_0_15 = 0x0067, // 0x0067 = 103
    .limit_16_19 = 0x00,
    .type = 0x9,   // 10B1    con B = 0 o B = 1    // comenzanos 'busy'
    .s = 0,
    .dpl = 0,
    .p = 1,
    .db = 0,
    .g = 0,
    .avl = 0,
    .l = 0,
  },

  // Descriptor de TSS de la tarea idle
  [GDT_IDX_TSS_IDLE] = (gdt_entry){
    .base_0_15 = 0x0000, // Se determina en tss.c en tiempo de ejecucion o compilacion.
    .base_23_16 = 0x00,
    .base_31_24 = 0x00,
    .limit_0_15 = 0x0067, // 0x0067 = 103
    .limit_16_19 = 0x00,
    .type = 0x9,  // 10B1  con B = 0 o B = 1
    .s = 0,
    .dpl = 0,
    .p = 1,
    .db = 0,
    .g = 0,
    .avl = 0,
    .l = 0,
  }

};

gdt_descriptor GDT_DESC = {
  sizeof(gdt) - 1,
  (uint32_t) &gdt
};
