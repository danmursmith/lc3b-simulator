	.ORIG x3000
	AND R0, R0, #0
	ADD R0, R0, #5
	LEA R7, B
	JMP R7
	ADD R0, R0, #5
	HALT
B	HALT 
	.END
