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
    int numFiles = argc - 1, errorFlag = 0, res;

    labelTable = NULL;
    macroArr = NULL;
    macroCounter = 0;
    icArr = NULL;
    dcArr = NULL;
    entryLineArr = NULL;
    externLineArr = NULL;
    argvPointer = &argv;
    amOffset = numFiles;

    /* 1) check that source file/s were entered, and are legal */
    if (check_fileEntered(argc) == ERROR || check_fileName(numFiles) == ERROR)
        goto cleanUp;

    /* 2) initialize relevant structs and arrays */
    if (initializeLabelTables(argc) == ERROR) /* initialize labelTables array */
        goto cleanUp;

    lineArr = calloc(numFiles, sizeof(lineNode *)); /* array of lists (each list = file) */
    if (check_allocation(lineArr) == ERROR)
        goto cleanUp;

    icArr = calloc(numFiles, sizeof(int)); /* IC per file */
    dcArr = calloc(numFiles, sizeof(int)); /* DC per file */
    if (check_allocation(icArr) == ERROR || check_allocation(dcArr) == ERROR)
        goto cleanUp;

    entryLineArr = calloc(numFiles, sizeof(lineNode *)); /* .entry lines per file */
    if (check_allocation(entryLineArr) == ERROR)
        goto cleanUp;

    externLineArr = calloc(numFiles, sizeof(lineNode *)); /* lines using extern labels per file */
    if (check_allocation(externLineArr) == ERROR)
        goto cleanUp;

    if (initializeMacroArr() == ERROR) /* macro table */
        goto cleanUp;

    /* 3) open .as files (store in fileArr) */
    fileArr = getFiles(argc, argv);
    if (fileArr == NULL)
        goto cleanUp;

    /* 4) build nameArr (file base names) */
    nameArr = make_nameArr(argc, argv);
    if (nameArr == NULL)
        goto cleanUp;

    /* 5) pre-assembler */
    for (fileCounter = 0; fileCounter < numFiles; ++fileCounter)
    {
        lineCounter = 0;

        if (create_amFile(argc, fileArr, nameArr, fileCounter) == ERROR) {
            errorFlag = 1;
            continue;
        }

        res = preAssemble(fileCounter);
        if (res != 0) {
            printf("======= Pre-assembler found %d issue(s) in file %s =======\n", res, (*argvPointer)[fileCounter + 1]);
            errorFlag = 1;
        }

        if (fileArr[(argc - 1) + fileCounter]) {
            fflush(fileArr[amOffset + fileCounter]);
        }
    }

    if (errorFlag) {
        printf("======= Errors were found in the pre-assembler. Compilation terminated =======\n");
        goto cleanUp;
    }

    printf("======= Pre-assembler completed successfully =======\n");

    /* 6) first pass */
    for (fileCounter = 0; fileCounter < numFiles; fileCounter++)
    {
        lineCounter = 0;
        res = firstPass(fileCounter);
        if (res != 0) {
            printf("\n\n======= Error in first pass of file %s (%d errors found) =======\n", nameArr[fileCounter], res);
            errorFlag = 1;
        }
    }

    if (errorFlag) {
        printf("======= Errors were found in the first pass. Compilation terminated =======\n");
        goto cleanUp;
    }

    printf("======= First pass completed successfully =======\n");

    /* 7) second pass */
    if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
        goto cleanUp;

    printf("======= Second pass completed successfully =======\n");
    printf("\n\n======= Compilation over =======\n");

    /* 8) cleanup everything - close all files and free all memory */
cleanUp:
    closeFiles(argc, fileArr);            /* close all open files */
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
