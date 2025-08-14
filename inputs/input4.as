; input3.as - file with first pass errors = should cause errors

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
	
	; .data with no arguments
	.data
	
	; .data with number too large
	.data 130
	
	; .data with number too small
	.data -129
	
	; .data with non-number
	.data 1, a, 3
	
	; .data with fraction
	.data 1, 3.5, 2
	
	; .data with misplaced comma
	.data ,1,2
	
	; .data with misplaced comma
	.data 1,,2
	
	; .data with misplaced comma
	.data 1,2,
	
	; no space after .data directive
	.data1,2,3
	
	; extraneous text after .data line
	.data 1,2,3 extra
	
	; .mat with incorrect commas in dimensions
	.mat [2][3] 1,2,3,,4
	
	; .mat with non-integer dimensions
	.mat [2][1b] 1,2,3,4
	
	; .mat with text between directive and dimensions
	.mat XYZ [2][2] 1,2,3,4
	
	; .mat with text between height and width
	.mat [2] abc [2] 1,2,3,4
	
	; .mat with number too large
	.mat [1][1] 999
	
	; .mat with number too small
	.mat [1][1] -999
	
	; .mat with non-numeric value
	.mat [2][2] 1, x, 3, 4
	
	; .mat with too many values
	.mat [2][2] 1,2,3,4,5
	
	; extraneous text after .mat
	.mat [2][2] 1,2,3,4 extra_after_mat
	
	; .string with bad use of quotes
	.string "text
	
	; .string with bad use of quotes
	.string text"
	
	; .string with bad use of quotes
	.string text
	
	; no space after .string
	.string"abc"
	
	; extraneous text after .string
	.string "ok" extra
	
	; no space after .entry
	.entryLABEL
	
	; empty .entry
	.entry
	
	; extraneous text after .entry
	.entry SOME extra
	
	; no space after .extern
	.externLABEL
	
	; empty .extern
	.extern
	
	; extraneous text after .extern
	.extern X more
