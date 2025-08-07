#ifndef CONSTANTS_H
#define CONSTANTS_H

/* --- Length Barriers --- */
#define WORD_LENGTH 12
#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 31
#define ASSEMBLED_LINE_LENGTH 24
#define NUM_OF_OPCODES 16
#define MAX_TOTAL_ADDRESSES 256

/* --- Macro Marks --- */
#define MACRO_START_MARK "mcro"
#define MACRO_END_MARK "mcroend"

/* --- Directive Marks --- */
#define DATA_MARK ".data"
#define STRING_MARK ".string"
#define ENTRY_MARK ".entry"
#define EXTERN_MARK ".extern"
#define MAT_MARK ".mat"

/* --- File Extensions --- */
#define ORIGINAL_EXT ".as"
#define AFTER_PREASSEMBLER_EXT ".am"
#define AFTER_SECOND_PASS_EXT ".ob"
#define EXTERNAL_EXT ".ext"
#define ENTRY_EXT ".ent"

/* --- General Assembly Settings --- */
#define IC_INIT_VALUE 100
#define DC_INIT_VALUE 0

#endif /* CONSTANTS_H */

