; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:   dd 0xFFFFFFFF
sched_task_selector:   dw 0xFFFF


debug_mode: dd 0x00000000

tmp: dd 0x00000000
tmp2: dd 0x00000000

debug_eax: dd 0x00000000
debug_ebx: dd 0x00000000
debug_ecx: dd 0x00000000
debug_edx: dd 0x00000000
debug_esi: dd 0x00000000
debug_edi: dd 0x00000000
debug_ebp: dd 0x00000000

debug_eip: dd 0x00000000

debug_cs:  dd 0x00000000
debug_ds:  dd 0x00000000
debug_es:  dd 0x00000000
debug_fs:  dd 0x00000000
debug_gs:  dd 0x00000000

debug_esp: dd 0x00000000
debug_ss:  dd 0x00000000

debug_eflags: dd 0x00000000

debug_cr0: dd 0x00000000
debug_cr2: dd 0x00000000
debug_cr3: dd 0x00000000
debug_cr4: dd 0x00000000

debug_stack_0: dd 0x00000000
debug_stack_4: dd 0x00000000
debug_stack_8: dd 0x00000000
debug_stack_C: dd 0x00000000

debug_error_code: dd 0x00000000


;; PIC
extern pic_finish1

;; Sched
extern sched_nextTask
extern sched_killCurrentTask
extern sched_setDebugStop
extern sched_unsetDebugStop
extern sched_isStopped
extern sched_taskNextClock

extern print_exception
extern print_NumTopRightEdge

extern reset_cicle_counters
extern usePortalGun_C
extern IamRick_C
extern whereIsMorty_C

extern screen_showDebugModeLegend
extern screen_hideDebugModeLegend
extern screen_drawBox
extern screen_drawDebugBox
extern screen_backup
extern screen_restore
extern screen_ssInvalido

extern print_hex

;;
;; Definicion de MACROS
;; -------------------------------------------------------------------------- ;;


;; MACRO PARA GENERAR RUTINAS DE ATENCION DE INTERRUPCIONES QUE  **NO**  PUSHEAN CODIGO DE ERROR.

%macro ISR 1

  global _isr%1

  _isr%1:
    ;xchg bx, bx
    pushad

    ; REGISTROS DEL PUSHAD

      mov edx, [esp]  ; edi
      mov [debug_edi], edx

      mov edx, [esp+4]  ; esi
      mov [debug_esi], edx

      mov edx, [esp+8]  ; ebp
      mov [debug_ebp], edx

      ;mov edx, [esp+12] ; esp  ; el ESP lo sacamos de mas abajo en la pila,  de la parte debida a la excepcion.
      ;mov [debug_esp], edx

      mov edx, [esp+16] ; ebx
      mov [debug_ebx], edx

      mov edx, [esp+20] ; edx
      mov [debug_edx], edx

      mov edx, [esp+24] ; ecx
      mov [debug_ecx], edx

      mov edx, [esp+28] ; eax
      mov [debug_eax], edx

    ; PUSHEOS debido a la INTERRUPCION.

    mov edx, [esp+32]
    mov [debug_eip], edx

    mov edx, [esp+36]
    mov [debug_cs], edx

    mov edx, [esp+40]
    mov [debug_eflags], edx

    mov edx, [esp+44]
    mov [debug_esp], edx

    mov edx, [esp+48]
    mov [debug_ss], edx

    call sched_killCurrentTask  ; Se marca la tarea actual como muerta.

    cmp word [debug_mode], 0x0DEB
    jne .onlyExceptionLabel

    ;showDebugWindow:
      call screen_backup

      mov eax, cr4
      push eax
      mov eax, cr3
      push eax
      mov eax, cr2
      push eax
      mov eax, cr0
      push eax
      push dword [debug_eflags]
      push dword [debug_ss]
      push gs
      push fs
      push es
      push ds
      push dword [debug_cs]
      push dword [debug_eip]
      push dword [debug_esp]
      push dword [debug_ebp]
      push dword [debug_edi]
      push dword [debug_esi]
      push dword [debug_edx]
      push dword [debug_ecx]
      push dword [debug_ebx]
      push dword [debug_eax]
      call screen_drawDebugBox
      add esp, 80
      ;xchg bx, bx


      mov eax, 1
      push eax
      mov eax, 27
      push eax
      mov eax, %1
      push eax
      call print_exception
      add esp, 12


      ; Listado pila nivel 3, maximo 4 posiciones.

      mov ax, [debug_ss]

      ; Se chequea que el selector de segmento no sea un valor muy fuera de los validos.
      mov cx, 40; GDT_IDX_TSS_USER_LAST = 40
      shl ecx, 3

      cmp ecx, eax
      jl .ssInvalido

      mov [tmp], es
      mov es, ax
      mov esi, dword [debug_esp]

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug primer valor de la pila de nivel 3.
      mov ebx, 29
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      jmp .finListadoPila

      .ssInvalido:
      call screen_ssInvalido ; Se informa que el selector de segmento obtenido es invalido (aunque no debería llegar a ejecutarse esto.). Es para evitar un loop de excepciones sin fin en caso de que ss de mal y no podamos leer la pila.

      .finListadoPila:

      mov ax, [tmp]
      mov es, ax

      call sched_setDebugStop

      jmp .jmpFar

    .onlyExceptionLabel:
      mov eax, 47
      push eax
      mov eax, 33
      push eax
      mov eax, %1
      push eax
      call print_exception
      add esp, 12   ; En convenci'on C 32 bit los par'ametros se pasan por pila.

    .jmpFar:

    popad
    mov word [sched_task_selector], 0x70  ; Se salta a tarea Idle
    jmp far [sched_task_offset]

    ; Nunca se van a ejecutar las lineas que siguen pues la tarea murio
    jmp $

%endmacro



;; MACRO PARA GENERAR RUTINAS DE ATENCION DE INTERRUPCIONES QUE  **SI**  PUSHEAN CODIGO DE ERROR.

%macro ISR_withErrorCode 1

  global _isr%1

  _isr%1:
    ;xchg bx, bx
    pushad

    ; REGISTROS DEL PUSHAD

      mov edx, [esp]  ; edi
      mov [debug_edi], edx

      mov edx, [esp+4]  ; esi
      mov [debug_esi], edx

      mov edx, [esp+8]  ; ebp
      mov [debug_ebp], edx

      ;mov edx, [esp+12] ; esp  ; el ESP lo sacamos de mas abajo en la pila,  de la parte debida a la excepcion.
      ;mov [debug_esp], edx

      mov edx, [esp+16] ; ebx
      mov [debug_ebx], edx

      mov edx, [esp+20] ; edx
      mov [debug_edx], edx

      mov edx, [esp+24] ; ecx
      mov [debug_ecx], edx

      mov edx, [esp+28] ; eax
      mov [debug_eax], edx

    ; PUSHEOS debido a la INTERRUPCION.

    mov edx, [esp+32]
    mov [debug_error_code], edx

    mov edx, [esp+36]
    mov [debug_eip], edx

    mov edx, [esp+40]
    mov [debug_cs], edx

    mov edx, [esp+44]
    mov [debug_eflags], edx

    mov edx, [esp+48]
    mov [debug_esp], edx

    mov edx, [esp+52]
    mov [debug_ss], edx

    call sched_killCurrentTask  ; Se marca la tarea actual como muerta.


    cmp word [debug_mode], 0x0DEB
    jne .onlyExceptionLabel

    ;showDebugWindow:
      call screen_backup

      mov eax, cr4
      push eax
      mov eax, cr3
      push eax
      mov eax, cr2
      push eax
      mov eax, cr0
      push eax
      push dword [debug_eflags]
      push dword [debug_ss]
      push gs
      push fs
      push es
      push ds
      push dword [debug_cs]
      push dword [debug_eip]
      push dword [debug_esp]
      push dword [debug_ebp]
      push dword [debug_edi]
      push dword [debug_esi]
      push dword [debug_edx]
      push dword [debug_ecx]
      push dword [debug_ebx]
      push dword [debug_eax]
      call screen_drawDebugBox
      add esp, 80

      mov eax, 1
      push eax
      mov eax, 27
      push eax
      mov eax, %1
      push eax
      call print_exception
      add esp, 12


      ; Listado pila nivel 3, maximo 4 posiciones.

      mov ax, [debug_ss]

      ; Se chequea que el selector de segmento no sea un valor muy fuera de los validos.
      mov cx, 40; GDT_IDX_TSS_USER_LAST = 40
      shl ecx, 3

      cmp ecx, eax
      jl .ssInvalido

      mov [tmp], es
      mov es, ax
      mov esi, dword [debug_esp]

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug primer valor de la pila de nivel 3.
      mov ebx, 29
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      add esi, 4

      ; Se verifica si se llego a la base de la pila.
      lea edi, [es:esi]
      cmp edi, 0x8002000
      je .finListadoPila
      ;xchg bx, bx
      ; Se lee y escribe en pantalla de debug el siguiente valor de la pila de nivel 3.
      add ebx, 1
      push 0x7F
      push ebx
      push 45
      push 8
      push dword [es:esi]
      call print_hex
      add esp, 20

      jmp .finListadoPila

      .ssInvalido:
      call screen_ssInvalido ; Se informa que el selector de segmento obtenido es invalido (aunque no debería llegar a ejecutarse esto.). Es para evitar un loop de excepciones sin fin en caso de que ss de mal y no podamos leer la pila.

      .finListadoPila:

      mov ax, [tmp]
      mov es, ax

      call sched_setDebugStop

      jmp .jmpFar

    .onlyExceptionLabel:
      mov eax, 47
      push eax
      mov eax, 33
      push eax
      mov eax, %1
      push eax
      call print_exception
      add esp, 12   ; En convenci'on C 32 bit los par'ametros se pasan por pila.

    .jmpFar:

    popad

    add esp, 4  ; se acomoda el push generado por el codigo de error

    mov word [sched_task_selector], 0x70  ; Se salta a tarea Idle
    jmp far [sched_task_offset]

    ; Nunca se van a ejecutar las lineas que siguen pues la tarea murio
    jmp $

%endmacro




;; Rutina de atencion de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;

; Se llama a la macro ISR que genera el c'odigo ASM
; para definir las funciones que van recibir los c'odigos de interrupci'on
; hacer llamados particulares para atenderlas.

ISR        0
ISR        1
ISR        2
ISR        3
ISR        4
ISR        5
ISR        6
ISR        7
ISR_withErrorCode  8
ISR        9
ISR_withErrorCode 10
ISR_withErrorCode 11
ISR_withErrorCode 12
ISR_withErrorCode 13
ISR_withErrorCode 14
ISR         15
ISR         16
ISR_withErrorCode 17
ISR         18
ISR         19
ISR         20
ISR         21
ISR         22
ISR         23
ISR         24
ISR         25
ISR         26
ISR         27
ISR         28
ISR         29
ISR         30
ISR         31

;; Rutina de atencion del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
  pushad
  call pic_finish1

  call sched_isStopped
  cmp al, 1
  je .fin ; Si esta stopeado estamos en idle, y vamos a mantenernos ahi hasta que el usuario presione "y".

    call nextClock

    ;mov word [portal_called_on_cicle], 0
    ;xchg bx, bx
    call reset_cicle_counters

    call sched_nextTask

    pushad
    call sched_taskNextClock
    popad

    shl ax, 3
    str bx
    cmp ax, bx
    je .fin
      mov word [sched_task_selector], ax
      jmp far [sched_task_offset]
  .fin:
  popad
iret

;; Rutina de atencion del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33
_isr33:
  pushad
  in al, 0x60
  cmp al, 0x15   ; 0x15  =  codigo de teclado IBM para la tecla "y"
  push eax     ; Parametro para print_NumTopRightEdge.
  jne .fin_caso_tecla_v
    call sched_isStopped
    cmp al, 0
    jne .unStop
      cmp word [debug_mode], 0x0DEB
      jne .activate_debug_mode
      ;deactivate_debug_mode:
        mov word [debug_mode], 0x0000
        call screen_hideDebugModeLegend
        jmp .fin_caso_tecla_v
      .activate_debug_mode:
        mov word [debug_mode], 0x0DEB
        call screen_showDebugModeLegend
        jmp .fin_caso_tecla_v
    .unStop:

      ; Se puede volver a dibujar todo lo previamente tapado
      call screen_restore
      call sched_unsetDebugStop
  .fin_caso_tecla_v:
  call print_NumTopRightEdge  ; Se escribe numero de tecla numerica en la esquina superior derecha de la pantalla.
  add esp, 4  ; Deshacemos el push.
  call pic_finish1
  popad
iret


;; Rutinas de atencion de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr137  ; usePortalGun
_isr137:
  pushad
  ; Se pasan parametros para llamar a usePortalGun_C
  push edx ; dword [ebp-16] ; withMorty
  push ecx ; dword [ebp-12] ; cross
  push ebx ; dword [ebp-8]  ; y
  push eax ; dword [ebp-4]  ; x
  call usePortalGun_C
  add esp, 16
  mov [tmp], eax
  popad
  mov eax, [tmp]
  ;xchg bx, bx
iret


global _isr138  ; iAmRick
_isr138:
  pushad
  ; El primer parametro que reciben las syscalls lo reciben por eax.
  ; Ahora se lo pasamos con un push (convencion C 32 bit) a la funcion que llamamos.
  push eax
  call IamRick_C
  add esp, 4
  popad
iret


global _isr139  ; whereIsMorty
_isr139:
  pushad
  mov ebx, eax
  add ebx, 4
  push tmp ; puntero a y
  push tmp2; puntero a x
  call whereIsMorty_C
  add esp, 8
  ;mov eax, [esp-8]
  ;mov [tmp], eax
  ;mov ebx, [esp-4]
  ;mov [tmp2], ebx
  popad
  mov eax, [tmp2]
  mov ebx, [tmp]
iret


;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:    dd 0x00000000
isrClock:     db '|/-\'
nextClock:
  pushad
  inc DWORD [isrNumber]
  mov ebx, [isrNumber]
  cmp ebx, 0x4
  jl .ok
    mov DWORD [isrNumber], 0x0
    mov ebx, 0
  .ok:
    add ebx, isrClock
    print_text_pm ebx, 1, 0x0f, 49, 79
  popad
ret
