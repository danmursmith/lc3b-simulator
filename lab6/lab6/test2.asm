		.ORIG x3050
		LEA R2, NUMBER
		LDW R2, R2, #0
		LEA R1, SIX
		LDW R1, R1, #0
		AND R3, R3, #0
AGAIN 	ADD R3, R3, R2
		ADD R1, R1, #-1
		BRp AGAIN
		HALT
NUMBER 	.FILL x0007
SIX 	.FILL x0006
		.END