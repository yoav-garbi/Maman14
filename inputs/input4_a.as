; input4_a.as - file with first pass errors = should cause errors

	; extraneous text before line
	sjnjn mov r1, r2
	
	; illegal label name- too long (more than 30 chars)
	ThisLabelNameIsWayTooLongForRules123: mov r1, r2
	
	; illegal label name- empty (only ":", no name)
	: mov r1, r2
	
	; illegal label name- contains non-alnum character
	bad#name: mov r1, r2
	
	; illegal label name- starts with a non-letter
	1notstartletter: mov r1, r2
	
	; illegal label name- same as register
	r3: mov r1, r2
	
	; illegal label name- same as opcode
	mov: r1, r2
	
	; duplicate label definition
	dup: mov r1, r2
	dup: add r1, r2
	
	; missing space after label definition
	tight:mov r1, r2
	
	; empty line after label definition
	onlylbl:
	
	; label attached to .entry
	lab_entry: .entry SOME
	
	; label attached to .extern
	lab_extern: .extern OUTTHERE
	
	; two label definitions in one line
	two1: two2: mov r1, r2
	
	; non-existant command
	muv r1, r2
	
	; no space after opcode
	mov#5, r1
	
	; too many operands for command
	clr r1, r2
	
	; not enough operands for command
	add r1
	
	; invalid operand type for command
	clr #0
	
	; incorrect comma usage in command
	mov r1,, r2
	
	; incorrect comma usage in command
	mov r1 r2
	
	; mommand with non-existent register
	mov r8, r1
	
	; command with matrix using non-existent register
	mov MAT[r8][r1], r0
	
	; command uses non-matrix label as matrix
	DATA_LBL: .data 1,2,3
	mov DATA_LBL[r1][r2], r0
	
	; extraneous text after command
	stop extra_text
	
	
