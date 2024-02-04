#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_mgr.h"
#include "dberror.h"

FILE *file_obj;
RC returnCode;
#define FILE_REPERMISSIONS "r+"

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    return readBlock(0, fHandle, memPage);
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    if (fileName == NULL || fHandle == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    file_obj = fopen(fileName, FILE_REPERMISSIONS);
    if (file_obj == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    fseek(file_obj, 0, SEEK_END);
    fHandle->totalNumPages = ftell(file_obj) / PAGE_SIZE;
    fHandle->curPagePos = 0;
    fHandle->fileName = fileName;
    rewind(file_obj);
    return RC_OK;
}

RC createPageFile(char *fileName) {
    file_obj = fopen(fileName, FILE_REPERMISSIONS);
    if (file_obj == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    char *memory_block = calloc(PAGE_SIZE, sizeof(char));
    memset(memory_block, '\0', PAGE_SIZE);
    fwrite(memory_block, sizeof(char), PAGE_SIZE, file_obj);

    free(memory_block);
    fclose(file_obj);

    return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle) {
    if (fclose(file_obj) != 0) {
        return RC_FILE_NOT_FOUND;
    }

    return RC_OK;
}

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->fileName == NULL || pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    fseek(file_obj, pageNum * PAGE_SIZE, SEEK_SET);
    fread(memPage, sizeof(char), PAGE_SIZE, file_obj);

    fHandle->curPagePos = pageNum;
    return RC_OK;
}

RC destroyPageFile(char *Fname) {
    if (remove(Fname) == 0) {
        return RC_OK;
    }

    return RC_FILE_NOT_FOUND;
}

RC getBlockPos(SM_FileHandle *fHandle) {
    if (fHandle == NULL || fHandle->fileName == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    return fHandle->curPagePos;
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    int pos = fHandle->totalNumPages - 1;
    return readBlock(pos, fHandle, memPage);
}

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    int pagePos = getBlockPos(fHandle);
    if (pagePos <= 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    return readBlock(pagePos - 1, fHandle, memPage);
}

RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    int pagesToAdd = numberOfPages - fHandle->totalNumPages;

    while (pagesToAdd > 0) {
        appendEmptyBlock(fHandle);
        pagesToAdd--;
    }

    return RC_OK;
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    int pagePos = getBlockPos(fHandle);
    if (pagePos >= fHandle->totalNumPages - 1) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    return readBlock(pagePos + 1, fHandle, memPage);
}

RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    fseek(file_obj, pageNum * PAGE_SIZE, SEEK_SET);
    fwrite(memPage, sizeof(char), PAGE_SIZE, file_obj);

    fHandle->curPagePos = pageNum;
    return RC_OK;
}

void initStorageManager(void) {
}

RC appendEmptyBlock(SM_FileHandle *fHandle) {
    char *emptyBlock = (char *)calloc(PAGE_SIZE, sizeof(char));
    fwrite(emptyBlock, 1, PAGE_SIZE, file_obj);
    free(emptyBlock);

    fHandle->totalNumPages++;
    fHandle->curPagePos = fHandle->totalNumPages - 1;

    return RC_OK;
}

