/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
  #define __SCHED_H__

  #include "stdint.h"
  #include "screen.h"
  #include "tss.h"

  void sched_init();

  int16_t sched_currentTask();
  void sched_killCurrentTask();
  int16_t sched_nextTask();
  void sched_setDebugStop();
  void sched_unsetDebugStop();
  int8_t sched_isStopped();
  void sched_taskNextClock();
  void print_taskState();
  uint32_t sched_userTaskIsAlive(uint32_t i);

#endif	/* !__SCHED_H__ */
