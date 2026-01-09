#include "prototypes.h"

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
    const char *p = skipWhiteSpace((char *)line);
    const char *after_mcro;
    size_t i;

    if (out_sz > 0) out_name[0] = '\0';

    if (strncmp(p, "mcro", 4) != 0) return 0;

    if (p[4] != ' ' && p[4] != '\t') {
        printf("\nMissing white space between 'mcro' and macro name. (Line %d, file: \"%s\")\n\n",
               lineCounter, fname_as);
        return -2;
    }

    after_mcro = skipWhiteSpace((char *)(p + 4));
    if (*after_mcro == '\0' || *after_mcro == '\n' || *after_mcro == ';') {
        printf("\nMissing macro name. (Line %d, file: \"%s\")\n\n", lineCounter, fname_as);
        return -1;
    }

    i = 0;
    while (after_mcro[i] && !isspace((unsigned char)after_mcro[i])) {
        if (i + 1 < out_sz) out_name[i] = after_mcro[i];
        i++;
    }
    if (out_sz > 0) out_name[(i < out_sz ? i : out_sz-1)] = '\0';
    after_mcro += i;

    after_mcro = skipWhiteSpace((char *)after_mcro);
    if (*after_mcro != '\0' && *after_mcro != '\n' && *after_mcro != ';') {
        printf("\nExtraneous text after line. (Line %d, file: \"%s\")\n\n",
               lineCounter, fname_as);
        return -1;
    }

    return 1;
}

  int mcro_close_status(const char *line, const char *fname_as)
{
    const char *p = skipWhiteSpace((char *)line);

    if (strncmp(p, "mcroend", 7) != 0) return 0;

    if (p[7] != '\0' && p[7] != '\n' && p[7] != ' ' && p[7] != '\t' && p[7] != ';') {
        printf("\nText glued to 'mcroend' directive. (Line %d, file: \"%s\")\n\n",
               lineCounter, fname_as);
        return -1;
    }

    p = skipWhiteSpace((char *)(p + 7));
    if (*p != '\0' && *p != '\n' && *p != ';') {
        printf("\nExtraneous text after line. (Line %d, file: \"%s\")\n\n",
               lineCounter, fname_as);
        return 2;
    }

    return 1;
}

  const char *scan_label_prefix(const char *s, char *label, size_t label_sz)
{
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

  const char *scan_token(const char *s, char *buf, size_t buf_sz)
{
    const char *p = skipWhiteSpace((char *)s);
    size_t i = 0;
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
    for (i = 0; i < macroCounter; ++i) {
        if (macroArr[i] && macroArr[i]->name && strcmp(macroArr[i]->name, name) == 0)
            return macroArr[i];
    }
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
    int readLine;

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

        {
            int st = mcro_close_status(lineBuf.content, fname_as);
            if (st == 1) break;
            if (st == 2) { if (pErr) (*pErr)++; break; }
            if (st == -1) { if (pErr) (*pErr)++; }
        }

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


/* ============================================================================================== */
/* read from fileArr[index] (.as), write to fileArr[amOffset+index] (.am) */
int preAssemble(int index)
{
    FILE *in_fp, *out_fp;
    char label[MAX_LABEL_LENGTH];
    int countError = 0;
    LineData currentLine;
    char *after_label;
    int readLine;

    char fname_as[buffer_size];
    {
        const char *src = NULL;
        if (argvPointer && *argvPointer && (*argvPointer)[index+1])
            src = (*argvPointer)[index+1];
        else
            src = nameArr[index];

        strncpy(fname_as, src ? src : "unknown", sizeof(fname_as)-1);
        fname_as[sizeof(fname_as)-1] = '\0';

        {
            size_t L = strlen(fname_as);
            if (L >= 3 && fname_as[L-3]=='.' && fname_as[L-2]=='a' && fname_as[L-1]=='m')
                fname_as[L-1] = 's';
        }
    }

    {
        int savedFileCounter = fileCounter;
        int savedLineCounter = lineCounter;

        fileCounter = index + 1;
        lineCounter = 0;

        in_fp  = fileArr[index];
        out_fp = fileArr[amOffset + index];

        if (check_fileExistence(in_fp) == ERROR || check_fileExistence(out_fp) == ERROR) {
            fileCounter = savedFileCounter;
            lineCounter = savedLineCounter;
            return ERROR;
        }

        freeMacroArr();
        if (initializeMacroArr() == ERROR) {
            fileCounter = savedFileCounter;
            lineCounter = savedLineCounter;
            return ERROR;
        }

        readLine = takeInLine(currentLine.content, in_fp);
        while (readLine != EOF_only_line) {
            if (readLine != 0) {
                countError++; /* too-long line counted */
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
                } else if (st == -1) {
                    countError++;
                    if (macroName[0] != '\0' && addMacro(macroName) == 0) {
                        if (read_mcro_body(in_fp, macroName, &countError, fname_as) == ERROR)
                            countError++;
                    }
                    readLine = takeInLine(currentLine.content, in_fp);
                    continue;
                } else if (st == -2) {
                    countError++;
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

        /* restore globals */
        fileCounter = savedFileCounter;
        lineCounter = savedLineCounter;
    }

    return countError;
}
