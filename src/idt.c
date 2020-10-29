/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"
#include "tss.h"

idt_entry idt[255] = { }; // una idt_entry mide 64 bit  => IDT mide 64 x 256 bit

idt_descriptor IDT_DESC = {
  sizeof(idt) - 1,    // 15 bit bajos de IDTR guardan limite        de IDT.
  (uint32_t) &idt     // 32 bit altos de IDTR guardan direcion base de IDT.
};

/*
    La siguiente es una macro de EJEMPLO para ayudar a armar entradas de
    interrupciones. Para usar, descomentar y completar CORRECTAMENTE los
    atributos y el registro de segmento. Invocarla desde idt_inicializar() de
    la siguiene manera:

    void idt_inicializar() {
        IDT_ENTRY(0);
        ...
        IDT_ENTRY(19);
        ...
    }
*/

#define INT_SEL 0x40   // CS_RING_0

                       // P=1 (present), DPL=00 (desc. privilege level 0), 0,
                       // D=1 (32bit), 110 (interrupt gate), 000, XXXXX  =
#define INT_ATT 0x8E00 // 1 00 0 1 110 000 00000 = 1000 1110 0000 0000 = 0x8E00

#define IDT_ENTRY(numero)                                                                        \
  idt[numero].offset_0_15 = (uint16_t) ((uint32_t)(&_isr ## numero) & (uint32_t) 0xFFFF);        \
  idt[numero].segsel = (uint16_t) INT_SEL;                                                       \
  idt[numero].attr = (uint16_t) INT_ATT;                                                         \
  idt[numero].offset_16_31 = (uint16_t) ((uint32_t)(&_isr ## numero) >> 16 & (uint32_t) 0xFFFF);

                         // P=1 (present), DPL=11 (desc. privilege level 3), 0,
                         // D=1 (32bit), 110 (interrupt gate), 000, XXXXX  =
#define INT_ATT_3 0xEE00 // 1 11 0 1 110 000 00000 = 1110 1110 0000 0000 = 0xEE00

#define IDT_ENTRY_3(numero)                                                                      \
  idt[numero].offset_0_15 = (uint16_t) ((uint32_t)(&_isr ## numero) & (uint32_t) 0xFFFF);        \
  idt[numero].segsel = (uint16_t) INT_SEL;                                                       \
  idt[numero].attr = (uint16_t) INT_ATT_3;                                                       \
  idt[numero].offset_16_31 = (uint16_t) ((uint32_t)(&_isr ## numero) >> 16 & (uint32_t) 0xFFFF);



void idt_init() {

  // Se carcan los descriptores
  // en la tabla de descriptores de interrupciones (IDT).

  // Con la macro IDT_ENTRY se genera el c'odigo C
  // para cargar cada entrada a la estructura.

  IDT_ENTRY(0);
  IDT_ENTRY(1);
  IDT_ENTRY(2);
  IDT_ENTRY(3);
  IDT_ENTRY(4);
  IDT_ENTRY(5);
  IDT_ENTRY(6);
  IDT_ENTRY(7);
  IDT_ENTRY(8);
  IDT_ENTRY(9);
  IDT_ENTRY(10);
  IDT_ENTRY(11);
  IDT_ENTRY(12);
  IDT_ENTRY(13);
  IDT_ENTRY(14);
  IDT_ENTRY(15);
  IDT_ENTRY(16);
  IDT_ENTRY(17);
  IDT_ENTRY(18);
  IDT_ENTRY(19);
  IDT_ENTRY(20);
  IDT_ENTRY(21);
  IDT_ENTRY(22);
  IDT_ENTRY(23);
  IDT_ENTRY(24);
  IDT_ENTRY(25);
  IDT_ENTRY(26);
  IDT_ENTRY(27);
  IDT_ENTRY(28);
  IDT_ENTRY(29);
  IDT_ENTRY(30);
  IDT_ENTRY(31);
  IDT_ENTRY(32);//     Interrupcion de reloj.    Entrada   32 = 0x20   de la IDT.
  IDT_ENTRY(33);//     Interrupcion de teclado.  Entrada   33 = 0x21   de la IDT.
  IDT_ENTRY_3(137);//  Interrupcion de software.Llamada al sistema usePortalGun  tiene que poder ser llamada desde nivel 3 (usuario).
  IDT_ENTRY_3(138);//  Interrupcion de software.Llamada al sistema iAmRick       tiene que poder ser llamada desde nivel 3 (usuario).
  IDT_ENTRY_3(139);//  Interrupcion de software.Llamada al sistema whereIsMorty  tiene que poder ser llamada desde nivel 3 (usuario).
}
