	.ORIG x3000
	ADD r3, r1, #1
	AND r2, r3, r3
	TRAP x25
	.END