/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "screen.h"
#include "i386.h"
#include "mmu.h"
#include "defines.h"


uint32_t screen_backup_pages;

void screen_backup_init(){
  screen_backup_pages = mmu_nextFreeKernelPage();
                        mmu_nextFreeKernelPage();
  return;
}

void screen_backup(){
  uint16_t *src = (uint16_t *) VIDEO;
  uint16_t *dst = (uint16_t *) screen_backup_pages;
  for (uint32_t i = 0; i < 80*50; i++) {
    dst[i] = src[i];
  }
  return;
}

void screen_restore(){
  uint16_t *src = (uint16_t *) screen_backup_pages;
  uint16_t *dst = (uint16_t *) VIDEO;
  for (uint32_t i = 0; i < 80*50; i++) {
    dst[i] = src[i];
  }
  return;
}

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t) text[i];
    p[y][x].a = (uint8_t) attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
  return;
}


void print_n(const char* text, uint32_t n, uint32_t x, uint32_t y, uint16_t attr) {
  ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
  int32_t i;
  for (i = 0; ( text[i] != 0 ) && ( i < n ) ; i++) {
    p[y][x].c = (uint8_t) text[i];
    p[y][x].a = (uint8_t) attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
  return;
}


void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y, uint16_t attr) {
  ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for(i = 0; i < size; i++) {
    uint32_t resto  = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
  return;
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y, uint16_t attr) {
  ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
  hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
  hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
  hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
  hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
  hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
  hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
  hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
  for(i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
  return;
}

void screen_drawBox(uint32_t fInit,
                    uint32_t cInit,
                    uint32_t fSize,
                    uint32_t cSize,
                    uint8_t character,
                    uint8_t attr ) {
  ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit+fSize; f++) {
    for (c = cInit; c < cInit+cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
  return;
}

void screen_drawDebugBox( uint32_t eax,
                          uint32_t ebx,
                          uint32_t ecx,
                          uint32_t edx,
                          uint32_t esi,
                          uint32_t edi,
                          uint32_t ebp,
                          uint32_t esp,
                          uint32_t eip,
                          uint32_t cs,
                          uint32_t ds,
                          uint32_t es,
                          uint32_t fs,
                          uint32_t gs,
                          uint32_t ss,
                          uint32_t eflags,
                          uint32_t cr0,
                          uint32_t cr2,
                          uint32_t cr3,
                          uint32_t cr4 ) {
  screen_drawBox(1,25,40,30,0,0);

  screen_drawBox(4,26,36,28,0,0x70);

  print("eax", 27,  6, 0x70);     print_hex(eax, 8, 31,  6, 0x7F);
  print("ebx", 27,  8, 0x70);     print_hex(ebx, 8, 31,  8, 0x7F);
  print("ecx", 27, 10, 0x70);     print_hex(ecx, 8, 31, 10, 0x7F);
  print("edx", 27, 12, 0x70);     print_hex(edx, 8, 31, 12, 0x7F);
  print("esi", 27, 14, 0x70);     print_hex(esi, 8, 31, 14, 0x7F);
  print("edi", 27, 16, 0x70);     print_hex(edi, 8, 31, 16, 0x7F);
  print("ebp", 27, 18, 0x70);     print_hex(ebp, 8, 31, 18, 0x7F);
  print("esp", 27, 20, 0x70);     print_hex(esp, 8, 31, 20, 0x7F);
  print("eip", 27, 22, 0x70);     print_hex(eip, 8, 31, 22, 0x7F);
  print(" cs", 27, 24, 0x70);     print_hex( cs, 4, 31, 24, 0x7F);
  print(" ds", 27, 26, 0x70);     print_hex( ds, 4, 31, 26, 0x7F);
  print(" es", 27, 28, 0x70);     print_hex( es, 4, 31, 28, 0x7F);
  print(" fs", 27, 30, 0x70);     print_hex( fs, 4, 31, 30, 0x7F);
  print(" gs", 27, 32, 0x70);     print_hex( gs, 4, 31, 32, 0x7F);
  print(" ss", 27, 34, 0x70);     print_hex( ss, 4, 31, 34, 0x7F);
  print(" eflags", 27, 37, 0x70); print_hex(eflags, 8, 35, 37, 0x7F);

  print("cr0", 41,  7, 0x70);     print_hex(cr0, 8, 45,  7, 0x7F);
  print("cr2", 41,  9, 0x70);     print_hex(cr2, 8, 45,  9, 0x7F);
  print("cr3", 41, 11, 0x70);     print_hex(cr3, 8, 45, 11, 0x7F);
  print("cr4", 41, 13, 0x70);     print_hex(cr4, 8, 45, 13, 0x7F);

  print("stack", 36, 27, 0x70);

  print("[esp]", 36, 29, 0x70);
  print("[esp+4]", 36, 30, 0x70);
  print("[esp+8]", 36, 31, 0x70);
  print("[esp+12]", 36, 32, 0x70);
  print("[esp+16]", 36, 33, 0x70);

  return;
}

void screen_ssInvalido(){
  print("invalid ss", 43, 27, 0x74);
}

void print_exception(uint32_t codigo, uint16_t pos_x, uint16_t pos_y){
  uint16_t formato = 0x70;
  print(" EXCEPTION ",         pos_x +  0, pos_y + 1, formato);
  if (codigo < 10) {
    print("             ",   pos_x +  0, pos_y + 0, formato);
    print_dec(codigo, 1,     pos_x + 11, pos_y + 1, formato);
    print(" ",               pos_x + 12, pos_y + 1, formato);
    print("             ",   pos_x +  0, pos_y + 2, formato);
  } else if (codigo < 100) {
    print("              ",  pos_x +  0, pos_y + 0, formato);
    print_dec(codigo, 2,     pos_x + 11, pos_y + 1, formato);
    print(" ",               pos_x + 13, pos_y + 1, formato);
    print("              ",  pos_x +  0, pos_y + 2, formato);
  } else {
    print("               ", pos_x +  0, pos_y + 0, formato);
    print_dec(codigo, 3,     pos_x + 11, pos_y + 1, formato);
    print(" ",               pos_x + 14, pos_y + 1, formato);
    print("              ",  pos_x +  0, pos_y + 2, formato);
  }
}

void print_NumTopRightEdge(uint32_t teclaCodigoIBM){

  if ( teclaCodigoIBM >= 2 && teclaCodigoIBM <= 11 ) {

    // Teclas num\'ericas continuas horizontales.
    print_dec(teclaCodigoIBM -1, 1, 79, 0, 0xA);//1 caracter, columna 79, fila 0

    // Todos los otros casos son del teclado num\'erico derecho.

  } else if ( teclaCodigoIBM == 199 ) {
    print_dec(7, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 200 ) {
    print_dec(8, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 201 ) {
    print_dec(9, 1, 79, 0, 0xA);

  } else if ( teclaCodigoIBM == 203 ) {
    print_dec(4, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 204 ) {
    print_dec(5, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 205 ) {
    print_dec(6, 1, 79, 0, 0xA);

  } else if ( teclaCodigoIBM == 207 ) {
    print_dec(1, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 208 ) {
    print_dec(2, 1, 79, 0, 0xA);
  } else if ( teclaCodigoIBM == 209 ) {
    print_dec(3, 1, 79, 0, 0xA);

  } else if ( teclaCodigoIBM == 210 ) {
    print_dec(0, 1, 79, 0, 0xA);

  }
}

void screen_showDebugModeLegend(){
  uint16_t formato = 0x0F;
  uint16_t pos_x = 34;
  uint16_t pos_y = 0;
  print(" DEBUG MODE ", pos_x, pos_y,formato);
}
void screen_hideDebugModeLegend(){
  uint16_t formato = 0x0F;
  uint16_t pos_x = 34;
  uint16_t pos_y = 0;
  print("            ", pos_x, pos_y,formato);
}

void screen_swichToCharAttsIfCharAtts(uint32_t x, uint32_t y, uint16_t old_atts, uint16_t new_atts){
  uint16_t *video = (uint16_t *) 0xB8000;
  if (video[((uint32_t)SIZE_M) * y + x + SIZE_M] == old_atts ){
    video[((uint32_t)SIZE_M) * y + x + SIZE_M] =  new_atts;
  }
}

void screen_drawGameOverBox( uint16_t winner, uint16_t dead ) {
  screen_drawBox(19,25,7,34,0,0);
  //print_exception(exception, 33, 1);
  screen_drawBox(20,26,5,32,0,0x70);

  print_hex(winner, 4, 28,  22, 0x70);
  print("winner!!", 33,  22, 0x70);

  if ( dead != 0 ) {
    print("(      dead )", 43,  22, 0x70);
    print_hex(dead, 4, 45,  22, 0x70);
  }
  return;
}
