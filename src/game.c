/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "tss.h"

game_state_t game_state = {
  .remaining_calls_by_Rick_before_Morty = 10,
  .portal_called_on_cicle = FALSE,
  .I_am_Rick_code_received = 0x0000,
  .I_am_Rick_received = FALSE,
  .taskType = 0,

  .Cronenbergs_C137_count_on_round = 0,
  .Cronenbergs_D248_count_on_round = 0,
  .Cronenbergs_killed_by_C137 = 0,
  .Cronenbergs_killed_by_D248 = 0,

  .C137_portal_not_crossed_x = 0,
  .C137_portal_not_crossed_y = 0,
  .D248_portal_not_crossed_x = 0,
  .D248_portal_not_crossed_y = 0,

  .over = 0,
};

//void game_init() {
//}

void reset_cicle_counters(){
  game_state.portal_called_on_cicle = FALSE;
  game_state.I_am_Rick_code_received = 0x0000;
  game_state.I_am_Rick_received = FALSE;
}


void reset_round_counters(){
  print_dec(game_state.Cronenbergs_C137_count_on_round + game_state.Cronenbergs_killed_by_C137, 7,  4, 45, 0x1F); // 7 caracteres, columna  4, fila 45.
  print_dec(game_state.Cronenbergs_D248_count_on_round + game_state.Cronenbergs_killed_by_D248, 7, 69, 45, 0x4F); // 7 caracteres, columna 69, fila 45.
  game_state.Cronenbergs_C137_count_on_round = 0;
  game_state.Cronenbergs_D248_count_on_round = 0;
}

void game_state_set_task_type(uint32_t task_type){
  game_state.taskType = task_type;
  return;
}

uint32_t usePortalGun_C(int32_t x, int32_t y, uint32_t cross, uint32_t withMorty) {
  if (game_state.portal_called_on_cicle == FALSE) {

    if ( ( (x != 0) || (y != 0) )
        &&
       (  (game_state.taskType == 0) // Rick C
       || (game_state.taskType == 2) // Rick D
       || ( (game_state.taskType == 1) && (game_state.remaining_calls_by_Rick_before_Morty == 0) ) // Morty C
       || ( (game_state.taskType == 3) && (game_state.remaining_calls_by_Rick_before_Morty == 0) ) // Morty D
     )
     ) {

      if (game_state.remaining_calls_by_Rick_before_Morty > 0) {
        // Se decrementa cantidad de disparos faltantes para que llame un Morty.
        game_state.remaining_calls_by_Rick_before_Morty = game_state.remaining_calls_by_Rick_before_Morty - 1;
      }
      if ((game_state.taskType == 1) || (game_state.taskType == 3)  ) {
        // Si llamo un Morty hay que volver a esperar 10 turnos para que vuelva a poder llamar.
        game_state.remaining_calls_by_Rick_before_Morty = 10;
      }
      game_state.portal_called_on_cicle = TRUE;

      uint32_t rick_code;
      if (game_state.I_am_Rick_code_received == 0) {
        if (game_state.taskType < 2){
          rick_code = 0xC137;
        } else if ((2 <= game_state.taskType) && (game_state.taskType < 4)) {
          rick_code = 0xD248;
        }
      } else {
        rick_code = game_state.I_am_Rick_code_received;
      }

      // Se obtiene puntero a directorio de paginas del usuario (tarea) actual.
      uint32_t pd_addr = rcr3();

      /* SE CALCULA POSICION FISICA DE TAREA A PARTIR DE DIRECTORIO */
      uint32_t user_code_page;
      uint32_t user_x;
      uint32_t user_y;
      user_code_page = game_calculateTaskCodepageAndWorldCoords(pd_addr, &user_x, &user_y);

      // Se calcula direccion fisica apuntada por la portalGun
      int32_t pointed_x = (user_x + x);
      int32_t pointed_y = (user_y + y);
      while ( pointed_x < 0 ){
        pointed_x = pointed_x + SIZE_M;
      }
      while ( pointed_y < 0 ){
        pointed_y = pointed_y + SIZE_N;
      }
      while ( pointed_x >= SIZE_M) {
        pointed_x = pointed_x % SIZE_M;
      }
      while ( pointed_y >= SIZE_N) {
        pointed_y = pointed_y % SIZE_N;
      }

      uint32_t phys_address_pointed = (((  ( (uint32_t)SIZE_M) * pointed_y )  +  pointed_x  ) << 13);
      phys_address_pointed = phys_address_pointed + TASK_CODE_PHYS;

      uint32_t morty_index = 0;
      uint32_t morty_taskType = 0;
      if (game_state.taskType == 0) {             // Rick  C
        morty_index = GDT_IDX_TSS_USER_FIRST + 1; // Morty C
        morty_taskType = 1;
      } else if (game_state.taskType == 2) {      // Rick  D
        morty_index = GDT_IDX_TSS_USER_FIRST + 3; // Morty D
        morty_taskType = 3;
      }


      if (cross) {
        mmu_RelocateTaskPhyscally(pd_addr, TASK_CODE, user_code_page << 12, phys_address_pointed);
        game_drawOnWorld_blank(user_x, user_y);
        game_drawOnWorld_task(game_state.taskType, rick_code, TRUE, pointed_x, pointed_y);

        if (withMorty) {

          if (morty_index > 0 ){
            // Se ejecuta solamente si llamo un Rick.
            uint32_t morty_old_phys_page;
            uint32_t morty_x;
            uint32_t morty_y;
            morty_old_phys_page = game_calculateTaskCodepageAndWorldCoords(get_user_task_pd(morty_index), &morty_x, &morty_y);
            int32_t morty_new_x = (x + morty_x) /*% (int32_t)SIZE_M*/;
            int32_t morty_new_y = (y + morty_y) /*% (int32_t)SIZE_N*/;
            while ( morty_new_x < 0 ){
              morty_new_x = morty_new_x + SIZE_M;
            }
            while ( morty_new_y < 0 ){
              morty_new_y = morty_new_y + SIZE_N;
            }
            while ( morty_new_x >= SIZE_M) {
              morty_new_x = morty_new_x % SIZE_M;
            }
            while ( morty_new_y >= SIZE_N) {
              morty_new_y = morty_new_y % SIZE_N;
            }
            uint32_t morty_new_phys_address = (((  ( (uint32_t)SIZE_M) * morty_new_y )  +  morty_new_x  ) << 13);
            morty_new_phys_address = morty_new_phys_address + TASK_CODE_PHYS;
            mmu_RelocateTaskPhyscally(get_user_task_pd(morty_index), TASK_CODE, morty_old_phys_page << 12, morty_new_phys_address);
            game_drawOnWorld_blank(morty_x, morty_y);
            game_drawOnWorld_task(morty_taskType, rick_code, sched_userTaskIsAlive(morty_taskType), morty_new_x, morty_new_y);
          }
        }
      } else {

        if (withMorty && ((game_state.taskType == 0) || (game_state.taskType == 2))) {
          // Se mapea el portal para que lo vea Morty, siendo que lo dispara Rick.
          pd_addr = get_user_task_pd(morty_index);
        }

        mmu_unmapTaskCodeAndStack(pd_addr, TASK_CODE + 0x2000);
        mmu_mapTaskCodeAndStack(pd_addr, TASK_CODE + 0x2000, phys_address_pointed);

        // pisar dibujo viejo si es que tiene atributos portal (portal que no atravesado), con verde.
        screen_swichToCharAttsIfCharAtts(pointed_x, pointed_y, 0x2700, 0x2B2A);
        if (rick_code == 0xC137) {
          // pisar dibujo nuevo si es que tiene atributos verde, con portal (portal que no atravesado).
          screen_swichToCharAttsIfCharAtts(game_state.C137_portal_not_crossed_x, game_state.C137_portal_not_crossed_y, 0x2B2A, 0x2700);
          game_state.C137_portal_not_crossed_x = pointed_x;
          game_state.C137_portal_not_crossed_y = pointed_y;
        } else if (rick_code == 0xD248) {
          // pisar dibujo nuevo si es que tiene atributos verde, con portal (portal que no atravesado).
          screen_swichToCharAttsIfCharAtts(game_state.D248_portal_not_crossed_x, game_state.D248_portal_not_crossed_y, 0x2B2A, 0x2700);
          game_state.D248_portal_not_crossed_x = pointed_x;
          game_state.D248_portal_not_crossed_y = pointed_y;
        }
      }
      return TRUE; // El portal se generó por un Rick o por un Morty.
    }
  }
  return FALSE; // El portal no se pudo crear pues ya fue creado en el ciclo.
}

void IamRick_C(uint32_t code){
  game_state.I_am_Rick_code_received = code;
  if (sched_currentTask() >= 21){
    // Una tarea Cronenberg dijo "I am Rick". Significa que fue conquistada por un Rick.
    game_drawOnWorld_taskWhereItIs(rcr3(), TRUE);
    if (game_state.I_am_Rick_received == FALSE) {
      game_state.I_am_Rick_received = TRUE;
      if (code == 0xC137) {
        game_state.Cronenbergs_C137_count_on_round = game_state.Cronenbergs_C137_count_on_round +1;
      } else if (code == 0xD248) {
        game_state.Cronenbergs_D248_count_on_round = game_state.Cronenbergs_D248_count_on_round +1;
      }
    }
  }
}


void whereIsMorty_C(int32_t* x, int32_t* y) {
  uint32_t morty_index = 0;
  if (game_state.taskType == 0) {             // Rick  C
    morty_index = GDT_IDX_TSS_USER_FIRST + 1; // Morty C
  } else if (game_state.taskType == 2) {      // Rick  D
    morty_index = GDT_IDX_TSS_USER_FIRST + 3; // Morty D
  }
  if (morty_index > 0 ){
    uint32_t morty_pd_addr = get_user_task_pd(morty_index);
    uint32_t morty_x;
    uint32_t morty_y;
    game_calculateTaskCodepageAndWorldCoords(morty_pd_addr, &morty_x, &morty_y);
    uint32_t rick_pd_addr = rcr3();
    uint32_t rick_x;
    uint32_t rick_y;
    game_calculateTaskCodepageAndWorldCoords(rick_pd_addr, &rick_x, &rick_y);

    *x = morty_x - rick_x;
    *y = morty_y - rick_y;


  }
}


void game_drawOnWorld_task(uint32_t task_type, uint32_t I_am_Rick_code, uint32_t alive, uint32_t x, uint32_t y) {
  uint16_t attr;
  if (!alive) {
    attr = 0x7F;    // tarea muerta --> fondo: gris = 7
  } else {
    if (I_am_Rick_code == 0) {
      if ( (task_type == 0) || (task_type == 1) ) {
        attr = 0x1F;
      } else if ( (task_type == 2) || (task_type == 3) ) {
        attr = 0x4F;
      } else {
        attr = 0x67;
      }
    } else if (I_am_Rick_code == 0xC137) {
      if ( (task_type < 4) ) {
        attr = 0x1F;
      } else {
        attr = 0x17;
      }
    } else if (I_am_Rick_code == 0xD248) {
      if ( (task_type < 4) ) {
        attr = 0x4F;
      } else {
        attr = 0x47;
      }
    }
  }

  if ( (task_type == 0) || (task_type == 2) ) {
      print("R", x, y+1, attr);
  } else if ( (task_type == 1) || (task_type == 3) ) {
      print("M", x, y+1, attr);
  } else {
      print("+", x, y+1, attr);
  }
}


void game_drawOnWorld_blank(uint32_t x, uint32_t y) {
  print(" ", x, y+1, 0x2F);
}

void game_drawOnWorld_taskWhereItIs(uint32_t pd_addr, uint32_t alive) {
  uint32_t x;
  uint32_t y;
  game_calculateTaskCodepageAndWorldCoords(pd_addr, &x, &y);
  game_drawOnWorld_task(game_state.taskType, game_state.I_am_Rick_code_received, alive, x, y);
}


uint32_t game_calculateTaskCodepageAndWorldCoords(uint32_t task_pd_addr, uint32_t* task_x_ptr, uint32_t* task_y_ptr){



  pde_t *task_pd;
  task_pd = (pde_t *) task_pd_addr;

  // Se obtiene puntero a tabla de paginas.
  pte_t *task_pt;
  task_pt = (pte_t *) (task_pd[TASK_CODE >> 22].base << 12);       // task_pde_Index = TASK_CODE >> 22 = 0x080 >> 2 = 0x020 = 32
  // Se obtiene pagina del codigo (nivel 3) de la tarea.
  uint32_t task_code_page = (uint32_t) task_pt[(TASK_CODE & 0x003FF000) >> 12].base;
  // Se obtiene pdagina de la pila (nivel 3) de la tarea.
  //uint32_t task_stack3_page = task_pt[1].base;
  // Se calculan las coordenada 'x' e 'y' del usuario actual en el mundo Cronenberg.

  uint32_t task_worldCords = (task_code_page - (TASK_CODE_PHYS >> 12))/2;
  *task_x_ptr = task_worldCords % (uint32_t)SIZE_M;
  *task_y_ptr = task_worldCords / (uint32_t)SIZE_M;
  return task_code_page;
}

void game_pointForKiller(){

  // se obtienen coordenadas de   TAREA MUERTA

  uint32_t pd_addr = rcr3();
  uint32_t user_x;
  uint32_t user_y;
  uint32_t user_code_page = game_calculateTaskCodepageAndWorldCoords(pd_addr, &user_x, &user_y);

  // se revisa quien la pudo haber   MATADO   por haberse   DESPLAZADO SOBRE ELLA

  for (int16_t i = 0; i <=3 ; i = i+1) {

    int16_t task_index = GDT_IDX_TSS_USER_FIRST + i;
    uint32_t potentialKiller_pd_addr = get_user_task_pd(task_index);
    uint32_t potentialKiller_x;
    uint32_t potentialKiller_y;
    uint32_t potentialKiller_code_page = game_calculateTaskCodepageAndWorldCoords(potentialKiller_pd_addr, &potentialKiller_x, &potentialKiller_y);

    if ( user_code_page == potentialKiller_code_page ) {
      // El "equipo asesino" recibe puntaje   por tarea asesinada por desplazamiento sobre ella.
      if ( i < 2 ) {
          game_state.Cronenbergs_killed_by_C137 = game_state.Cronenbergs_killed_by_C137 + 1;
      } else if ( (2 <= i) && (i < 4) ) {
          //breakpoint ();
          game_state.Cronenbergs_killed_by_D248 = game_state.Cronenbergs_killed_by_D248 + 1;
      }
    }
  }

  // se revisa quien la pudo haber   MATADO   por apuntarle portal sin cruzarlo, en intento fallido de conquistarla
  // Si dos portales justo apuntan al mismo lugar,   no se puede determinar el asesino y no se hace nada.
  if ( ( game_state.C137_portal_not_crossed_x != game_state.D248_portal_not_crossed_x )
       ||
       ( game_state.C137_portal_not_crossed_y != game_state.D248_portal_not_crossed_y )
     )
  {
    if ( (game_state.C137_portal_not_crossed_x == user_x) && (game_state.C137_portal_not_crossed_y == user_y)) {

      // C137 mato una tarea y recibe puntaje.
      game_state.Cronenbergs_killed_by_C137 = game_state.Cronenbergs_killed_by_C137 + 1;

    } else if ( (game_state.D248_portal_not_crossed_x == user_x) && (game_state.D248_portal_not_crossed_y == user_y)) {

      // C137 mato una tarea y recibe puntaje.
      game_state.Cronenbergs_killed_by_D248 = game_state.Cronenbergs_killed_by_D248 + 1;

    }
  }

    /* Si ademas,   la tarea muerta es algun RICK,
      se setea game_state.over en verdadero,
      siendo el rick que quedo vivo el ganador.*/

  if ( (game_state.taskType == 0) ) {

    // Murio   Rick 0xC137
    game_state.over = TRUE;

    // y gana  Rick 0xD248
    screen_drawGameOverBox( 0xD248, 0xC137 );

  } else if ( (game_state.taskType == 2) ) {

    // Murio   Rick 0xD248
    game_state.over = TRUE;

    // y gana  Rick 0xC137
    screen_drawGameOverBox( 0xC137, 0xD248 );

  }
}


uint8_t game_over(){
  return game_state.over;
}
