; avOS - http://avos.sourceforge.net
; Copyright (c) 2003 by Jimmy Moore
;
; All files in this archive are subject to the GNU General Public License.
; See the file COPYING in the source tree root for full license agreement.
; This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
; KIND, either express of implied.
;
;
; A general purpose serial prog
;
; Date:     01/01/2004
; Author:   By DoggerMoore
;
        bits 32

        section .text

        jmp main

%include "linux.inc"

        SERIAL_SETUP equ B115200 | CS8 | CLOCAL | CREAD

	    section .text
main:
        pop eax
        pop eax
        mov esi, esp
        cmp dword [esi], 0
         je near noargs
    
        sc2 arg_findone, esi, key00
        mov dword [convcr], eax
    
        ;; TODO make buffer sizes command line...
        ;; TODO make uart params command line...
     
noargs:


        sc1 printf, msg00
    
        sc2 tcgetattr, STDIN, stdin_termios
        and dword [stdin_termios + TERM_LFLAG], ~ICANON
        mov byte [stdin_termios + TERM_CC + VTIME], 0
        mov byte [stdin_termios + TERM_CC + VMIN], 1
        sc3 tcsetattr, STDIN, 0, stdin_termios

        sc1 printf, msg01
        call serial_open

mainloop:
        mov word [pollfor], POLLIN | POLLOUT
        mov word [pollres], 0
        sc3 poll, pollfd, 1, 0              ;; Check if its ready for read?
        cmp eax, 0
         jl near erra
;; Check it anyways...
        test word [pollres], POLLERR
         jnz near erra
        test word [pollres], POLLHUP
         jnz near erra         

        test word [pollres], POLLOUT
         jz near notReadyOUT
         
        cmp dword [outStream], 1
         je near notReadyOUT        
;; ---------------------------------------- ;; Write stuff
        mov word [pollINres], 0
        sc3 poll, pollIN, 1, 0              ;; Check if STDIN ready for read?
        cmp eax, 0
         jl near erra
        test word [pollINres], POLLERR
         jnz near erra
        test word [pollINres], POLLHUP
         jnz near erra
        test word [pollINres], POLLIN
         jz near notReadyOUT

        sc3 read, STDIN, inbuff, dword [outbuffsize]   ;; Read a byte from STDIN
        cmp eax, 0
         jg okread
        mov dword [outStream], 1
okread:
        cmp dword [convcr], 1
         jne notcr
        cmp byte [inbuff], 0ah
         jne notcr
        mov byte [inbuff], 0dh          ;; Convert to 0dh
notcr:
        
        add dword [bytesOUT], eax
        sc3 write, dword [fd], inbuff, eax

        sc1 usleep, 200                 ;; Wait a bit...        
        jmp doneOUT
notReadyOUT:
        and word [pollres], ~POLLOUT
doneOUT:
        test word [pollres], POLLIN
         jz notReadyIN
;; ---------------------------------------- ;; Read stuff
        sc3 read, dword [fd], buff, dword [inbuffsize]
        cmp eax, 0
         jl near erra
         je nodat
         
        sc3 write, STDOUT, buff, eax          ;; Echo to STDOUT as well
        add dword [bytesIN], eax
nodat:

notReadyIN:

        test word [pollres], POLLOUT | POLLIN
         jz neitherReady

        inc dword [num]
        jmp mainloop
        
neitherReady:
        sc1 tcdrain, dword [fd]
        sc1 usleep, 100
        jmp mainloop
        
erra:   
        sc1 perror, errtt
        sc1 tcdrain, dword [fd]
        sc1 close, dword [fd]    
        sc1 exit, 0

    
; ------------------------------------------------------------------------------
; serial open
; ------------------------------------------------------------------------------
serial_open:
        sc2 open, serialFile, O_RDWR | O_NOCTTY
        cmp eax, -1
         jne noe1
        sc1 perror, err00
        sc1 exit, 0
        ret
noe1:   mov dword [fd], eax
        mov dword [pollfd], eax
        ;; Opened the serial port, now lets config it...
    
        sc2 tcgetattr, dword [fd], serial_termios

        sc7 printf, msg03, dword [serial_termios], dword [serial_termios + 4], dword [serial_termios + 8], dword [serial_termios + 12], dword [serial_termios + 33 + 16], dword [serial_termios + 33 + 20]

        mov dword [serial_termios + TERM_CFLAG], SERIAL_SETUP
        mov dword [serial_termios + TERM_IFLAG], IGNPAR
        mov dword [serial_termios + TERM_OFLAG], 0
        mov dword [serial_termios + TERM_LFLAG], 0
    
        mov byte [serial_termios + TERM_CC + VTIME], 0
        mov byte [serial_termios + TERM_CC + VMIN], 1

        sc2 tcflush, dword [fd], TCIFLUSH
    
        sc3 tcsetattr, dword [fd], TCSANOW, serial_termios
        cmp eax, -1
         jg noe3
        sc1 perror, err02
        ret     
noe3:
        mov dword [flags], TIOCM_DTR
        sc3 ioctl, dword [fd], TIOCMBIS, flags          ;; Set DTR
        cmp eax, 0
         jge noe4
         sc1 perror, errff
noe4:
        mov dword [flags], TIOCM_RTS
        sc3 ioctl, dword [fd], TIOCMBIC, flags          ;; Clear RTS    
        cmp eax, 0
         jge noe5 
        sc1 perror, errff
noe5:

        sc2 tcgetattr, dword [fd], serial_termios
        sc7 printf, msg03, dword [serial_termios], dword [serial_termios + 4], dword [serial_termios + 8], dword [serial_termios + 12], dword [serial_termios + 33 + 16], dword [serial_termios + 33 + 20]
        ret

; ------------------------------------------------------------------------------
; Get an arg... (->args, ->key, ->default)
; ------------------------------------------------------------------------------
arg_getone:
        push ebp
        mov ebp, esp
        push esi
        mov esi, dword [ebp + 8]
loopfk: cmp dword [esi], 0
         je nokf
        sc2 strcmp, [esi], [ebp + 12]           ; Is this our key?
        cmp eax, 0
         je foundk
        add esi, 4                              ; Lets move on an arg
        jmp loopfk                              ; Try again...
foundk:
        mov eax, dword [esi + 4]                ; Get next arg
        cmp eax, 0
         je nokf
        mov edi, esi
        add edi, 8
loopde: mov ebx, dword [edi]
        mov dword [esi], ebx
        cmp ebx, 0
         je donei
        add edi, 4
        add esi, 4
        jmp loopde
donei:
        pop esi
        pop ebp
        ret
            
nokf:   mov eax, dword [ebp + 16]
        pop esi
        pop ebp
        ret
        
; ------------------------------------------------------------------------------
; Find an arg... (->args, ->key)
; ------------------------------------------------------------------------------
arg_findone:
        push ebp
        mov ebp, esp
        push esi
        mov esi, dword [ebp + 8]
moopfk: cmp dword [esi], 0
         je mokf
        sc2 strcmp, [esi], [ebp + 12]           ; Is this our key?
        cmp eax, 0
         je moundk
        add esi, 4                              ; Lets move on an arg
        jmp moopfk                              ; Try again...
moundk:
        pop esi
        pop ebp
        mov eax, 1
        ret
            
mokf:   pop esi
        pop ebp
        xor eax, eax
        ret
            
; ==============================================================================
	section .data
key00:  db '--cr', 0
    
err00:  db 'Error opening serial device!', 0dh, 0ah, 0
err01:  db 'Error getting serial settings!', 0dh, 0ah, 0
err02:  db 'Error setting serial settings!', 0dh, 0ah, 0
errf2:  db 'Error setting blocking!', 0dh, 0ah, 0    
errff:  db 'Error!', 0
errtt:  db 'Error in read', 0

msg00:  db 'Serial tester v1.00', 0dh, 0ah, 0
msg01:  db 'Opening serial device...', 0dh, 0ah, 0
msg02:  db 'Setting CTS + DTR...', 0dh, 0ah, 0
msg03:  db '[Serial info]', 0dh, 0ah
        db ' Input mode:   %08x', 0dh, 0ah
        db ' Output mode:  %08x', 0dh, 0ah
        db ' Control mode: %08x', 0dh, 0ah
        db ' Local mode:   %08x', 0dh, 0ah
        db ' Input speed:  %08x', 0dh, 0ah
        db ' Output speed: %08x', 0dh, 0ah, 0
msg04:  db 'Getting byte %d [%08x]...', 0dh, 0ah, 0
msg10:  db 'Sent %d [%08x]...', 0dh, 0ah, 0
msg99:  db 'Recieved byte %02x', 0dh, 0ah, 0
msgff:  db 'Waiting...', 0dh, 0ah, 0

serialFile:     db '/dev/ttyS0', 0
fd:             dd 0
flags:          dd 0
num:            dd 0
bytesIN:        dd 0
bytesOUT:       dd 0

TERM_IFLAG      equ 0
TERM_OFLAG      equ 4
TERM_CFLAG      equ 8
TERM_LFLAG      equ 12
TERM_CC         equ 17

serial_termios:     dd 0    ;; input mode   iflag
                    dd 0    ;; output mode  oflag
                    dd 0    ;; control mode cflag
                    dd 0    ;; local mode   lflag
                    db 0    ;; line disc    c_line
                    dd 0,0,0,0,0,0,0,0  ;; control chars
                    dd 0    ;; input speed
                    dd 0    ;; output speed

stdin_termios:      dd 0
                    dd 0
                    dd 0
                    dd 0
                    db 0
                    dd 0,0,0,0,0,0,0,0
                    dd 0
                    dd 0
                    
pollfd:             dd 0
pollfor:            dw POLLIN | POLLOUT
pollres:            dw 0

outStream:          dd 0

pollIN:             dd STDIN
pollINfor:          dw POLLIN
pollINres:          dw 0

convcr:             dd 0
inbuffsize:         dd 16
outbuffsize:        dd 16

	section .bss

buff:   resb 4096
inbuff: resb 4096
tbuff:  resb 4096
