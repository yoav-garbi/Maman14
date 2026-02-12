#include "prototypes.h"





static void trim_right(char *s) {
    size_t n;
    if (!s) return;
    n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t'))
        s[--n] = '\0';
}


int extract_macro_name_after_check(const char *line, char *out_name, size_t out_sz) {
	int n1;
	char directive[MAX_LINE_LENGTH];
	const char *p;

	(void)out_sz; /* name length already validated by your checker */

	p = skipWhiteSpace((char *)line);
	if (sscanf(p, "%s%n", directive, &n1) != 1) return 0; /* first token */
	if (strcmp(directive, "mcro") != 0) return 0;       /* not a macro line */

	if (check_macroOpenLine((char *)line) != 0) return 0;

	p += n1;
	if (sscanf(p, "%s", out_name) != 1) return 0;      /* macro name */
	return 1;
}


static int parse_mcro_open(const char *line, char *out_name, size_t out_sz) {
    int ok, n1;
    char dummy[MAX_LINE_LENGTH];
    const char *p;

    (void)out_sz;
    ok = check_macroOpenLine((char *)line);
    if (ok != 0) return 0;

    p = line;
    if (sscanf(p, "%s%n", dummy, &n1) != 1) return 0; /* "mcro" */
    p += n1;
    if (sscanf(p, "%s", out_name) != 1) return 0; /* macro name */
    return 1;
}

static int mcro_is_close(const char *line) {
	const char *p = skipWhiteSpace((char *)line);
	if (strncmp(p, "mcroend", 7) != 0)
		return 0;
	return (check_macroCloseLine((char *)line) == 0);
}

static const char *scan_label_prefix(const char *s, char *label, size_t label_sz) {
    const char *p = skipWhiteSpace((char *)s);
    const char *q = p;
    size_t len;

    while (*q && *q != ':' && !isspace((unsigned char)*q)) q++;
    if (*q != ':') return NULL;

    len = (size_t)(q - p) + 1; /* include ':' */
    if (label_sz) {
        if (len >= label_sz) len = label_sz - 1;
        memcpy(label, p, len);
        label[len] = '\0';
    }
    return q + 1;
}

static const char *scan_token(const char *s, char *buf, size_t buf_sz) {
    const char *p = skipWhiteSpace((char *)s);
    size_t i = 0;
    while (*p && !isspace((unsigned char)*p)) {
        if (i + 1 < buf_sz) buf[i++] = *p;
        p++;
    }
    if (buf_sz > 0) buf[i] = '\0';
    return p;
}

static macro *find_macro(const char *name) {
    int i;
    if (!macroArr || macroCounter <= 0) return NULL;
    for (i = 0; i < macroCounter; ++i) {
        if (macroArr[i] && macroArr[i]->name && strcmp(macroArr[i]->name, name) == 0)
            return macroArr[i];
    }
    return NULL;
}

static void emit_macro_body(FILE *out, const macro *m) {
    int i;
    if (!m) return;
    for (i = 0; i < m->lineAmount; ++i) {
        fputs(m->macroLines[i], out);
        fputc('\n', out);
    }
}

static int read_mcro_body(FILE *fp, const char *macroName, int *pLineCounter) {
    LineData lineBuf;
    int readLine;
    (void)pLineCounter;

    for (;;) {
        readLine = takeInLine(lineBuf.content, fp);
        if (readLine == EOF_only_line) {
            printf("\nUnexpected EOF before 'mcroend'. (File: \"%s\")\n\n", (*argvPointer)[fileCounter]);
            return ERROR;
        }
        if (readLine != 0) continue;

        trim_right(lineBuf.content);
        if (mcro_is_close(lineBuf.content)) break;

        if (addLineToMacro((char *)macroName, lineBuf.content) == ERROR)
            return ERROR;
    }
    return 0;
}

static int only_ws_or_comment(const char *p) {
    p = skipWhiteSpace((char *)p);
    return (*p == '\0' || *p == '\n' || *p == ';');
}

static int expand_macro(FILE *out, const macro *m, const char *opt_label) {
    int i, first_real = -1;
    if (!m) return 0;

    for (i = 0; i < m->lineAmount; ++i) {
        const char *s = skipWhiteSpace(m->macroLines[i]);
        if (*s != '\0' && *s != '\n' && *s != ';') { first_real = i; break; }
    }

    if (opt_label && first_real < 0) return 0;

    if (opt_label && first_real >= 0) {
        for (i = 0; i < first_real; ++i) { fputs(m->macroLines[i], out); fputc('\n', out); }
        fputs(opt_label, out); fputc(' ', out);
        fputs(m->macroLines[first_real], out); fputc('\n', out);
        for (i = first_real + 1; i < m->lineAmount; ++i) { fputs(m->macroLines[i], out); fputc('\n', out); }
        return 1;
    }

    emit_macro_body(out, m);
    return 1;
}


/* =================================================================================== */
int preAssemble(int index) {
	FILE *in_fp, *out_fp;
	char label[MAX_LABEL_LENGTH];
	int countError = 0;
	LineData currentLine;
	char *ptr, *after_label;
	int readLine;
	
    in_fp  = fileArr[index];
    out_fp = fileArr[amOffset + index];

    if (check_fileExistence(in_fp) == ERROR || check_fileExistence(out_fp) == ERROR)
        return ERROR;

    freeMacroArr();
    if (initializeMacroArr() == ERROR)
        return ERROR;

    readLine = takeInLine(currentLine.content, in_fp);
    while (readLine != EOF_only_line) {
        if (readLine != 0) {
            countError++;
            readLine = takeInLine(currentLine.content, in_fp);
            continue;
        }

		trim_right(currentLine.content);

		{
		char macroName[MAX_LABEL_LENGTH];
		char firstToken[MAX_LABEL_LENGTH];
		const char *p = skipWhiteSpace(currentLine.content);

		if (sscanf(p, "%s", firstToken) == 1 && strcmp(firstToken, "mcro") == 0 && parse_mcro_open(currentLine.content, macroName, sizeof(macroName))) {
			if (addMacro(macroName) == ERROR) {
			countError++;
		} else {
			if (read_mcro_body(in_fp, macroName, &lineCounter) == ERROR)
			countError++;
			}
				readLine = takeInLine(currentLine.content, in_fp);
				continue;
			}
		}

        ptr = currentLine.content;
        after_label = (char *)scan_label_prefix(ptr, label, sizeof(label));

        if (after_label != NULL) {
            char tok[MAX_LABEL_LENGTH];
            const char *after_tok;
            macro *m;

            after_label = (char *)skipWhiteSpace(after_label);
            after_tok = scan_token(after_label, tok, sizeof(tok));
            m = (tok[0] ? find_macro(tok) : NULL);

            if (m && only_ws_or_comment(after_tok)) {
                if (!expand_macro(out_fp, m, label)) {
                    fputs(currentLine.content, out_fp); fputc('\n', out_fp);
                }
            } else {
                fputs(currentLine.content, out_fp); fputc('\n', out_fp);
            }
        } else {
            char tok[MAX_LABEL_LENGTH];
            const char *after_tok;
            macro *m;

            after_tok = scan_token(currentLine.content, tok, sizeof(tok));
            m = (tok[0] ? find_macro(tok) : NULL);

            if (m && only_ws_or_comment(after_tok)) {
                (void)expand_macro(out_fp, m, NULL);
            } else {
                fputs(currentLine.content, out_fp); fputc('\n', out_fp);
            }
        }

        readLine = takeInLine(currentLine.content, in_fp);
    }

    return countError;
}
