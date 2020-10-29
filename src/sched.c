/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "sched.h"
#include "defines.h"
#include "game.h"

int16_t user_task_current;

// Se declara variable global de booleanos que
// indica tareas de usuario vivas con verdadero y
//        tareas de usuario muertas (por una excepcion) con falso.
// Los indices coinciden con los de las tss de usuarios en la gdt.
// Indices por fuera de tareas de usuario no interesan y van a contener basura.
int8_t user_task_alive[GDT_IDX_TSS_USER_LAST + 1];

int8_t sched_task_clock[GDT_IDX_TSS_USER_LAST + 1];
char sched_clocks_ascii[5]; // "|/-\x";


int8_t debug_stop;

void sched_init() {
  // Se inicia el contador de tarea de usuario actual en ultima tarea
  // para que la proxima incrementacion quede en la primera
  // y la primera tarea de usuario ejecutada sea efectivamente la primera.
  user_task_current = GDT_IDX_TSS_USER_LAST;

  // Se carga arreglo con listado de tareas 'tipo usuario' todas con 'verdadero' (todas comienzan vivas).
  for (int16_t i = GDT_IDX_TSS_USER_FIRST; i <= GDT_IDX_TSS_USER_LAST; i++ ) {
    user_task_alive[i] = (int8_t) TRUE;
  }

  // Caracteres necesarios para dibujar relojes en panel inferior.
  sched_clocks_ascii[0] = 0x7C; // "|"
  sched_clocks_ascii[1] = 0x2F; // "/"
  sched_clocks_ascii[2] = 0x2D; // "-"
  sched_clocks_ascii[3] = 0x5C; // "\"
  sched_clocks_ascii[4] = 0x78; // "x"

  // Etiquetas de ESTADO   tareas Rick y Morty.
  print("R", 18, 43, 0x07);
  print("M", 22, 43, 0x07);
  print("R", 26, 43, 0x07);
  print("M", 30, 43, 0x07);

  // Etiquetas de ESTADO   tareas Cronenberg ( 00 hasta 07 ).
  for (uint16_t i = 4; i < 12; i = i+1){
    uint16_t row = 43;
    uint16_t col = 18 + (i * 4);
    print_dec(i-4, 2, col, row, 0x07);
  }

  // Etiquetas de ESTADO   tareas Cronenberg ( 08 hasta 19 ).
  for (uint16_t i = 12; i < 24; i = i+1){
    uint16_t row = 46;
    uint16_t col = (i * 4) -30;
    print_dec(i-4, 2, col, row, 0x07);
  }

}

int16_t sched_currentTask() {
  return user_task_current;
}

void sched_killCurrentTask() {
  user_task_alive[user_task_current] = (int8_t) FALSE;
  game_pointForKiller(user_task_current);
  game_drawOnWorld_taskWhereItIs(get_user_task_pd(user_task_current), FALSE);
  print_taskState();
  return;
}

uint32_t sched_userTaskIsAlive(uint32_t i) {
  return user_task_alive[i + GDT_IDX_TSS_USER_FIRST];
}

void sched_setDebugStop() {
  debug_stop = (int8_t) TRUE;
  return;
}

void sched_unsetDebugStop() {
  debug_stop = (int8_t) FALSE;
  return;
}

int8_t sched_isStopped() {
  return debug_stop;
}

int16_t sched_nextTask() {

  // Se recorre arreglo de tareas 'tipo usuario' vivas y busca siguiente 'viva'.

  int16_t current = user_task_current;
  int16_t i = current;

  /*if (i == (GDT_IDX_TSS_USER_LAST-1)) {
      //breakpoint();
  } else if (i == (GDT_IDX_TSS_USER_LAST)) {
      //breakpoint();
  }*/

  if ( game_over() ) {

    user_task_current = GDT_IDX_TSS_IDLE;

  } else {

    if (i == GDT_IDX_TSS_USER_LAST) {
      i = GDT_IDX_TSS_USER_FIRST;
      reset_round_counters();
    } else {
      i = i + 1;
    }

    int8_t next_not_found = (int8_t) TRUE;

    while ( next_not_found && (i != current) ) {
      if (user_task_alive[i] == (int8_t) TRUE) {
        next_not_found = (int8_t) FALSE;
      } else {
        if (i == GDT_IDX_TSS_USER_LAST) {
          i = GDT_IDX_TSS_USER_FIRST;
          reset_round_counters();
        } else {
          i = i + 1;
        }
      }
    }

    user_task_current = i;

    //uint32_t a = i - GDT_IDX_TSS_USER_FIRST;
    //breakpoint();
    //a = a + a;
    game_state_set_task_type(i - GDT_IDX_TSS_USER_FIRST);   // 0 es Rick C,   1 es Morty C,   2 es Rick D,   3 es Morty D,   y mayores son cronenbergs, no nos importa que no sean todos los cronenberg iguales.

  }
  return user_task_current;
}

void sched_taskNextClock() {
  uint32_t i = user_task_current;
  sched_task_clock[i] = sched_task_clock[i] + 1;
  if (sched_task_clock[i] == 4) {
    sched_task_clock[i] = 0;
  }
  print_taskState();
}

void print_taskState() {
  uint32_t task = user_task_current;
  uint16_t user_task_number = task - GDT_IDX_TSS_USER_FIRST;
  char* caracter;
  caracter = sched_clocks_ascii;

  uint16_t atts;

  if (user_task_alive[task]) {
    if (user_task_number < 2) {
      atts = 0x1F;
    } else if ((2 <= user_task_number) && (user_task_number < 4)){
      atts = 0x4F;
    } else {
      atts = 0x6F;
    }
    caracter = (char*) ( (uint32_t) caracter + sched_task_clock[task] );
  } else {
    atts = 0x74;
    caracter = (char*) ( (uint32_t) caracter + 4 );
  }

  uint16_t row = 42;
  uint16_t col = 18 + (user_task_number * 4);
  if (user_task_number > 11 ) {
    col = col - 48;
    row = 45;
  }

  print_n(caracter, 1, col, row, atts);
  return;
}
