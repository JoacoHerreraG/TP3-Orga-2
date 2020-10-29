; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"
extern GDT_DESC
;extern EBP_INIT
;extern ESP_INIT
extern screen_backup_init
extern idt_init
extern IDT_DESC
extern pic_reset
extern pic_enable
extern mmu_initKernelDir
extern mmu_init
extern mmu_mapPage
extern mmu_unmapPage
extern mmu_initTaskDir
extern tss_init
extern sched_init
extern tss_user_tasks_init
extern tlbflush
global start



;; Saltear seccion de datos, pues no son c'odigo y no hay otra forma de hacerlo.
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db   'Iniciando kernel en Modo Real'
start_rm_len equ  $ - start_rm_msg

start_pm_msg db   'Iniciando kernel en Modo Protegido'
start_pm_len equ  $ - start_pm_msg



;;
;; Seccion de codigo.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
  ; Deshabilitar interrupciones del BIOS
  cli

  ; Cambiar modo de video a 80 X 50
  mov ax, 0003h
  int 10h ; set mode 03h
  xor bx, bx
  mov ax, 1112h
  int 10h ; load 8x8 font

  ; Imprimir mensaje de bienvenida
  print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0


  ; Habilitar A20
  ; El archivo a20.asm tiene instrucciones prearmadas de la catedra.
  ; 'A20_enable' habilita la memoria mas alla del 1 MB
  ; Se verifica con 'A20_check' si efectivamente
  ; la memoria expandida esta habilitada
  call A20_disable
  call A20_check
  call A20_enable
  call A20_check
  ;xchg bx,bx ; Punto de parada para depurar con bochs.

  ; Se carga el registro gdtr con la direcci'on de memoria
  ; de la tabla de segmentos globales (GDT),
  ; mediante instrucci'on lgdt
  ; esta direcci'on de memoria es GDT_DESC.
  lgdt [GDT_DESC]
  ;xchg bx,bx ; Punto de parada para depurar con bochs.
  ; Activar modo protegido (PE) en 1er bit de registro de control 0 (CR0)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax

  ; Saltar a modo protegido
  ;jmp CS_RING_0:modo_protegido
  jmp 0x40:modo_protegido   ; 0x08 << 3 = 0x40

BITS 32
modo_protegido:

  ; Establecer selectores de segmentos
  ;mov ax, DS_RING_0
  mov ax, 0x48  ; 0x09 << 3 = 0x48       Entrada de la gdt descriptor de segmento de datos de nivel 0.

  mov ds, ax		; ds = selector de segmento de datos apunta al descriptor de segmento de datos de nivel 0.
  mov es, ax		; es = selector de segmento de datos apunta al descriptor de segmento de datos de nivel 0.
  mov fs, ax		; fs = selector de segmento de datos apunta al descriptor de segmento de datos de nivel 0.
  mov gs, ax		; gs = selector de segmento de datos apunta al descriptor de segmento de datos de nivel 0.
  mov ss, ax		; ss = selector de segmento de pila  apunta al descriptor de segmento de datos de nivel 0.

  ; Establecer la base de la pila
  ;mov ebp, EBP_INIT
  mov ebp, 0x27000
  ;mov esp, ESP_INIT
  mov esp, 0x27000

  ; Imprimir mensaje de bienvenida
  ;print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0

  ; Inicializar pantalla
  ;call screen_init  ; Va a ser m'as prolijo llevar p. video a otro archivo...

      mov ax, 0x60  ; '12 << 3' = 0x0C << 3 = 0x60
      mov es, ax

      xor ecx, ecx
      .imprimir1LineaNegra:
        mov word [es:ecx], 0x0700 ;0:fondo negro;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 0x00A0
      jl .imprimir1LineaNegra

      .imprimir40LineasVerdes:
        mov word [es:ecx], 0x2700 ;2:fondo verde;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 0x19A0
      jl .imprimir40LineasVerdes

      .imprimir9LineasNegras:
        mov word [es:ecx], 0x0700 ;0:fondo negro;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 0x1F3F
      jl .imprimir9LineasNegras

      mov ecx, 7046
      .imprimir6ChrAzul1:
        mov word [es:ecx], 0x1700 ;1:fondo azul;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7063
      jl .imprimir6ChrAzul1

      mov ecx, 7176
      .imprimir6ChrRojo1:
        mov word [es:ecx], 0x4700 ;4:fondo rojo;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7193
      jl .imprimir6ChrRojo1


      mov ecx, 7206
      .imprimir6ChrAzul2:
        mov word [es:ecx], 0x1700 ;1:fondo azul;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7223
      jl .imprimir6ChrAzul2

      mov ecx, 7336
      .imprimir6ChrRojo2:
        mov word [es:ecx], 0x4700 ;4:fondo rojo;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7353
      jl .imprimir6ChrRojo2


      mov ecx, 7366
      .imprimir6ChrAzul3:
        mov word [es:ecx], 0x1700 ;1:fondo azul;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7383
      jl .imprimir6ChrAzul3

      mov ecx, 7496
      .imprimir6ChrRojo3:
        mov word [es:ecx], 0x4700 ;4:fondo rojo;7:texto blanco;00:sin caracter
        inc ecx
        inc ecx
        cmp ecx, 7513
      jl .imprimir6ChrRojo3

      ; Se restablece el selector de segmento 'es' para que todos los
      ; selectores de segmentos esten iguales antes de llamar una funcion C.
      mov ax, fs
      mov es, ax

  ; Inicializar el manejador de memoria (MMU)
  ; Se inicia manejador de memoria que
  ; va a servir para mapear memoria para cada tareas.
  ; ( Basicamente se inicia el puntero a proxima_pagina_libre
  ; en el "area libre del kernel",
  ; que contiene paginas libres que se van a ir utilizando para
  ; los mapas de memoria de las tareas. )
  call mmu_init

  ; Se genera y se carga el directorio y la tabla de paginas
  ; para mapear con mapeo identidad los primeros 4MB de la memoria.
  call mmu_initKernelDir
  ; mmu_initKernelDir devuelve
  ; sobre eax el puntero al directorio de paginas del kernel.

  ; Sobre el registro de control 3 (CR3)
  ; es posible escribir la dirección del directorio de paginasdirectamente,
  ; pues los primeros 20 bit ya corresponden con
  ; los 20 bit mas significativos del directorio de paginas y
  ; los 12 BIT RESTANTES para los propositos de este trabajo
  ; quedan TODOS EN 0 incluyendo los bit 4 (PCD) y 3 (PWT).
  ; Vienen en 0 pues toda pagina, el directorio y las tablas
  ; est'an siempre alineadas a 4KB.
  mov cr3, eax
  ;xchg bx,bx ; Punto de parada para depurar con bochs.



  ; Habilitar paginacion
  ; Se trae el valor del registro de control 0 (CR0).
  mov eax, cr0
  ; Se fija el bit 32 en 1, lo que va a activar paginaci'on.
  or eax, 0x80000000
  ; Se mueve el valor modificado de cr0 sobre cr0, activando paginacion.
  mov cr0, eax




  ; Se va a escribir en pantalla nuevamente.

  ; Se sabe que la direccion "f'isica" 0xB8000,
  ; donde comienza la parte del BIOS que controla
  ; la grilla de caracteres de la pantalla (en modo de video a 80 X 50)
  ; es accesible a traves de la entrada 0 del directorio de paginas,
  ; y la entrada B8 de la unica tabla de pagina definida hasta el momento.
  ; Esto se corresponde con el mapeo identidad (o identity mapping) aplicado.

  ; Se escriben las libretas de los integrantes en la ultima linea
  ; Caracter de pantalla x 2 = 80 x 49 x 2 = 0x1EA0
  ; Posici'on de memoria = 0xB8000 + 0x1EA0 = 0xB9EA0
  ; Entrada de directorio = 0x000
  ; Entrada de tabla = 0x0B9
  ; Desplazamiento dentro de pagina (offset) = 0xEA0
  mov ecx, 0xB9EA0
  mov word [ecx], 0x0733 ; 0: fondo negro; 7: texto blanco; 33: caracter '3'
  inc ecx
  inc ecx
  mov word [ecx], 0x0735 ; 0: fondo negro; 7: texto blanco; 33: caracter '5'
  inc ecx
  inc ecx
  mov word [ecx], 0x0736 ; 0: fondo negro; 7: texto blanco; 33: caracter '6'
  inc ecx
  inc ecx
  mov word [ecx], 0x072F ; 0: fondo negro; 7: texto blanco; 33: caracter '/'
  inc ecx
  inc ecx
  mov word [ecx], 0x0730 ; 0: fondo negro; 7: texto blanco; 33: caracter '0'
  inc ecx
  inc ecx
  mov word [ecx], 0x0738 ; 0: fondo negro; 7: texto blanco; 33: caracter '8'
  inc ecx
  inc ecx
  inc ecx
  inc ecx
  mov word [ecx], 0x0732 ; 0: fondo negro; 7: texto blanco; 33: caracter '2'
  inc ecx
  inc ecx
  mov word [ecx], 0x0737 ; 0: fondo negro; 7: texto blanco; 33: caracter '7'
  inc ecx
  inc ecx
  mov word [ecx], 0x0733 ; 0: fondo negro; 7: texto blanco; 33: caracter '3'
  inc ecx
  inc ecx
  mov word [ecx], 0x072F ; 0: fondo negro; 7: texto blanco; 33: caracter '/'
  inc ecx
  inc ecx
  mov word [ecx], 0x0731 ; 0: fondo negro; 7: texto blanco; 33: caracter '1'
  inc ecx
  inc ecx
  mov word [ecx], 0x0737 ; 0: fondo negro; 7: texto blanco; 33: caracter '7'


  ; Se reervan 2 paginas para salvaguardar pantalla antes de la pantalla de debug.
  call screen_backup_init

  ; Inicializar tss

  ; Inicializar tss de la tarea Idle

  ; Inicializar el scheduler
  call sched_init

  ; Inicializar la IDT en memoria
  call idt_init

  ; Cargar IDT
  lidt [IDT_DESC]

  ; Configurar controlador de interrupciones
  call pic_reset
  call pic_enable

  ; Cargar tarea inicial

  call tss_init
  call tss_user_tasks_init


   ; El registro de tarea (o 'task register' o 'tr')
   ; no puede ser cargado directamente, y
   ; hay que intermediar con otro registro como por ejemplo 'ax'.
  mov ax, 0x68 ; 13 = 0x0D  ; 0x0D << 3 = 0x68  ; Entrada 13 de la gdt tiene el descriptor de la tarea inicial.
  ltr ax
  ;xchg bx, bx

  ; Habilitar interrupciones
  sti

            ; Pruebas mmu_mapPage

            ; void mmu_mapPage(uint32_t cr3, uint32_t virtual, uint32_t phy, uint32_t attrs);
            ; los atributos provienen de los flags del cr3 (pag 114 del manual)

            ;push 2      ; 2: Escritura ; Demas atributos en 0.
            ;push 0x00400000 ; Direccion fisica
            ;push 0x0050E000 ; Direccion virtual
            ;mov eax, cr3
            ;push eax    ; cr3
            ;xchg bx, bx
            ;call mmu_mapPage
            ;xchg bx, bx
            ;add esp, 4*4  ; cantidad de parametros pusheados * tam de cada parametro en bytes
            ;xchg bx, bx
            ; Prueba de escritura en nueva direccion virtual.
            ;xchg bx, bx
            ;mov BYTE [0x0050E027], 0x1
            ;xchg bx, bx

            ;push 2      ; 2: Escritura ; Demas atributos en 0.
            ;push 0x00404000 ; Direccion fisica
            ;push 0x0050D000 ; Direccion virtual
            ;mov eax, cr3
            ;push eax    ; cr3
            ;xchg bx, bx
            ;call mmu_mapPage
            ;xchg bx, bx
            ;add esp, 4*4  ; cantidad de parametros pusheados * tam de cada parametro en bytes
            ;xchg bx, bx
            ; Prueba de escritura en nueva direccion virtual.
            ;xchg bx, bx
            ;mov BYTE [0x0050d008], 0x1
            ;xchg bx, bx

            ; Pruebas mmu_unmapPage

            ;push 0x0050E000 ; Direccion virtual
            ;mov eax, cr3
            ;push eax    ; cr3
            ;xchg bx, bx
            ;call mmu_unmapPage
            ;xchg bx, bx
            ;add esp, 2*4  ; cantidad de parametros pusheados * tam de cada parametro en bytes
            ;xchg bx, bx

            ;push 0x0050D000 ; Direccion virtual
            ;mov eax, cr3
            ;push eax    ; cr3
            ;xchg bx, bx
            ;call mmu_unmapPage
            ;xchg bx, bx
            ;add esp, 2*4  ; cantidad de parametros pusheados * tam de cada parametro en bytes
            ;xchg bx, bx

            ; Pruebas mmu_initTaskDir
            ;xchg bx, bx
            ;push 0x00400000 ; Direccion fisica (en mundo Cronenberg) de 1 pagina de codigo seguida de 1 pagina de pila de una tarea.
            ;push 0x08000000 ; Direccion virtual de 1 pagina de codigo seguida de 1 pagina de pila de una tarea.
            ;xchg bx, bx
            ;call mmu_initTaskDir
            ;xchg bx, bx
            ;add esp, 2*4  ; cantidad de parametros pusheados * tam de cada parametro en bytes
            ;xchg bx, bx
            ;mov cr3, eax
            ;xchg bx, bx
  ; Prueba de dividir por 0 para generar interrupci'on.
  ;xor eax, eax
  ;idiv eax   ; Prueba de dividir por 0 para generar interrupci'on.



  ; Saltar a la primera tarea: Tarea 'inutil' o tarea 'Idle'.
  jmp 0x70:0  ; 14 = 0x0E  ; 0x0E << 3 = 0x70  ; Entrada 14 de la gdt tiene el descriptor de la tarea 'que no hace nada' (o tarea 'idle', o tarea 'inutil').

  ; Ciclar infinitamente (por si algo sale mal...)
  mov eax, 0xFFFF
  mov ebx, 0xFFFF
  mov ecx, 0xFFFF
  mov edx, 0xFFFF
  jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
