#include "prototypes.h"
#include "header.h"

lineNode **lineArr;
char **nameArr;
FILE **fileArr;
binTree **labelTable;
macro **macroArr;
int *icArr;
int *dcArr;
int lineCounter;
int fileCounter;
int macroCounter;
int amOffset;
lineNode **entryLineArr;
lineNode **externLineArr;
char ***argvPointer;

int main (int argc, char *argv[])
{
    int numFiles = argc-1, errorFlag = 0, res;
    int i;

    labelTable = NULL;
    macroArr = NULL;
    macroCounter = 0;
    icArr = NULL;
    dcArr = NULL;
    entryLineArr = NULL;
    externLineArr = NULL;
    argvPointer = &argv;
    amOffset = numFiles;

    if (check_fileEntered(argc) == ERROR || check_fileName(numFiles) == ERROR)
        goto cleanUp;

    if (initializeLabelTables(argc) == ERROR)
        goto cleanUp;

    lineArr = (lineNode **)calloc(numFiles, sizeof(lineNode *));
    if (check_allocation(lineArr) == ERROR)
        goto cleanUp;

    icArr = (int *)calloc(numFiles, sizeof(int));
    dcArr = (int *)calloc(numFiles, sizeof(int));
    if (check_allocation(icArr) == ERROR || check_allocation(dcArr) == ERROR)
        goto cleanUp;

    entryLineArr = (lineNode **)calloc(numFiles, sizeof(lineNode *));
    if (check_allocation(entryLineArr) == ERROR)
        goto cleanUp;

    externLineArr = (lineNode **)calloc(numFiles, sizeof(lineNode *));
    if (check_allocation(externLineArr) == ERROR)
        goto cleanUp;

    if (initializeMacroArr() == ERROR)
        goto cleanUp;

    fileArr = getFiles(argc, argv);
    if (fileArr == NULL)
        goto cleanUp;

    nameArr = make_nameArr(argc, argv);
    if (nameArr == NULL)
        goto cleanUp;

    for (i = 0; i < numFiles; ++i)
    {
        fileCounter = i;
        lineCounter = 0;

        if (create_amFile(argc, fileArr, nameArr, i) == ERROR) {
            errorFlag = 1;
            continue;
        }

        res = preAssemble(i);
        if (res != 0) {
            printf("======= Pre-assembler found %d issue(s) in file %s =======\n", res, (*argvPointer)[i+1]);
            errorFlag = 1;
        }

        if (fileArr[(argc-1) + i])
            fflush(fileArr[amOffset + i]);
    }

    if (errorFlag) {
        printf("======= Errors were found in the pre-assembler. Compilation terminated =======\n");
        goto cleanUp;
    }
    printf("======= Pre-assembler completed succesfully =======\n");

    for (i = 0; i < numFiles; ++i)
    {
        fileCounter = i;
        lineCounter = 0;
        res = firstPass(i);
        if (res != 0) {
            printf("\n\n======= Error in first pass of file %s (%d errors found) =======\n", (*argvPointer)[i+1], res);
            errorFlag = 1;
        }
    }

    if (errorFlag) {
        printf("======= Errors were found in the first pass. Compilation terminated =======\n");
        goto cleanUp;
    }
    printf("======= First pass completed succesfully =======\n");

    if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
        goto cleanUp;
    printf("======= Second pass completed succesfully =======\n");
    printf("\n\n======= Compilation over =======\n");

cleanUp:
    closeFiles(argc, fileArr);
    freeLabelTable(&labelTable, numFiles);
    freeListArr(&lineArr, numFiles);
    freeNameArr(&nameArr, numFiles);
    freeMacroArr();
    freeFileArr(&fileArr);
    free(icArr);
    free(dcArr);
    freeListArr(&entryLineArr, numFiles);
    freeListArr(&externLineArr, numFiles);
    return 0;
}
