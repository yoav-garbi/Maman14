#include "prototypes.h"





static void rstrip(char *s) {
    size_t n;
    if (!s) return;
    n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t'))
        s[--n] = '\0';
}

static int is_empty_or_comment(const char *s) {
    const char *p = skipWhiteSpace((char *)s);
    return (*p == '\0' || *p == '\n' || *p == ';');
}

static int extract_macro_name_after_check(const char *line, char *out_name, size_t out_sz) {
    int ok, n1;
    char dummy[MAX_LINE_LENGTH];
    const char *p;

    (void)out_sz; /* name length already validated by your checker */

    ok = check_macroOpenLine((char *)line);
    if (ok != 0) return 0;

    p = line;
    if (sscanf(p, "%s%n", dummy, &n1) != 1) return 0; /* "mcro" */
    p += n1;
    if (sscanf(p, "%s", out_name) != 1) return 0;     /* macro name */
    return 1;
}

static int is_mcro_close(const char *line) {
    return (check_macroCloseLine((char *)line) == 0);
}

static const char *leading_label(const char *s, char *label, size_t label_sz) {
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

static const char *first_token(const char *s, char *buf, size_t buf_sz) {
    const char *p = skipWhiteSpace((char *)s);
    size_t i = 0;
    while (*p && !isspace((unsigned char)*p)) {
        if (i + 1 < buf_sz) buf[i++] = *p;
        p++;
    }
    if (buf_sz > 0) buf[i] = '\0';
    return p;
}

static macro *find_macro_by_name(const char *name) {
    int i;
    if (!macroArr || macroCounter <= 0) return NULL;
    for (i = 0; i < macroCounter; ++i) {
        if (macroArr[i] && macroArr[i]->name && strcmp(macroArr[i]->name, name) == 0)
            return macroArr[i];
    }
    return NULL;
}

static void write_macro_body(FILE *out, const macro *m) {
    int i;
    if (!m) return;
    for (i = 0; i < m->lineAmount; ++i) {
        fputs(m->macroLines[i], out);
        fputc('\n', out);
    }
}

static int collect_macro_block(FILE *fp, const char *macroName, int *pLineCounter) {
    LineData lineBuf;
    int readLine;

    for (;;) {
        readLine = takeInLine(lineBuf.content, fp);
        if (readLine == EOF_only_line) {
            printf("\nUnexpected EOF before 'mcroend'. (File: \"%s\")\n\n", (*argvPointer)[fileCounter]);
            return ERROR;
        }
        (*pLineCounter)++;
        if (readLine != 0) continue;

        rstrip(lineBuf.content);
        if (is_mcro_close(lineBuf.content)) break;

        if (addLineToMacro((char *)macroName, lineBuf.content) == ERROR)
            return ERROR;
    }
    return 0;
}

static int is_ws_or_comment_rest(const char *p) {
    p = skipWhiteSpace((char *)p);
    return (*p == '\0' || *p == '\n' || *p == ';');
}

static int expand_macro_with_optional_label(FILE *out, const macro *m, const char *opt_label) {
    int i, first_real = -1;
    if (!m) return 0;

    for (i = 0; i < m->lineAmount; ++i) {
        const char *s = skipWhiteSpace(m->macroLines[i]);
        if (*s != '\0' && *s != '\n' && *s != ';') { first_real = i; break; }
    }

    if (opt_label && first_real < 0) {
        return 0;
    }

    if (opt_label && first_real >= 0) {
        for (i = 0; i < first_real; ++i) { fputs(m->macroLines[i], out); fputc('\n', out); }
        fputs(opt_label, out);
        fputc(' ', out);
        fputs(m->macroLines[first_real], out);
        fputc('\n', out);
        for (i = first_real + 1; i < m->lineAmount; ++i) { fputs(m->macroLines[i], out); fputc('\n', out); }
        return 1;
    }

    write_macro_body(out, m);
    return 1;
}


/* ========================================================================================== */
int preAssemble(int index) {

    FILE *fp;
    FILE *output;
    char label[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    LineData currentLine;
    char *ptr, *nextPtr;
    int readLine;
    
    fileCounter = index;
    lineCounter = 0;
	fp = fileArr[index];
	output = fileArr[index + amOffset];

    readLine = takeInLine(currentLine.content, fp);
    while (readLine != EOF_only_line) {
        lineNumber = lineCounter;

        if (readLine != 0) {
            countError++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        rstrip(currentLine.content);

        {
            char macroName[MAX_LABEL_LENGTH];
            if (extract_macro_name_after_check(currentLine.content, macroName, sizeof(macroName))) {
                if (addMacro(macroName) == ERROR) {
                    countError++;
                } else {
                    if (collect_macro_block(fp, macroName, &lineCounter) == ERROR)
                        countError++;
                }
                readLine = takeInLine(currentLine.content, fp);
                continue;
            }
        }

        if (is_empty_or_comment(currentLine.content)) {
            fputs(currentLine.content, output);
            fputc('\n', output);
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        ptr = currentLine.content;
        nextPtr = (char *)leading_label(ptr, label, sizeof(label));

        if (nextPtr != NULL) {
            char tok[MAX_LABEL_LENGTH];
            const char *after_tok;
            macro *mm;

            nextPtr = (char *)skipWhiteSpace(nextPtr);
            after_tok = first_token(nextPtr, tok, sizeof(tok));
            mm = (tok[0] ? find_macro_by_name(tok) : NULL);

            if (mm && is_ws_or_comment_rest(after_tok)) {
                if (!expand_macro_with_optional_label(output, mm, label)) {
                    fputs(currentLine.content, output); fputc('\n', output);
                }
            } else {
                fputs(currentLine.content, output);
                fputc('\n', output);
            }
        } else {
            char tok[MAX_LABEL_LENGTH];
            const char *after_tok;
            macro *mm;

            after_tok = first_token(currentLine.content, tok, sizeof(tok));
            mm = (tok[0] ? find_macro_by_name(tok) : NULL);

            if (mm && is_ws_or_comment_rest(after_tok)) {
                (void)expand_macro_with_optional_label(output, mm, NULL);
            } else {
                fputs(currentLine.content, output);
                fputc('\n', output);
            }
        }

        readLine = takeInLine(currentLine.content, fp);
    }

    return countError;
}
