; input3.as - file with macro errors = should cause errors

	; text after macro name
	mcro AAA				fd
	mov r3,r2
	mcroend
	
	; text after mcroend
	mcro BBB
	inc r3
	mcroend				    fdonvd46.,.
	
	; missing space after mcro
	mcroCCC
	mov r5,r4
	mcroend
	
	; missing space after mcroend
	mcro DDD
	inc r4
	mcroendSONSOPN
	
	; errors in the macro body- should not be detected here
	mcro FFF
	fakecommand r7
	mcroend
	
	
	
	
	AAA
	BBB
	CCC
	DDD
	FFF
