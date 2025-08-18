; input7_b.as - helper to input7_a.as , valid code too, no errors
.entry EXTFN
.entry EXTDATA
.entry LENGTHB
.entry ARRB

EXTFN:      mov r2, r0
            dec r0
            rts

EXTDATA:    .data 5,-5
LENGTHB:    .data 6
ARRB:       .mat [3][1] 9,8,7
