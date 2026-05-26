;
; Functii SPM pentru scriere Flash pe ATmega328P
; Extras din zevero/avr_boot (C) ChaN, 2010
; https://github.com/zevero/avr_boot
;

.nolist
#include <avr/io.h>
.list
.text

#ifndef SPMCSR
#define SPMCSR SPMCR
#endif

;---------------------------------------------------------------------------
; Sterge o pagina Flash
; void flash_erase(DWORD flash_addr);
;   r25:r24:r23:r22 = adresa paginii

.global flash_erase
.func flash_erase
flash_erase:
    movw    ZL, r22

    ldi     r24, 0b00000011         ; PGERS | SPMEN
    sts     _SFR_MEM_ADDR(SPMCSR), r24
    spm

1:  lds     r24, _SFR_MEM_ADDR(SPMCSR)
    sbrc    r24, 0
    rjmp    1b

    ldi     r24, 0b00010001         ; RWWSRE | SPMEN
    sts     _SFR_MEM_ADDR(SPMCSR), r24
    spm

    ret
.endfunc


;---------------------------------------------------------------------------
; Scrie o pagina Flash
; void flash_write(DWORD flash_addr, const BYTE* data);
;   r25:r24:r23:r22 = adresa paginii
;   r21:r20         = pointer catre buffer (SPM_PAGESIZE bytes)

.global flash_write
.func flash_write
flash_write:
    push    r0
    push    r1

    movw    ZL, r22
    movw    XL, r20
    ldi     r25, lo8(SPM_PAGESIZE / 2)

1:  ld      r0, X+
    ld      r1, X+
    ldi     r24, 0b00000001         ; SPMEN
    sts     _SFR_MEM_ADDR(SPMCSR), r24
    spm
    adiw    ZL, 2
    dec     r25
    brne    1b

    movw    ZL, r22
    ldi     r24, 0b00000101         ; PGWRT | SPMEN
    sts     _SFR_MEM_ADDR(SPMCSR), r24
    spm

2:  lds     r24, _SFR_MEM_ADDR(SPMCSR)
    sbrc    r24, 0
    rjmp    2b

    ldi     r24, 0b00010001         ; RWWSRE | SPMEN
    sts     _SFR_MEM_ADDR(SPMCSR), r24
    spm

    pop     r1
    pop     r0
    ret
.endfunc
