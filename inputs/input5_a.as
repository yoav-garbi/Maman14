; input5_a.as - file with second pass errors = should cause errors. needs helper file input5_b.as to cause some errors

	; command argument is a non-existant label
	mov fakeLabel, r2
	
	; entry directive on a label that is not defined localy in this file
	.entry fakeLabel
	
	; entry directive on label that was already entry-ed in another file (input5_b.as)
	entriedLabel: mov r1, r2
	.entry entriedLabel
	
	; extern directive on a label that was not entry-ed in any other file
	.extern someLabel1
	
	; extern directive on a label that was not declared in any other file
	.extern someLabel2
