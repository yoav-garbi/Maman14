; file input7_a.as - correct valid code 
.entry START
.extern EXTFN
.extern EXTDATA
.extern LENGTHB
.extern ARRB

mcro MC1
    inc r2
    prn #127
    sub r2, r3
mcroend

START:  lea ARRA[r1][r0], r5
        mov #127, r5
        add r5, r2
        mov ARRA[r0][r1], EXTDATA
        cmp LENGTHA, LENGTHB
        bne EXTFN
        jsr EXTFN
        mov ARRB[r2][r1], r0
        MC1
        not r6
        red r3
        prn #-128
        stop

STRA:       .string "Hi!"
LENGTHA:    .data 0,-128,127,511,-512
ARRA:       .mat [2][3] 1,2,3,4,5,6
