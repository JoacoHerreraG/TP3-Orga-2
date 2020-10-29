/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __TASK_H__
  #define __TASK_H__
  const uint8_t valuesX[24] = {2,  63, 19, 36, 30, 79, 21, 10, 73, 32, 70, 28, 73, 64, 28, 12,  3, 41, 27, 24, 54, 20, 33, 74};
  const uint8_t valuesY[24] = {31, 32, 39,  5, 37, 29, 12, 24,  0,  4,  2, 16, 15, 24, 24, 19, 23, 38, 12, 25, 14, 22,  0, 28};

  void getRandomPosition(uint8_t index, uint8_t *x, uint8_t *y) {
    *x = valuesX[index%24];
    *y = valuesY[index%24];
  }
#endif  /* !__TASK_H__ */
