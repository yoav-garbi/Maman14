; input8_b.as - helper to input8_a.as , valid code too, no errors
.entry HELPER
.entry EXTSTR
.entry ARRX

HELPER: sub r2, r5
        red r6
        rts

EXTSTR: .string "World"
ARRX:   .mat [3][2] 7,8,9,10,11,12
