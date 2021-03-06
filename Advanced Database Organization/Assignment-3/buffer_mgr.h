#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

// Include return codes and methods for logging errors
#include "dberror.h"

// Include bool DT
#include "dt.h"
#include "storage_mgr.h"

// Replacement Strategies
typedef enum ReplacementStrategy {
  RS_FIFO = 0,
  RS_LRU = 1,
  RS_CLOCK = 2,
  RS_LFU = 3,
  RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1

typedef struct BM_BufferPool {
  char *pageFile;
  int numPages;
  ReplacementStrategy strategy;
  void *mgmtData; // use this one to store the bookkeeping info your buffer 
                  // manager needs for a buffer pool
} BM_BufferPool;

typedef struct BM_PageHandle {
  PageNumber pageNum;
  char *data;
} BM_PageHandle;

typedef struct BM_BufferPool_MgmtInfo{
    SM_FileHandle *fileHandle;
    PageNumber *frameContent;
    bool *dirty;
    int *fixCount;
    BM_PageHandle *pageAddress;
    int readIOCount;
    int writeIOCount;
    // used in case of FIFO Page Replacement strategy
    int nextFrameToBeReplaced;
    // used in case of LRU Page Replacement strategy
    int *lruRecord;
} BM_BufferPool_MgmtInfo;

// convenience macros
#define MAKE_POOL()					\
  ((BM_BufferPool *) malloc (sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE()				\
  ((BM_PageHandle *) malloc (sizeof(BM_PageHandle)))

// Buffer Manager Interface Pool Handling
extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, 
		  void *stratData);
extern RC shutdownBufferPool(BM_BufferPool *const bm);
extern RC forceFlushPool(BM_BufferPool *const bm);

// Buffer Manager Interface Access Pages
extern RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);
extern RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
extern RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
extern RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
	    const PageNumber pageNum);

// Statistics Interface
extern PageNumber *getFrameContents (BM_BufferPool *const bm);
extern bool *getDirtyFlags (BM_BufferPool *const bm);
extern int *getFixCounts (BM_BufferPool *const bm);
extern int getNumReadIO (BM_BufferPool *const bm);
extern int getNumWriteIO (BM_BufferPool *const bm);


RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, 
		  void *stratData){
    SM_FileHandle *fh = malloc(sizeof(SM_FileHandle));
    RC returnCode;
    
    /* Variables to populate default values */
    int i;
    PageNumber defaultFrame = -1;
    bool defaultDirtyMark = false;
    int defaultFixCount = 0;
    /* Pointer pointing to the Buffer Pool Management Information */
    BM_BufferPool_MgmtInfo *bm_MgmtInfo = malloc(sizeof(BM_BufferPool_MgmtInfo));
    //BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo->frameContent = (PageNumber *) malloc (sizeof(PageNumber) * numPages);
    bm_MgmtInfo->dirty = (bool *) malloc (sizeof(bool) * numPages);
    bm_MgmtInfo->fixCount = (int *) malloc (sizeof(int) * numPages);
    bm_MgmtInfo->pageAddress = (BM_PageHandle *) malloc (sizeof(BM_PageHandle) * numPages);
    bm_MgmtInfo->lruRecord = (int *) malloc (sizeof(int) * numPages);
    
    returnCode = openPageFile (pageFileName, fh);
    if (returnCode == RC_OK){
        bm->pageFile = pageFileName;
        bm->numPages = numPages;
        bm->strategy = strategy;
        
        for (i=0;i<numPages;i++){
            bm_MgmtInfo->frameContent [i] = defaultFrame;
            bm_MgmtInfo->dirty [i] = defaultDirtyMark;
            bm_MgmtInfo->fixCount [i] = defaultFixCount;
            bm_MgmtInfo->lruRecord [i] = -1;
        }
        bm_MgmtInfo->fileHandle = fh;
        bm_MgmtInfo->readIOCount = 0;
        bm_MgmtInfo->writeIOCount = 0;
        bm_MgmtInfo->nextFrameToBeReplaced = -1;
        bm->mgmtData = bm_MgmtInfo;
    }
    
    return returnCode;
}

RC shutdownBufferPool(BM_BufferPool *const bm){
    RC returnCode;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    SM_FileHandle *fh;
    bm_MgmtInfo = bm->mgmtData;
    fh = bm_MgmtInfo->fileHandle;
    returnCode = forceFlushPool(bm);
    returnCode = closePageFile (fh);
    return returnCode;
}

RC forceFlushPool(BM_BufferPool *const bm){
    RC returnCode;
    int i, numPages;
    
    returnCode = RC_OK;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    BM_PageHandle *ph = malloc(sizeof(BM_PageHandle));
    SM_FileHandle *fh;
    bm_MgmtInfo = bm->mgmtData;
    fh = bm_MgmtInfo->fileHandle;
    numPages = bm->numPages;
        
    /* Check for Dirty Pages */
    for (i=0;i<numPages;i++){
        if ((bm_MgmtInfo->dirty [i] == true) && (bm_MgmtInfo->fixCount [i] == 0)){
            *ph = bm_MgmtInfo->pageAddress [i];
            returnCode = writeBlock (bm_MgmtInfo->frameContent [i], fh, ph->data);
            if (returnCode == RC_OK){  
                bm_MgmtInfo->dirty [i] = false;
                bm_MgmtInfo->writeIOCount++;
            }
        }
    }
        
    /* return RC code */
    return returnCode;
}

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
    RC returnCode;
    int i, numPages;
    
    returnCode = RC_OK;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    numPages = bm->numPages;
    
    for(i=0;i<numPages;i++){
        if (bm_MgmtInfo->frameContent [i] == page->pageNum){
            bm_MgmtInfo->dirty [i] = true;
            i = numPages;
        }
    }
    
    return returnCode;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
    RC returnCode;
    int i, numPages;
    
    returnCode = RC_OK;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    SM_FileHandle *fh;
    bm_MgmtInfo = bm->mgmtData;
    fh = bm_MgmtInfo->fileHandle;
    numPages = bm->numPages;
    
    for (i=0;i<numPages;i++){
        if (bm_MgmtInfo->frameContent [i] == page->pageNum){
            bm_MgmtInfo->fixCount [i]--;
            /*if (bm_MgmtInfo->dirty [i] == true){
                returnCode = writeBlock (bm_MgmtInfo->frameContent [i], fh, page->data);
                if (returnCode == RC_OK){
                    bm_MgmtInfo->writeIOCount++;
                }
            }
            if (returnCode == RC_OK){
                bm_MgmtInfo->fixCount [i]--;
            }*/
            i = numPages;
        }
    }
    
    return returnCode;
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
    RC returnCode;
    int i, numPages;
    
    returnCode = RC_OK;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    SM_FileHandle *fh;
    bm_MgmtInfo = bm->mgmtData;
    fh = bm_MgmtInfo->fileHandle;
    numPages = bm->numPages;
    
    /* Check for Dirty Pages */
    for (i=0;i<numPages;i++){
        if ((bm_MgmtInfo->dirty [i] == true) && (bm_MgmtInfo->frameContent [i] == page->pageNum)){
            returnCode = writeBlock (bm_MgmtInfo->frameContent [i], fh, page->data);
            if (returnCode == RC_OK){  
                bm_MgmtInfo->dirty [i] = false;
                bm_MgmtInfo->writeIOCount++;
            }
            i = numPages;
        }
    }
    
    free(bm_MgmtInfo);
    free(fh);
    return returnCode;
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
	    const PageNumber pageNum){
    RC returnCode;
    int i, j, numPages, frameToBeReplaced, pageFileSize,maxFixCount,pageFoundInMem,nextFrame, startLRUMove;
    returnCode = RC_OK;
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    SM_FileHandle *fh;
    BM_PageHandle *ph = malloc(sizeof(BM_PageHandle));
    SM_PageHandle memPage = malloc(sizeof(SM_PageHandle));
    
    bm_MgmtInfo = bm->mgmtData;
    fh = bm_MgmtInfo->fileHandle;
    numPages = bm->numPages;
    pageFoundInMem = 0;
    frameToBeReplaced = -1;
    nextFrame = bm_MgmtInfo->nextFrameToBeReplaced;
    
    /******************************************************/
    /* Code to check if the page is already in the memory */
    /******************************************************/
    for (i=0;i<numPages;i++){
        if (bm_MgmtInfo->frameContent [i] == pageNum){
            *page = bm_MgmtInfo->pageAddress [i];
            bm_MgmtInfo->fixCount [i]++;
            startLRUMove = 0;
            switch(bm->strategy){
                case RS_LRU:
                    for (j=0;j<numPages - 1;j++){
                        if (bm_MgmtInfo->lruRecord [j] == i){
                            startLRUMove = 1;
                        }
                        if (startLRUMove == 1){
                            bm_MgmtInfo->lruRecord [j] = bm_MgmtInfo->lruRecord [j+1];
                        }
                    }
                    bm_MgmtInfo->lruRecord [numPages - 1] = -1;
                    for (j=0;j<numPages;j++){
                        if (bm_MgmtInfo->lruRecord [j] == -1){
                            bm_MgmtInfo->lruRecord [j] = i;
                            j = numPages;
                        }
                    }
                    break;
            }
            pageFoundInMem = 1;
            i = numPages;
        }
    }
    
    if (pageFoundInMem == 0){
        /********************************************************/
        /* Code to find the page frame to be replaced goes here */
        /********************************************************/
        /********************************************************/
        /* First we will check for empty frames in buffer pool  */
        /********************************************************/
        for (i=0;i<numPages;i++){
            if (bm_MgmtInfo->frameContent [i] == -1){
                frameToBeReplaced = i;
                i = numPages;
            }
        }
        
        /********************************************************/
        /* In case of no empty frames, we will use replacement  */
        /* strategy to remove a page from frame and then load it*/
        /* with different page from page file                   */
        /********************************************************/
        if(frameToBeReplaced == -1){
            switch(bm->strategy){
                case RS_FIFO:
                    do {
                        nextFrame++;
                        if (nextFrame == numPages) {
                            nextFrame = 0;
                        }
                        bm_MgmtInfo->nextFrameToBeReplaced = nextFrame;
                    }while(bm_MgmtInfo->fixCount [nextFrame] > 0);
                    frameToBeReplaced = bm_MgmtInfo->nextFrameToBeReplaced;
                    break;
                case RS_LRU:
                    frameToBeReplaced = bm_MgmtInfo->lruRecord [0];
                    for (i=0;i< (numPages - 1);i++){
                        bm_MgmtInfo->lruRecord [i] = bm_MgmtInfo->lruRecord [i+1];
                    }
                    bm_MgmtInfo->lruRecord [numPages - 1] = -1;
                    break;
                default:
                    printf("\nReplacement Strategy not found\n");
            }
        }
        /* Condition to validate whether we found the replacement page */
        if (frameToBeReplaced == -1){
            /* return Error that page cannot be replaced */
            returnCode = RC_READ_NON_EXISTING_PAGE;
        }
        else{
            if (bm_MgmtInfo->dirty [frameToBeReplaced] == true){
               *ph = bm_MgmtInfo->pageAddress [frameToBeReplaced];
                returnCode = writeBlock (bm_MgmtInfo->frameContent [frameToBeReplaced], fh, ph->data);
                if (returnCode == RC_OK){
                    bm_MgmtInfo->writeIOCount++;
                }
            }
            pageFileSize = fh->totalNumPages;
            if (pageNum >= pageFileSize){
                returnCode = ensureCapacity (pageNum + 1, fh);
            }
            returnCode = readBlock (pageNum, fh, memPage);
            if (returnCode == RC_OK){
                page->pageNum = pageNum;
                page->data = memPage;
                bm_MgmtInfo->fixCount [frameToBeReplaced]++;
                bm_MgmtInfo->frameContent [frameToBeReplaced] = pageNum;
                bm_MgmtInfo->dirty [frameToBeReplaced] = false;
                bm_MgmtInfo->pageAddress [frameToBeReplaced] = *page;
                bm_MgmtInfo->readIOCount++;
                switch (bm->strategy){
                    case RS_LRU:
                        for (i=0;i<numPages;i++){
                            if (bm_MgmtInfo->lruRecord [i] == -1){
                                bm_MgmtInfo->lruRecord [i] = frameToBeReplaced;
                                i = numPages;
                            }
                        }
                        break;
                }
            }
        }
    }
    return returnCode;
}

PageNumber *getFrameContents (BM_BufferPool *const bm){
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    return bm_MgmtInfo->frameContent;
}

bool *getDirtyFlags (BM_BufferPool *const bm){
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    return bm_MgmtInfo->dirty;
}

int *getFixCounts (BM_BufferPool *const bm){
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    return bm_MgmtInfo->fixCount;
}

int getNumReadIO (BM_BufferPool *const bm){
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    return bm_MgmtInfo->readIOCount;
}

int getNumWriteIO (BM_BufferPool *const bm){
    BM_BufferPool_MgmtInfo *bm_MgmtInfo;
    bm_MgmtInfo = bm->mgmtData;
    return bm_MgmtInfo->writeIOCount;
}
#endif