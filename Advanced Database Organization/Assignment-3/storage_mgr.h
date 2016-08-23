#ifndef STORAGE_MGR_H
#define STORAGE_MGR_H

#include "dberror.h"

/************************************************************
 *                    handle data structures                *
 ************************************************************/
typedef struct SM_FileHandle {
  char *fileName;
  int totalNumPages;
  int curPagePos;
  void *mgmtInfo;
} SM_FileHandle;

typedef char* SM_PageHandle;

/************************************************************
 *                    interface                             *
 ************************************************************/
/* manipulating page files */
extern void initStorageManager (void);
extern RC createPageFile (char *fileName);
extern RC openPageFile (char *fileName, SM_FileHandle *fHandle);
extern RC closePageFile (SM_FileHandle *fHandle);
extern RC destroyPageFile (char *fileName);

/* reading blocks from disc */
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
extern int getBlockPos (SM_FileHandle *fHandle);
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);

/* writing blocks to a page file */
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC appendEmptyBlock (SM_FileHandle *fHandle);
extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle);

/* Dummy Function */

void initStorageManager(void){
}

/* Function to Create a new file for the given name */
RC createPageFile (char *fileName){
    FILE *fptr;
    int i;
    char defaultString[PAGE_SIZE];
    /*Creates a new file with the given name */
    fptr = fopen(fileName, "wb");

    if (!fptr)
        return RC_FILE_HANDLE_NOT_INIT;
    else{
        for (i=0;i<PAGE_SIZE;++i){
            defaultString[i] = '\0';
        }
        
        /* Writes an empty page */
        fwrite(defaultString,PAGE_SIZE,1,fptr);
        
        /* Closes the file */
        fclose(fptr);
        
        return RC_OK;
    }
}


/* Function to open the the given file name */
RC openPageFile (char *fileName, SM_FileHandle *fHandle){
    FILE *fptr;
    int numberOfItems,readItemInd;
    char fileRecord[PAGE_SIZE];

    /*Opens the mentioned file */
    fptr = fopen(fileName, "r+b");

    if (!fptr)
        return RC_FILE_NOT_FOUND;
    else{

		/* Initialising the total number of Items */
        numberOfItems = 0;
        
		/* The below do-while loop is to read all the entries in the file */
        do{
			/* readItemInd will be 1 if 1 item is read */
            readItemInd = fread(fileRecord,PAGE_SIZE,1,fptr);
            if (readItemInd)
                ++numberOfItems;
        }while(readItemInd);
        
		/* Below are the assignment statements to*/
		/* initialise the File Handle structure */
        fHandle->fileName = fileName;           /*File name*/
        fHandle->totalNumPages = numberOfItems; /* Total number of Items */
        fHandle->curPagePos = 0;                /* Current page position */
        fHandle->mgmtInfo = fptr;               /*Address of the file */

		/* close the file */
		
        return RC_OK;
    }
}

/* Function to close the file */
RC closePageFile (SM_FileHandle *fHandle){
    FILE *fptr;
	/* Get the File Address */
    fptr = fHandle->mgmtInfo;
    fclose(fptr);

    return RC_OK;
}

/* Function to delete the file */
RC destroyPageFile (char *fileName){
    
	int returnCode;
    returnCode = remove(fileName);
    
	if (returnCode == 0)
        return RC_OK;
    else
        return RC_FILE_NOT_FOUND;
}

/* Function to read a given page number from the file */
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;
    
	/* Set the location to required page number */
	fseek(fptr,pageNum*PAGE_SIZE,SEEK_SET);

	/* Read the page */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);
    
	
	if (fileReadInd){

		/* Update the current page position in the structure */
        fHandle->curPagePos = pageNum;
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to get the Current Page number */
int getBlockPos (SM_FileHandle *fHandle){
    int curPagePos;
	/* Get the current page number from file handler */
    curPagePos = fHandle->curPagePos;
    return curPagePos;
}

/* Function to read the First Page of the file */
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd;
	
	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Set the location to the first page of the file */
    fseek(fptr,0,SEEK_SET);

	/* Read the first page */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);
    if (fileReadInd){
		/*Update the current page position to first page */
        fHandle->curPagePos = 0;
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to read the previous page of the file */
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd,prevPage;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Condition to avoid reading the previous page */
	/* if the current page is first page */
    if (fHandle->curPagePos == 0){
        return RC_READ_NON_EXISTING_PAGE;
    }
    else{
        prevPage = (fHandle->curPagePos - 1);
    }

	/* Set the location to read the previous page */
    fseek(fptr,prevPage*PAGE_SIZE,SEEK_SET);

	/* Read the previous page */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);

    if (fileReadInd){
		/* Update the current page position */
		/* to the previous page */
        fHandle->curPagePos = prevPage;
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to read the current page of the file */
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd,curPage;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Get the current page number */
    curPage = fHandle->curPagePos;

	/* Set the location to read the current page of the file */
    fseek(fptr,curPage*PAGE_SIZE,SEEK_SET);

	/* Read the current page of the file */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);

    if (fileReadInd){
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to read the next page of the fiole */
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd,nextPage,lastPage;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Get the last page number of the file*/
	lastPage = fHandle->totalNumPages - 1;

	/* Condition to avoid reading the next page */
	/* if the current page is last page */
    if (fHandle->curPagePos == lastPage){
        return RC_READ_NON_EXISTING_PAGE;
    }
    else{
        nextPage = (fHandle->curPagePos + 1);
    }

	/* Set the location to read the next page of the file */
    fseek(fptr,nextPage*PAGE_SIZE,SEEK_SET);

	/* Read the next page on the file */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);

    if (fileReadInd){
		/* Update the current page position */
        fHandle->curPagePos = nextPage;
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to read the last page number of the file */
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileReadInd,lastPage;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Get the last page number of the file */
    lastPage = fHandle->totalNumPages - 1;

	/* Set the location to read the last page of the file */
    fseek(fptr,lastPage*PAGE_SIZE,SEEK_SET);

	/* Read the last page of the file */
    fileReadInd = fread(memPage,PAGE_SIZE,1,fptr);

    if (fileReadInd){
		/* Update the current page position */
        fHandle->curPagePos = lastPage;
        return RC_OK;
    }
    else{
        return RC_READ_NON_EXISTING_PAGE;
    }
}

/* Function to write a given page number of the file */
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileWriteInd;
    RC ensureCapacityRC;
	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

    if (pageNum > fHandle->totalNumPages){
        ensureCapacityRC = ensureCapacity(pageNum, fHandle);
        if (ensureCapacityRC == 0){
            return RC_WRITE_FAILED;
        }
    }
	/* Set the location to write the given page number */
    fseek(fptr,pageNum*PAGE_SIZE,SEEK_SET);

	/* Write the content to the given page number of the file */
    fileWriteInd = fwrite(memPage,PAGE_SIZE,1,fptr);
    if (fileWriteInd){
		/* Update the current page position */
        fHandle->curPagePos = pageNum;
        return RC_OK;
    }
    else{
        return RC_WRITE_FAILED;
    }
}

/* Function to write to the current page of the file */
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *fptr;
    int fileWriteInd,curPage;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Get the current page number */
    curPage = fHandle->curPagePos;

	/* Set the location to write the current page */
    fseek(fptr,curPage*PAGE_SIZE,SEEK_SET);

	/* Write the content to the current page of the file */
    fileWriteInd = fwrite(memPage,PAGE_SIZE,1,fptr);

    if (fileWriteInd){
        return RC_OK;
    }
    else{
        return RC_WRITE_FAILED;
    }
}

/* Function to Add an Empty Page at the end to the file */
RC appendEmptyBlock (SM_FileHandle *fHandle){
    FILE *fptr;
    int i,fileWriteInd,lastPage;

	/* Default data for the page */
    char defaultString[PAGE_SIZE];

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

    if (!fptr)
        return RC_FILE_HANDLE_NOT_INIT;
    else{
		/* Make the default string as '\0' */
        for (i=0;i<PAGE_SIZE;++i){
            defaultString[i] = '\0';
        }

		/* Get the page number to be added */
        lastPage = fHandle->totalNumPages;
		
		/* Set the location to the last page to write */
        fseek(fptr,lastPage*PAGE_SIZE,SEEK_SET);

		/* Write the default data */
        fileWriteInd=fwrite(defaultString,PAGE_SIZE,1,fptr);

        if (fileWriteInd) {

			/* Update the current page position */
			fHandle->curPagePos = lastPage;

			/* Update the total number of pages */
            fHandle->totalNumPages = ++lastPage;

            return RC_OK;
        }
        else{
            return RC_WRITE_FAILED;
        }
    }
}

/* Function to ensure the file has the given number of pages */
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
    FILE *fptr;
    int i,curTotalPages;
    RC returnCode;

	/* Get the File Address */
    fptr = fHandle->mgmtInfo;

	/* Get the total number of pages of the file */
    curTotalPages = fHandle->totalNumPages;

	/* Loop in to append new pages till the given number of pages */
    for (i=curTotalPages;i<numberOfPages;++i){
		/* call function to create empty pages */
        returnCode=appendEmptyBlock(fHandle);

        if (returnCode != RC_OK){
            return RC_WRITE_FAILED;
        }
    }
    return RC_OK;
}

#endif