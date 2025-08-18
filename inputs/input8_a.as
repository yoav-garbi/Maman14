; file input8_a.as - correct valid code 
.entry MAIN
.entry COUNT
.extern HELPER
.extern EXTSTR
.extern ARRX

mcro MC2
    clr r1
    add r1, r2
    prn r2
mcroend

mcro MC3
    mov #-5, r7
    not r7
mcroend

MAIN:   mov #5, COUNT
        lea ARRX[r0][r1], r3
        jsr HELPER
        prn COUNT
        MC2
        inc r4
        bne HELPER
        MC3
        stop

TXT:    .string "Hello"
COUNT:  .data 1,2,3,4,5,-128,127,511,-512
ARRA:   .mat [2][2] 10,20,30,40
