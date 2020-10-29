/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
  #define __GAME_H__

  #include "stdint.h"
  #include "defines.h"
  #include "screen.h"
  #include "mmu.h"
  #include "sched.h"

//  typedef enum e_taskType {
//      RickC137       = 1,
//      MortyC137      = 2,
//      RickD248       = 3,
//      MortyD248      = 4,
//      Cronenberg     = 5,
//      CronenbergC137 = 6,
//      CronenbergD248 = 7,
//  } e_taskType_t;

  typedef struct str_game_state {
    uint32_t I_am_Rick_code_received;
    uint8_t  I_am_Rick_received;

    uint8_t  remaining_calls_by_Rick_before_Morty;
    uint8_t  portal_called_on_cicle;
    uint8_t  taskType;

    uint8_t  Cronenbergs_C137_count_on_round;
    uint8_t  Cronenbergs_D248_count_on_round;
    uint8_t  Cronenbergs_killed_by_C137;
    uint8_t  Cronenbergs_killed_by_D248;

    uint8_t  C137_portal_not_crossed_x;
    uint8_t  C137_portal_not_crossed_y;
    uint8_t  D248_portal_not_crossed_x;
    uint8_t  D248_portal_not_crossed_y;

    uint8_t  over;
  } __attribute__((__packed__)) game_state_t;



  void game_init();

  void reset_cicle_counters();
  void reset_round_counters();

  void game_state_set_task_type();

  uint32_t usePortalGun_C(int32_t x, int32_t y, uint32_t cross, uint32_t withMorty);
  void IamRick_C();

  void game_drawOnWorld_blank(uint32_t x, uint32_t y);
  void game_drawOnWorld_task(uint32_t, uint32_t, uint32_t, uint32_t x, uint32_t y);

  uint32_t game_calculateTaskCodepageAndWorldCoords(uint32_t task_pd_addr, uint32_t* task_x_ptr, uint32_t* task_y_ptr);
  void game_drawOnWorld_taskWhereItIs(uint32_t pd_addr, uint32_t alive);

  void game_pointForKiller();
  uint8_t game_over();

#endif  /* !__GAME_H__ */
