; input5_b.as - helper file to file input5_a.as

	; entry directive on label that was already entry-ed in another file (input5_a.as)
	entriedLabel: mov r3, r4
	.entry entriedLabel
	
	; label declared but no entry-ed into other file's scope
	someLabel1: mov r1, r2
