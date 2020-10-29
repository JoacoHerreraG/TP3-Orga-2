/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__

  #define __DEFINES_H__

  /* Bool */
  /* -------------------------------------------------------------------------- */
  #define TRUE                    0x00000001
  #define FALSE                   0x00000000
  #define ERROR                   1

  /* Atributos paginas */
  /* -------------------------------------------------------------------------- */
  #define PAG_P                   0x00000001
  #define PAG_R                   0x00000000
  #define PAG_RW                  0x00000002
  #define PAG_S                   0x00000000
  #define PAG_US                  0x00000004

  /* Misc */
  /* -------------------------------------------------------------------------- */
  #define SIZE_N                  40 // Y
  #define SIZE_M                  80 // X

  /* Indices en la gdt */
  /* -------------------------------------------------------------------------- */
  #define GDT_COUNT 42

  #define GDT_IDX_NULL_DESC          0
  #define GDT_IDX_CS_RING_0   			 8
  #define GDT_IDX_DS_RING_0   			 9
  #define GDT_IDX_CS_RING_3   			10
  #define GDT_IDX_DS_RING_3   			11
  #define GDT_IDX_DS_VIDEO_RING_0		12
  #define GDT_IDX_TSS_INIT    			13
  #define GDT_IDX_TSS_IDLE			    14
  #define GDT_IDX_TSS_USER_FIRST    17  // tarea de usuario 0    (o 1era)
  #define GDT_IDX_TSS_USER_LAST     40  // tarea de usuario 23   (o 24ava)

  //#define CS_RING_0			8
  //#define DS_RING_0			9

  /* Offsets en la gdt */
  /* -------------------------------------------------------------------------- */
  #define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC << 3)


  /* Selectores de segmentos */
  /* -------------------------------------------------------------------------- */


  /* Direcciones de memoria */
  /* -------------------------------------------------------------------------- */
  #define BOOTSECTOR               0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
  #define KERNEL                   0x00001200 /* direccion fisica de comienzo del kernel */
  #define VIDEO                    0x000B8000 /* direccion fisica del buffer de video */

  /* Direcciones virtuales de codigo, pila y datos */
  /* -------------------------------------------------------------------------- */
  #define TASK_CODE             0x08000000 /* direccion virtual del codigo */

  /* Direcciones fisicas de codigos */
  /* -------------------------------------------------------------------------- */
  /* En estas direcciones estan los codigos de todas las tareas. De aqui se
   * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
   */

  #define TASK_CODE_PHYS           0x00400000



  /* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
  /* -------------------------------------------------------------------------- */
  #define KERNEL_PAGE_DIR          0x00027000
  #define KERNEL_PAGE_TABLE_0      0x00028000
  #define CRYSTALS_MAP             0x00029000

#endif  /* !__DEFINES_H__ */
