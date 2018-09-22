    .ORIG x3000
    ;bits[15:8]
    LEA R1, F
    LDW R2, R1, #0
    AND R1, R0, R2
    RSHFA R1, R1, #8
    ;bits[7:0]
    LEA R3, S
    LDB R3, R3, #0
    AND R3, R0, R3
    LSHF R3, R3, #8
    RSHFA R3, R3, #8

    ;add together and store
    LEA R2, Re
    LDW R2, R2, #0
    ADD R1, R1, R3
    STW R1, R2, #0

    HALT
Re   .FILL x3050
F   .FILL xFF00
S   .FILL x00FF
    .END