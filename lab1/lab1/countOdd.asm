            .ORIG x3000
            LEA R0, START
            LEA R1, STORE
            LEA R2, COUNT
            LDW R2, R2, #0      ; decrement counter
            AND R3, R3, #0      ; odd counter
    LOOP    LDB R4, R0, #0
            AND R4, R4, #1
            BRz EVEN            ; odd if positive
            ADD R3, R3, #1
    EVEN    ADD R0, R0, #1
            ADD R2, R2, #-1
            BRp LOOP            ; done if #0
            STW R3, R1, #0
    START   .FILL x4000
    STORE   .FILL x4100
    COUNT   .FILL x0100
            .END
