#include "prototypes.h"
#include <string.h>

FILE **getFiles(int argc, char *argv[])
{
    FILE **arr;
    FILE *fp;
    int i, savedFC;

    arr = (FILE **)calloc(total_num_of_files(argc), sizeof(FILE *));
    if (check_allocation(arr) == ERROR)
        return NULL;

    savedFC = fileCounter;

    for (i = 0; i < argc - 1; ++i)
    {
        fileCounter = i + 1;
        fp = fopen(argv[i+1], "r");
        if (check_fileExistence(fp) == ERROR) {
            fileCounter = savedFC;
            return NULL;
        }
        arr[i] = fp;
    }

    fileCounter = savedFC;
    return arr;
}

char **make_nameArr(int argc, char *argv[])
{
    int i;
    char **names;

    names = (char **)calloc(argc - 1, sizeof(char *));
    if (check_allocation(names) == ERROR)
        return NULL;

    for (i = 0; i < argc - 1; ++i)
    {
        names[i] = (char *)malloc(buffer_size * sizeof(char));
        if (check_allocation(names[i]) == ERROR)
            return NULL;
        strcpy(names[i], argv[i+1]);
    }
    return names;
}

static int build_out_name_am(const char *in_as, char out[], int out_sz)
{
    int len;
    if (!in_as) return ERROR;
    len = (int)strlen(in_as);
    if (len < 3 || out_sz < len + 1) return ERROR;
    strcpy(out, in_as);
    out[len-2] = 'a';
    out[len-1] = 'm';
    return 0;
}

static int build_out_name_ob(const char *in_as, char out[], int out_sz)
{
    int len;
    if (!in_as) return ERROR;
    len = (int)strlen(in_as);
    if (len < 3 || out_sz < len + 1) return ERROR;
    strcpy(out, in_as);
    out[len-2] = 'o';
    out[len-1] = 'b';
    return 0;
}

static int build_out_name_ent(const char *in_as, char out[], int out_sz)
{
    int len;
    if (!in_as) return ERROR;
    len = (int)strlen(in_as);
    if (len < 3 || out_sz < len + 2) return ERROR;
    strcpy(out, in_as);
    out[len-2] = 'e';
    out[len-1] = 'n';
    out[len]   = 't';
    out[len+1] = '\0';
    return 0;
}

static int build_out_name_ext(const char *in_as, char out[], int out_sz)
{
    int len;
    if (!in_as) return ERROR;
    len = (int)strlen(in_as);
    if (len < 3 || out_sz < len + 2) return ERROR;
    strcpy(out, in_as);
    out[len-2] = 'e';
    out[len-1] = 'x';
    out[len]   = 't';
    out[len+1] = '\0';
    return 0;
}

int create_amFile(int argc, FILE **fileArr, char **nameArr, int i)
{
    char out[MAX_LINE_LENGTH];
    FILE *fp;

    if (build_out_name_am(nameArr[i], out, (int)sizeof(out)) == ERROR)
        return ERROR;

    fp = fopen(out, "w+");
    if (check_newFileExistence(fp) == ERROR)
        return ERROR;

    fileArr[(argc-1) + i] = fp;
    printf("\t%s\n", out);
    return 0;
}

int create_obFile(int argc, FILE **fileArr, char **nameArr, int i)
{
    char out[MAX_LINE_LENGTH];
    FILE *fp;

    if (build_out_name_ob(nameArr[i], out, (int)sizeof(out)) == ERROR)
        return ERROR;

    fp = fopen(out, "w+");
    if (check_newFileExistence(fp) == ERROR)
        return ERROR;

    fileArr[2*(argc-1) + i] = fp;
    printf("\t%s\n", out);
    return 0;
}

int create_entFile(int argc, FILE **fileArr, char **nameArr, int i)
{
    char out[MAX_LINE_LENGTH];
    FILE *fp;

    if (build_out_name_ent(nameArr[i], out, (int)sizeof(out)) == ERROR)
        return ERROR;

    fp = fopen(out, "w+");
    if (check_newFileExistence(fp) == ERROR)
        return ERROR;

    fileArr[3*(argc-1) + i] = fp;
    printf("\t%s\n", out);
    return 0;
}

int create_extFile(int argc, FILE **fileArr, char **nameArr, int i)
{
    char out[MAX_LINE_LENGTH];
    FILE *fp;

    if (build_out_name_ext(nameArr[i], out, (int)sizeof(out)) == ERROR)
        return ERROR;

    fp = fopen(out, "w+");
    if (check_newFileExistence(fp) == ERROR)
        return ERROR;

    fileArr[4*(argc-1) + i] = fp;
    printf("\t%s\n", out);
    return 0;
}

int closeFiles(int argc, FILE **fileArr)
{
    int i;
    if (fileArr == NULL)
        return 0;
    for (i = 0; i < total_num_of_files(argc); ++i)
        if (fileArr[i] != NULL)
            fclose(fileArr[i]);
    return 0;
}

int takeInLine(char buffer[], FILE *source)
{
    int flag;
    char *c, *fgetsStatus;

    do {
        flag = 0;
        fgetsStatus = fgets(buffer, buffer_size, source);
        lineCounter++;
        if (fgetsStatus == NULL)
            return EOF_only_line;
        c = skipWhiteSpace(fgetsStatus);
        if (*c == ';')
            continue;
        if (check_lineLength(buffer) == ERROR)
            continue;
        c = skipWhiteSpace(c);
        if (*c != '\n')
            flag = 1;
    } while (flag == 0);

    return 0;
}

int recognize_opcode(char *code)
{
    int i, status;
    for (i = 0; i < num_of_opcodes && strcmp(code, opcodeTable[i].name); ++i) ;
    status = check_opcodeName(i);
    if (status == 0)
        return i;
    return ERROR;
}

static int writeEntNode(FILE *file, binTree *root)
{
    char address[address_binary_representation_size+1];
    if (!root) return 0;
    writeEntNode(file, root->left);
    if (root->isEntry) {
        base10_to_base2_forAddress(root->address, address);
        fprintf(file, "%s\t\t", root->str);
        base2_to_base4_strToFile(address, file);
        fprintf(file, "\n");
    }
    writeEntNode(file, root->right);
    return 0;
}

static int writeExtNode(FILE *file, binTree *root)
{
    char address[address_binary_representation_size+1];
    if (!root) return 0;
    writeExtNode(file, root->left);
    if (root->isExternal) {
        base10_to_base2_forAddress(root->address, address);
        fprintf(file, "%s\t\t", root->str);
        base2_to_base4_strToFile(address, file);
        fprintf(file, "\n");
    }
    writeExtNode(file, root->right);
    return 0;
}

int writeEnt(FILE *file)
{
    return writeEntNode(file, labelTable[fileCounter]);
}

int writeExt(FILE *file)
{
    return writeExtNode(file, labelTable[fileCounter]);
}
