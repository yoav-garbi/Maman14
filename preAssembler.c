#include "prototypes.h"
#include <string.h>

void trim_right(char *s)
{
    size_t n;
    if (!s) return;
    n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t'))
        s[--n] = '\0';
}

int parse_mcro_open(const char *line, char *out_name, size_t out_sz, const char *fname_as)
{
    const char *p;
    char tok[MAX_LINE_LENGTH];
    int n1, chk;

    if (out_sz > 0) out_name[0] = '\0';

    p = skipWhiteSpace((char *)line);
    if (sscanf(p, "%s%n", tok, &n1) != 1) return 0;
    if (strncmp(tok, "mcro", 4) != 0) return 0;

    if (strcmp(tok, "mcro") != 0) {
        printf("\nMissing white space between 'mcro' and macro name. (Line %d, file: \"%s\")\n\n", lineCounter, fname_as);
        return -2;
    }

    chk = check_macroOpenLine((char *)line);

    p += n1;
    if (sscanf(p, "%s", out_name) != 1)
        return -1;

    return (chk == 0) ? 1 : -1;
}

int mcro_close_status(const char *line, const char *fname_as)
{
    const char *p;
    char tok[MAX_LINE_LENGTH];

    p = skipWhiteSpace((char *)line);
    if (sscanf(p, "%s", tok) != 1) return 0;

    if (strncmp(tok, "mcroend", 7) == 0 && strcmp(tok, "mcroend") != 0) {
        printf("\nText glued to 'mcroend' directive. (Line %d, file: \"%s\")\n\n", lineCounter, fname_as);
        return -1;
    }

    if (strcmp(tok, "mcroend") != 0) return 0;
    return (check_macroCloseLine((char *)line) == 0) ? 1 : 2;
}

const char *scan_label_prefix(const char *s, char *label, size_t label_sz)
{
    const char *p, *q;
    size_t len;

    p = skipWhiteSpace((char *)s);
    q = p;
    while (*q && *q != ':' && !isspace((unsigned char)*q)) q++;
    if (*q != ':') return NULL;

    len = (size_t)(q - p) + 1;
    if (label_sz) {
        if (len >= label_sz) len = label_sz - 1;
        memcpy(label, p, len);
        label[len] = '\0';
    }
    return q + 1;
}

const char *scan_token(const char *s, char *buf, size_t buf_sz)
{
    const char *p;
    size_t i = 0;

    p = skipWhiteSpace((char *)s);
    while (*p && !isspace((unsigned char)*p)) {
        if (i + 1 < buf_sz) buf[i++] = *p;
        p++;
    }
    if (buf_sz > 0) buf[i] = '\0';
    return p;
}

macro *find_macro(const char *name)
{
    int i;
    if (!macroArr || macroCounter <= 0) return NULL;
    for (i = 0; i < macroCounter; ++i)
        if (macroArr[i] && macroArr[i]->name && strcmp(macroArr[i]->name, name) == 0)
            return macroArr[i];
    return NULL;
}

void emit_macro_body(FILE *out, const macro *m)
{
    int i;
    if (!m) return;
    for (i = 0; i < m->lineAmount; ++i) {
        fputs(m->macroLines[i], out);
        fputc('\n', out);
    }
}

int read_mcro_body(FILE *fp, const char *macroName, int *pErr, const char *fname_as)
{
    LineData lineBuf;
    int readLine, st;

    for (;;) {
        readLine = takeInLine(lineBuf.content, fp);
        if (readLine == EOF_only_line) {
            printf("\nUnexpected EOF before 'mcroend'. (File: \"%s\")\n\n", fname_as);
            if (pErr) (*pErr)++;
            return ERROR;
        }
        if (readLine != 0)
            continue;

        trim_right(lineBuf.content);

        st = mcro_close_status(lineBuf.content, fname_as);
        if (st == 1) break;
        if (st == 2) { if (pErr) (*pErr)++; break; }
        if (st == -1) { if (pErr) (*pErr)++; }

        if (addLineToMacro((char *)macroName, lineBuf.content) == ERROR)
            return ERROR;
    }
    return 0;
}

int only_ws_or_comment(const char *p)
{
    p = skipWhiteSpace((char *)p);
    return (*p == '\0' || *p == '\n' || *p == ';');
}

int expand_macro(FILE *out, const macro *m, const char *opt_label)
{
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

int preAssemble(int index)
{
    FILE *in_fp, *out_fp;
    char label[MAX_LABEL_LENGTH];
    int countError = 0;
    LineData currentLine;
    char *after_label;
    int readLine;
    const char *fname_as;

    if (argvPointer && *argvPointer && (*argvPointer)[fileCounter+1])
        fname_as = (*argvPointer)[fileCounter+1];
    else
        fname_as = nameArr[fileCounter];

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
            int st = parse_mcro_open(currentLine.content, macroName, sizeof(macroName), fname_as);
            if (st == 1) {
                if (addMacro(macroName) == ERROR) {
                    countError++;
                } else {
                    if (read_mcro_body(in_fp, macroName, &countError, fname_as) == ERROR)
                        countError++;
                }
                readLine = takeInLine(currentLine.content, in_fp);
                continue;
            } else if (st == -1 || st == -2) {
                LineData tmp;
                int rl;
                countError++;
                for (;;) {
                    rl = takeInLine(tmp.content, in_fp);
                    if (rl == EOF_only_line) {
                        printf("\nUnexpected EOF before 'mcroend'. (File: \"%s\")\n\n", fname_as);
                        countError++;
                        break;
                    }
                    if (rl != 0) continue;
                    trim_right(tmp.content);
                    if (mcro_close_status(tmp.content, fname_as) != 0) break;
                }
                readLine = takeInLine(currentLine.content, in_fp);
                continue;
            }
        }

        after_label = (char *)scan_label_prefix(currentLine.content, label, sizeof(label));
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
