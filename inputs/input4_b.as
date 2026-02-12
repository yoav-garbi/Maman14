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

	; .entry and .extern directives on the same label in the same file
	.extern HELLO
	.entry HELLO
