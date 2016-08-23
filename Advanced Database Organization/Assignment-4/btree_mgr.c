//#include <stdio.h>
#include "dberror.h"
#include "tables.h"
#include "btree_mgr.h"
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

// init and shutdown index manager
RC initIndexManager (void *mgmtData){
    return RC_OK;
}
RC shutdownIndexManager (){
    return RC_OK;
}

// create, destroy, open, and close an btree index
RC createBtree (char *idxId, DataType keyType, int n){
    RC returnCode;
    int numberOfNodes, numberOfEntries, rootPageNum, curLastPage;
    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    h->data = (char *) malloc(PAGE_SIZE * sizeof(char));
    numberOfNodes = 0;
    numberOfEntries = 0;
    rootPageNum = -1;
    curLastPage = 0;

    returnCode = createPageFile(idxId);
    if (returnCode == RC_OK){
        CHECK(initBufferPool(bm, idxId, 3, RS_FIFO, NULL));
        CHECK(pinPage(bm, h, 0));
        sprintf(h->data, "%i;%i;%i;%i;%i;%i", keyType, n, numberOfNodes, numberOfEntries, rootPageNum, curLastPage);
        CHECK(markDirty(bm, h));
        CHECK(unpinPage(bm,h));
        CHECK(shutdownBufferPool(bm));
    }
    free(bm);
    free(h);
    return returnCode;
}

RC openBtree (BTreeHandle **tree, char *idxId){
    int nodeSize, numberOfNodes, numberOfEntries, rootPageNum, curLastPage;
    DataType dt;
    BTreeHandle *bth = (BTreeHandle *) malloc(sizeof(BTreeHandle));
    BTreeMgmtInfo *btm = (BTreeMgmtInfo *) malloc(sizeof(BTreeMgmtInfo));
    //btm->bTreeBM = (BM_BufferPool *) malloc(sizeof(BM_BufferPool));
    btm->bTreeBM = MAKE_POOL();
    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    rootPageNum = -1;
    curLastPage = 0;

    CHECK(initBufferPool(bm, idxId, 3, RS_FIFO, NULL));
    CHECK(pinPage(bm, h, 0));
    sscanf(h->data, "%i;%i;%i;%i;%i;%i", &dt, &nodeSize, &numberOfNodes, &numberOfEntries, &rootPageNum, &curLastPage);
    CHECK(unpinPage(bm,h));
    btm->bTreeBM = bm;
    btm->nodeSize = nodeSize;
    btm->numberOfNodes = numberOfNodes;
    btm->numberOfEntries = numberOfEntries;
    btm->rootPageNum = rootPageNum;
    btm->curLastPage = curLastPage;
    bth->idxId = idxId;
    bth->keyType = dt;
    bth->mgmtData = (BTreeMgmtInfo *) btm;
    *tree = bth;
    
    return RC_OK;
}

RC closeBtree (BTreeHandle *tree){
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    
    btm = tree->mgmtData;
    bm = btm->bTreeBM;
    CHECK(shutdownBufferPool(bm));
    return RC_OK;
}

RC deleteBtree (char *idxId){
    CHECK(destroyPageFile(idxId));
    return RC_OK;
}

// access information about a b-tree
RC getNumNodes (BTreeHandle *tree, int *result){
    BTreeMgmtInfo *btm;
    btm = tree->mgmtData;
    *result = btm->numberOfNodes;
    return RC_OK;
}

RC getNumEntries (BTreeHandle *tree, int *result){
    BTreeMgmtInfo *btm;
    btm = tree->mgmtData;
    *result = btm->numberOfEntries;
    return RC_OK;
}
RC getKeyType (BTreeHandle *tree, DataType *result){
    *result = tree->keyType;
    return RC_OK;
}

// index access
RC findKey (BTreeHandle *tree, Value *key, RID *result){
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    int nodeType, numberOfNodeEntries, nextLeafNodePointer, rootPageNum;
    int nodeSize, ridPage, ridSlot;
    int tempNodeDT, tempNodeVal;
    int i, bytesParsed, lengthParsed, pageRequested, breakLoopInd;
    bool recordFound;
    Value *compVal = (Value *) malloc (sizeof(Value));
    Value *compVal2 = (Value *) malloc (sizeof(Value));
    Value tempVal;
    RID tempRID;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    bm = btm->bTreeBM;
    rootPageNum = btm->rootPageNum;
    if (rootPageNum == -1){
        pageRequested = 1;
    }else{
        pageRequested = rootPageNum;
    }
    recordFound = false;
    breakLoopInd = 0;
    
    do{
        lengthParsed = 0;
        bytesParsed = 0;
        CHECK(pinPage(bm, h, pageRequested));
        sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
        CHECK(unpinPage(bm,h));
        lengthParsed = bytesParsed;
    
        Value *nodeEntries = malloc(sizeof(Value) * (nodeSize + 1));
        int *nextNodePointers = malloc(sizeof(int) * (nodeSize + 1));
        RID *recordPointers = malloc(sizeof(RID) * nodeSize);
    
        for(i=0; i < nodeSize; i++){
            sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
            nodeEntries [i] = tempVal;
            lengthParsed += bytesParsed;
        }
        tempVal.dt = tree->keyType;
        tempVal.v.intV = -1;
        nodeEntries [nodeSize] = tempVal;
        switch(nodeType){
            case 0:
                //non-leaf node
                for (i=0; i < (nodeSize + 1); i++){
                    sscanf(h->data + lengthParsed, "%i;%n", &nextNodePointers [i], &bytesParsed);
                    lengthParsed += bytesParsed;
                }
                for (i=0; i < nodeSize; i++){
                    
                    valueEquals(key, &nodeEntries [i], compVal);
                    if (compVal->v.boolV == true){
                        pageRequested = nextNodePointers [i+1];
                        i = nodeSize;
                    }else{
                        valueSmaller(key, &nodeEntries [i], compVal);
                        valueSmaller(key, &nodeEntries [i+1], compVal2);
                        if (compVal->v.boolV == true){
                            pageRequested = nextNodePointers [i];
                            i = nodeSize;
                        }else{
                            if (compVal->v.boolV == false){
                                if (compVal2->v.boolV == true){
                                    pageRequested = nextNodePointers [i+1];
                                    i = nodeSize;
                                }else{
                                    if ((compVal2->v.boolV == false) && (nodeEntries [i+1].v.intV == -1)){
                                        pageRequested = nextNodePointers [i+1];
                                        i = nodeSize;
                                    }   
                                }
                            }
                        }    
                    }
                }
                break;
            case 1:
                //leaf node
                breakLoopInd = 1;
                for (i=0; i < nodeSize; i++){
                    sscanf(h->data + lengthParsed, "%i;%i;%n", &tempRID.page, &tempRID.slot, &bytesParsed);
                    lengthParsed += bytesParsed;
                    recordPointers[i] = tempRID;
                }
                sscanf(h->data + lengthParsed, "%i;%n", &nextLeafNodePointer, &bytesParsed);
                lengthParsed += bytesParsed;
                for (i=0; i < nodeSize; i++){
                    valueEquals(key, &nodeEntries [i], compVal);
                    if (compVal->v.boolV == true){
                        recordFound = true;
                        breakLoopInd = 1;
                        *result = recordPointers [i];
                    }
                }
                break;
        }
        free(nodeEntries);
        free(nextNodePointers);
        free(recordPointers);
        
    }while(breakLoopInd == 0);
    
    free(compVal);
    free(compVal2);
    free(h);
    
    if (recordFound == true){
        return RC_OK;
    }else{
        return RC_IM_KEY_NOT_FOUND;
    }
}

RC insertKey (BTreeHandle *tree, Value *key, RID rid){
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    h->data = (char *) malloc(PAGE_SIZE * sizeof(char));
    BM_PageHandle *newLeaf = MAKE_PAGE_HANDLE();
    newLeaf->data = (char *) malloc(PAGE_SIZE * sizeof(char));
    int nodeSize, numberOfEntries, numberOfNodes, numberOfNodeEntries, curLastPage, rootPageNum;
    int nodeType, nextLeafNodePointer, splitpos;
    PageNumber leftChild, rightChild;
    int bytesParsed, lengthParsed, pageRequested;
    int i, j, breakLoopInd;
    int tempNextLeafPointer;
    bool recordFound, recordInsert, insertIntoParentInd;
    Value *compVal = (Value *) malloc (sizeof(Value));
    Value *compVal2 = (Value *) malloc (sizeof(Value));
    Value *newLeafKey = (Value *) malloc (sizeof(Value));
    Value tempVal;
    RID tempRID;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    curLastPage = btm->curLastPage;
    bm = btm->bTreeBM;
    rootPageNum = btm->rootPageNum;
    numberOfEntries = btm->numberOfEntries;
    numberOfNodes = btm->numberOfNodes;
    if ((rootPageNum == -1) || (numberOfEntries == 0)){
        pageRequested = 1;
    }else{
        pageRequested = rootPageNum;
    }
    recordFound = false;
    recordInsert = false; 
    insertIntoParentInd = false;
    breakLoopInd = 0;
    
    if (numberOfEntries == 0){
        curLastPage++;
        lengthParsed = 0;
        bytesParsed = 0;
        nodeType = 1;
        numberOfNodeEntries = 1;
        CHECK(pinPage(bm, h, pageRequested));
        bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
        lengthParsed += bytesParsed;
        for (i=0; i < nodeSize; i++){
            if (i == 0){
                tempVal = *key;
            }else{
                tempVal.dt = tree->keyType;
                tempVal.v.intV = -1;
            }
            bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
            lengthParsed += bytesParsed;
        }
        for (i=0; i < nodeSize; i++){
            if (i==0){
                tempRID  = rid;
            }else{
                tempRID.page = -1;
                tempRID.slot = -1;
            }
            bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
            lengthParsed += bytesParsed;
        }
        
        nextLeafNodePointer = -1;
        bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextLeafNodePointer);
        lengthParsed += bytesParsed;
        //printf("Printing while writing first node\n");
        //printf("%s\n", h->data);
        CHECK(markDirty(bm,h));
        CHECK(unpinPage(bm,h));
        numberOfEntries = 1;
        numberOfNodes = 1;
        recordInsert = true;
    }else{
        do{
            lengthParsed = 0;
            bytesParsed = 0;
            CHECK(pinPage(bm, h, pageRequested));
            sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
            lengthParsed = bytesParsed;
    
            Value *nodeEntries = malloc(sizeof(Value) * (nodeSize + 1));
            int *nextNodePointers = malloc(sizeof(int) * (nodeSize + 2));
            RID *recordPointers = malloc(sizeof(RID) * (nodeSize + 1));
    
            for(i=0; i < nodeSize; i++){
                sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
                nodeEntries [i] = tempVal;
                lengthParsed += bytesParsed;
            }
            tempVal.dt = tree->keyType;
            tempVal.v.intV = -1;
            nodeEntries [nodeSize] = tempVal;
            switch(nodeType){
                case 0:
                    //non-leaf node
                    for (i=0; i < (nodeSize + 1); i++){
                        sscanf(h->data + lengthParsed, "%i;%n", &nextNodePointers [i], &bytesParsed);
                        lengthParsed += bytesParsed;
                    }
                    CHECK(unpinPage(bm,h));
                
                    for (i=0; i < nodeSize; i++){
                        valueEquals(key, &nodeEntries [i], compVal);
                        if (compVal->v.boolV == true){
                            pageRequested = nextNodePointers [i+1];
                            i = nodeSize;
                        }else{
                            valueSmaller(key, &nodeEntries [i], compVal);
                            valueSmaller(key, &nodeEntries [i+1], compVal2);
                            if (compVal->v.boolV == true){
                                pageRequested = nextNodePointers [i];
                                i = nodeSize;
                            }else{
                                if (compVal->v.boolV == false){
                                    if (compVal2->v.boolV == true){
                                        pageRequested = nextNodePointers [i+1];
                                        i = nodeSize;
                                    }else{
                                        if ((compVal2->v.boolV == false) && (nodeEntries [i+1].v.intV == -1)){
                                            pageRequested = nextNodePointers [i+1];
                                            i = nodeSize;
                                        }   
                                    }
                                }
                            }    
                        }
                    }
                    break;
                case 1:
                    //leaf node
                    for (i=0; i < nodeSize; i++){
                        sscanf(h->data + lengthParsed, "%i;%i;%n", &tempRID.page, &tempRID.slot, &bytesParsed);
                        lengthParsed += bytesParsed;
                        recordPointers [i] = tempRID;
                    }
                    tempRID.page = -1;
                    tempRID.slot = -1;
                    recordPointers [nodeSize] = tempRID;
                    sscanf(h->data + lengthParsed, "%i;%n", &nextLeafNodePointer, &bytesParsed);
                    lengthParsed += bytesParsed;
                    
                    for (i=0; i < nodeSize + 1; i++){
                        valueEquals(key, &nodeEntries [i], compVal);
                        if (compVal->v.boolV == true){
                            recordFound = true;
                            breakLoopInd = 1;
                        }else{
                            valueSmaller(key, &nodeEntries [i], compVal);
                            if((compVal->v.boolV == true) || (nodeEntries [i].v.intV == -1)){
                                for (j = nodeSize; j > i; j--){
                                    recordPointers [j] = recordPointers [j-1];
                                    nodeEntries [j] = nodeEntries [j-1];
                                }
                                nodeEntries [i] = *key;
                                recordPointers [i] = rid;
                                numberOfNodeEntries++;
                                recordInsert = true;
                                breakLoopInd = 1;
                            }
                        }
                        if (breakLoopInd == 1){
                            i = nodeSize + 1;
                        }
                    }
                    
                    if (recordInsert == true){
                        numberOfEntries++;
                        // Condition to check if the node is overflowing
                        if (numberOfNodeEntries <= nodeSize){
                            lengthParsed = 0;
                            bytesParsed = 0;
                            bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
                            lengthParsed += bytesParsed;
                            for (i=0; i < nodeSize; i++){
                                tempVal = nodeEntries [i];
                                bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                                lengthParsed += bytesParsed;
                            }
                            for (i=0; i < nodeSize; i++){
                                tempRID  = recordPointers [i];
                                bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
                                lengthParsed += bytesParsed;
                            }
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextLeafNodePointer);
                            lengthParsed += bytesParsed;
                            CHECK(markDirty(bm,h));
                            CHECK(unpinPage(bm,h));
                            //printf("No Leaf Split\n");
                            //printf("Leaf Data\n");
                            //printf("%s\n", h->data);
                        }else{
                            //node is overflowing
                            curLastPage++;
                            if ((nodeSize % 2) == 0){
                                splitpos = (nodeSize / 2) + 1;
                            }else{
                                splitpos = (nodeSize + 1) / 2;
                            }
                            lengthParsed = 0;
                            bytesParsed = 0;
                            bytesParsed = sprintf(h->data, "%i;%i;", nodeType, splitpos);
                            lengthParsed += bytesParsed;
                            for (i=0; i < nodeSize; i++){
                                if (i < splitpos){
                                    tempVal = nodeEntries [i];
                                }else{
                                    tempVal.dt = tree->keyType;
                                    tempVal.v.intV = -1;
                                }
                                bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                                lengthParsed += bytesParsed;
                            }
                            for (i=0; i < nodeSize; i++){
                                if (i < splitpos){
                                    tempRID = recordPointers [i];
                                }else{
                                    tempRID.page = -1;
                                    tempRID.slot = -1;
                                }
                                bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
                                lengthParsed += bytesParsed;
                            }
                            tempNextLeafPointer = nextLeafNodePointer;
                            nextLeafNodePointer = curLastPage;
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextLeafNodePointer);
                            lengthParsed += bytesParsed;
                            //printf("Leaf Split\n");
                            //printf("Old Leaf Data\n");
                            //printf("%s\n", h->data);
                            CHECK(markDirty(bm,h));
                            CHECK(unpinPage(bm,h));
                            CHECK(pinPage(bm, newLeaf, curLastPage));
                        
                            lengthParsed = 0;
                            bytesParsed = 0;
                            j = splitpos;
                            bytesParsed = sprintf(newLeaf->data, "%i;%i;", nodeType, splitpos - 1);
                            lengthParsed += bytesParsed;
                            for (i=0; i < nodeSize; i++){
                                if (j < (nodeSize + 1)){
                                    tempVal = nodeEntries [j];
                                    j++;
                                }else{
                                    tempVal.dt = tree->keyType;
                                    tempVal.v.intV = -1;
                                }
                                bytesParsed = sprintf(newLeaf->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                                lengthParsed += bytesParsed;
                            }
                            j = splitpos;
                            for (i=0; i < nodeSize; i++){
                                if (j < (nodeSize + 1)){
                                    tempRID = recordPointers [j];
                                    j++;
                                }else{
                                    tempRID.page = -1;
                                    tempRID.slot = -1;
                                }
                                bytesParsed = sprintf(newLeaf->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
                                lengthParsed += bytesParsed;
                            }
                            nextLeafNodePointer = tempNextLeafPointer;
                            bytesParsed = sprintf(newLeaf->data + lengthParsed, "%i;", nextLeafNodePointer);
                            lengthParsed += bytesParsed;
                            //printf("New Leaf Data\n");
                            //printf("%s\n", newLeaf->data);
                            
                            *newLeafKey = nodeEntries [splitpos];
                            CHECK(markDirty(bm,newLeaf));
                            CHECK(unpinPage(bm,newLeaf));
                            leftChild = h->pageNum;
                            rightChild = newLeaf->pageNum;
                            insertIntoParentInd = true;
                            numberOfNodes++;
                        }
                    }else{
                        CHECK(unpinPage(bm,h));
                    }
                    break;
            }
            free(nodeEntries);
            free(nextNodePointers);
            free(recordPointers);
        
        }while(breakLoopInd == 0);
    }

    btm->curLastPage = curLastPage;
    btm->numberOfEntries = numberOfEntries;
    btm->numberOfNodes = numberOfNodes;

    if (insertIntoParentInd == true){
        insertIntoParent(tree, newLeafKey, leftChild, rightChild);
    }
    
    free(compVal);
    free(compVal2);
    free(newLeafKey);
    free(h);
    
    if (recordFound == true){
        return RC_IM_KEY_ALREADY_EXISTS;
    }else{
        if (recordInsert == true){
            return RC_OK;
        }
    }
}

RC insertIntoParent (BTreeHandle *tree, Value *key, PageNumber leftChild, PageNumber rightChild){
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_PageHandle *newNonLeaf = MAKE_PAGE_HANDLE();
    int nodeSize, numberOfNodeEntries, numberOfNodes, curLastPage, rootPageNum;
    int nodeType, splitpos, insertpos;
    int bytesParsed, lengthParsed, pageRequested;
    int i, j, breakLoopInd, insertIntoThisNode, insertIntoParentInd;
    int tempNodePointer;
    PageNumber leftNonLeafChild, rightNonLeafChild;
    bool recordInsert;
    Value *compVal = (Value *) malloc (sizeof(Value));
    Value *compVal2 = (Value *) malloc (sizeof(Value));
    Value *newNonLeafKey = (Value *) malloc (sizeof(Value));
    Value tempVal;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    curLastPage = btm->curLastPage;
    bm = btm->bTreeBM;
    rootPageNum = btm->rootPageNum;
    numberOfNodes = btm->numberOfNodes;
    if (rootPageNum == -1){
        pageRequested = 1;
    }else{
        pageRequested = rootPageNum;
    }
    recordInsert = false; 
    breakLoopInd = 0;
    insertIntoParentInd = 0;

    Value *nodeEntries = (Value *) malloc(sizeof(Value) * (nodeSize + 1));
    int *nextNodePointers = (int *) malloc(sizeof(int) * (nodeSize + 2));

    if (rootPageNum == -1){
        lengthParsed = 0;
        bytesParsed = 0;
        nodeType = 0;
        numberOfNodeEntries = 1;
        curLastPage++;
        CHECK(pinPage(bm, h, curLastPage));
        bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
        lengthParsed += bytesParsed;
        for (i=0; i < nodeSize; i++){
            if (i == 0){
                tempVal = *key;
            }else{
                tempVal.dt = tree->keyType;
                tempVal.v.intV = -1;
            }
            bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
            lengthParsed += bytesParsed;
        }
        for (i=0; i < nodeSize + 1; i++){
            if(i == 0){
                tempNodePointer = leftChild;
            }else{
                if (i == 1){
                    tempNodePointer = rightChild;
                }else{
                    tempNodePointer = -1;
                }
            }
            bytesParsed = sprintf(h->data + lengthParsed, "%i;", tempNodePointer);
            lengthParsed += bytesParsed;
        }
        CHECK(markDirty(bm,h));
        CHECK(unpinPage(bm,h));
        //printf("New Root Data\n");
        //printf("%s\n", h->data);
        recordInsert = true;
        rootPageNum = curLastPage;
        numberOfNodes++;
    }else{
        do{
            insertIntoThisNode = 0;
            lengthParsed = 0;
            bytesParsed = 0;
            CHECK(pinPage(bm, h, pageRequested));
            sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
            lengthParsed = bytesParsed;
            if (nodeType == 0){
                for(i=0; i < nodeSize; i++){
                    sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
                    nodeEntries [i] = tempVal;
                    lengthParsed += bytesParsed;
                }
                tempVal.dt = tree->keyType;
                tempVal.v.intV = -1;
                nodeEntries [nodeSize] = tempVal;
                for(i=0; i < nodeSize + 1; i++){
                    sscanf(h->data + lengthParsed, "%i;%n", &tempNodePointer, &bytesParsed);
                    lengthParsed += bytesParsed;
                    nextNodePointers [i] = tempNodePointer;
                }
                nextNodePointers [nodeSize + 1] = -1;
                
                for (i=0; i < nodeSize; i++){
                    
                    valueEquals(key, &nodeEntries [i], compVal);
                    if (compVal->v.boolV == true){
                        //pageRequested = nextNodePointers [i+1];
                        if (nextNodePointers [i] == leftChild){
                            insertIntoThisNode = 1;
                            insertpos = i;
                        }else{
                            pageRequested = nextNodePointers [i];
                        }
                        i = nodeSize;
                    }else{
                        valueSmaller(key, &nodeEntries [i], compVal);
                        valueSmaller(key, &nodeEntries [i+1], compVal2);
                        if (compVal->v.boolV == true){
                            if (nextNodePointers [i] == leftChild){
                                insertIntoThisNode = 1;
                                insertpos = i;
                            }else{
                                pageRequested = nextNodePointers [i];
                            }
                            i = nodeSize;
                        }else{
                            if (compVal->v.boolV == false){
                                if (compVal2->v.boolV == true){
                                    if (nextNodePointers [i+1] == leftChild){
                                        insertIntoThisNode = 1;
                                        insertpos = i + 1;
                                    }else{
                                        pageRequested = nextNodePointers [i+1];
                                    }
                                    i = nodeSize;
                                }else{
                                    if ((compVal2->v.boolV == false) && (nodeEntries [i+1].v.intV == -1)){
                                        if (nextNodePointers [i+1] == leftChild){
                                            insertIntoThisNode = 1;
                                            insertpos = i + 1;
                                        }else{
                                            pageRequested = nextNodePointers [i+1];
                                        }
                                        i = nodeSize;
                                    }   
                                }
                            }
                        }    
                    }
                }
                if (insertIntoThisNode == 1){
                    tempVal = *key;
                    breakLoopInd = 1;
                    for (i=nodeSize; i > insertpos; i--){
                        nodeEntries [i] = nodeEntries [i-1];
                    }
                    nodeEntries [insertpos] = tempVal;
                    
                    for (i=nodeSize + 1; i > insertpos + 1; i--){
                        nextNodePointers [i] = nextNodePointers [i-1];
                    }
                    nextNodePointers [insertpos + 1] = rightChild;
                    numberOfNodeEntries++;
                    
                    if (numberOfNodeEntries <= nodeSize){
                        //Insert into parent successful
                        lengthParsed = 0;
                        bytesParsed = 0;
                        bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
                        lengthParsed += bytesParsed;
                        for (i=0; i < nodeSize; i++){
                            tempVal = nodeEntries [i];
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                            lengthParsed += bytesParsed;
                        }
                        for (i=0; i < nodeSize + 1; i++){
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextNodePointers [i]);
                            lengthParsed += bytesParsed;
                        }
                        CHECK(markDirty(bm,h));
                        //printf("No Non-Leaf Split\n");
                        //printf("Non-leaf Data\n");
                        //printf("%s\n", h->data);
                        recordInsert = true;
                    }else{
                        //Node overflows. We need to split and insert middle value into its parent
                        curLastPage++;
                        if (nodeSize % 2 == 0){
                            splitpos = (nodeSize / 2);
                        }else{
                            splitpos = ((nodeSize + 1) / 2);
                        }

                        lengthParsed = 0;
                        bytesParsed = 0;
                        bytesParsed = sprintf(h->data, "%i;%i;", nodeType, splitpos);
                        lengthParsed += bytesParsed;
                        for (i=0; i < nodeSize; i++){
                            if (i < splitpos){
                                tempVal = nodeEntries [i];
                            }else{
                                tempVal.dt = tree->keyType;
                                tempVal.v.intV = -1;
                            }
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                            lengthParsed += bytesParsed;
                        }
                        for (i=0; i < nodeSize + 1; i++){
                            if (i < splitpos + 1){
                                tempNodePointer = nextNodePointers [i];
                            }else{
                                tempNodePointer = -1;
                            }
                            bytesParsed = sprintf(h->data + lengthParsed, "%i;", tempNodePointer);
                            lengthParsed += bytesParsed;
                        }
                        CHECK(markDirty(bm,h));
                        //printf("Non-Leaf Split\n");
                        //printf("Old Non-Leaf Data\n");
                        //printf("%s\n", h->data);

                        CHECK(pinPage(bm, newNonLeaf, curLastPage));
                        lengthParsed = 0;
                        bytesParsed = 0;
                        j = splitpos + 1;
                        if ((nodeSize % 2) == 0){
                            bytesParsed = sprintf(newNonLeaf->data, "%i;%i;", nodeType, splitpos);
                        }else{
                            bytesParsed = sprintf(newNonLeaf->data, "%i;%i;", nodeType, splitpos - 1);
                        }
                        lengthParsed += bytesParsed;
                        for (i=0; i < nodeSize; i++){
                            if (j < (nodeSize + 1)){
                                tempVal = nodeEntries [j];
                                j++;
                            }else{
                                tempVal.dt = tree->keyType;
                                tempVal.v.intV = -1;
                            }
                            bytesParsed = sprintf(newNonLeaf->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                            lengthParsed += bytesParsed;
                        }
                        j = splitpos + 1;
                        for (i=0; i < nodeSize + 1; i++){
                            if (j < (nodeSize + 2)){
                                tempNodePointer = nextNodePointers [j];
                                j++;
                            }else{
                                tempNodePointer = -1;
                            }
                            bytesParsed = sprintf(newNonLeaf->data + lengthParsed, "%i;", tempNodePointer);
                            lengthParsed += bytesParsed;
                        }
                        CHECK(markDirty(bm,newNonLeaf));
                        CHECK(unpinPage(bm,newNonLeaf));
                        //printf("New Non-Leaf Data\n");
                        //printf("%s\n", newNonLeaf->data);

                        numberOfNodes++;
                        recordInsert = true;
                        insertIntoParentInd = 1;
                        *newNonLeafKey = nodeEntries [splitpos];
                        // Incase when we are splitting the root
                        leftNonLeafChild = h->pageNum;
                        rightNonLeafChild = newNonLeaf->pageNum;
                        if (rootPageNum == h->pageNum){
                            rootPageNum = -1;
                        }
                    }
                }
                CHECK(unpinPage(bm,h));
            }
        }while(breakLoopInd == 0);
    }
    
    btm->curLastPage = curLastPage;
    btm->numberOfNodes = numberOfNodes;
    btm->rootPageNum = rootPageNum;

    free(nodeEntries);
    free(nextNodePointers);
    free(compVal);
    free(compVal2);
    free(h);
    
    if (insertIntoParentInd == 1){
        insertIntoParent(tree, newNonLeafKey, leftNonLeafChild ,rightNonLeafChild);
    }

    free(newNonLeafKey);
    
    if (recordInsert == true){
            return RC_OK;
    }
}

RC deleteKey (BTreeHandle *tree, Value *key){
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    h->data = (char *) malloc(PAGE_SIZE * sizeof(char));
    int minNodeSize, nodeSize, numberOfEntries, numberOfNodes, numberOfNodeEntries;
    int curLastPage, rootPageNum;
    int nodeType, nextLeafNodePointer, deletepos, splitpos;
    PageNumber childPageNumber, nextLeafPageNumber;
    int bytesParsed, lengthParsed, pageRequested;
    int i, j, breakLoopInd;
    int tempNextLeafPointer;
    bool recordFound, recordInsert, deleteFromParentInd;
    Value *compVal = (Value *) malloc (sizeof(Value));
    Value *compVal2 = (Value *) malloc (sizeof(Value));
    Value *newLeafKey = (Value *) malloc (sizeof(Value));
    Value tempVal;
    RID tempRID;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    curLastPage = btm->curLastPage;
    bm = btm->bTreeBM;
    rootPageNum = btm->rootPageNum;
    numberOfEntries = btm->numberOfEntries;
    numberOfNodes = btm->numberOfNodes;
    if ((rootPageNum == -1) || (numberOfEntries == 0)){
        pageRequested = 1;
    }else{
        pageRequested = rootPageNum;
    }
    recordFound = false;
    deleteFromParentInd = false;
    breakLoopInd = 0;

    do{
        lengthParsed = 0;
        bytesParsed = 0;
        CHECK(pinPage(bm, h, pageRequested));
        sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
        lengthParsed = bytesParsed;
    
        Value *nodeEntries = malloc(sizeof(Value) * (nodeSize + 1));
        int *nextNodePointers = malloc(sizeof(int) * (nodeSize + 2));
        RID *recordPointers = malloc(sizeof(RID) * (nodeSize + 1));
    
        for(i=0; i < nodeSize; i++){
            sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
            nodeEntries [i] = tempVal;
            lengthParsed += bytesParsed;
            }
        tempVal.dt = tree->keyType;
        tempVal.v.intV = -1;
        nodeEntries [nodeSize] = tempVal;
        switch(nodeType){
            case 0:
                //non-leaf node
                for (i=0; i < (nodeSize + 1); i++){
                    sscanf(h->data + lengthParsed, "%i;%n", &nextNodePointers [i], &bytesParsed);
                    lengthParsed += bytesParsed;
                }
                CHECK(unpinPage(bm,h));
                
                for (i=0; i < nodeSize; i++){
                    valueEquals(key, &nodeEntries [i], compVal);
                    if (compVal->v.boolV == true){
                        pageRequested = nextNodePointers [i+1];
                        i = nodeSize;
                    }else{
                        valueSmaller(key, &nodeEntries [i], compVal);
                        valueSmaller(key, &nodeEntries [i+1], compVal2);
                        if (compVal->v.boolV == true){
                            pageRequested = nextNodePointers [i];
                            i = nodeSize;
                        }else{
                            if (compVal->v.boolV == false){
                                if (compVal2->v.boolV == true){
                                    pageRequested = nextNodePointers [i+1];
                                    i = nodeSize;
                                }else{
                                    if ((compVal2->v.boolV == false) && (nodeEntries [i+1].v.intV == -1)){
                                        pageRequested = nextNodePointers [i+1];
                                        i = nodeSize;
                                    }   
                                }
                            }
                        }    
                    }
                }
                break;
            case 1:
                for (i=0; i < nodeSize; i++){
                    sscanf(h->data + lengthParsed, "%i;%i;%n", &tempRID.page, &tempRID.slot, &bytesParsed);
                    lengthParsed += bytesParsed;
                    recordPointers [i] = tempRID;
                }
                tempRID.page = -1;
                tempRID.slot = -1;
                recordPointers [nodeSize] = tempRID;
                sscanf(h->data + lengthParsed, "%i;%n", &nextLeafNodePointer, &bytesParsed);
                lengthParsed += bytesParsed;
                    
                for (i=0; i < nodeSize; i++){
                    valueEquals(key, &nodeEntries [i], compVal);
                    if (compVal->v.boolV == true){
                        recordFound = true;
                        breakLoopInd = 1;
                        deletepos = i;
                        i = nodeSize;
                    }
                }
                
                if (recordFound == true){
                    numberOfEntries--;
                    numberOfNodeEntries--;
                    for (i=deletepos; i < nodeSize; i++){
                        nodeEntries [i] = nodeEntries[i+1];
                        recordPointers [i] = recordPointers [i+1];
                    }
                    
                    lengthParsed = 0;
                    bytesParsed = 0;
                    bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
                    lengthParsed += bytesParsed;
                    for (i=0; i < nodeSize; i++){
                        tempVal = nodeEntries [i];
                        bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                        lengthParsed += bytesParsed;
                    }
                    for (i=0; i < nodeSize; i++){
                        tempRID  = recordPointers [i];
                        bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
                        lengthParsed += bytesParsed;
                    }
                    bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextLeafNodePointer);
                    lengthParsed += bytesParsed;
                    CHECK(markDirty(bm,h));
                    CHECK(unpinPage(bm,h));        
                    
                    if (nodeSize % 2){
                        minNodeSize = nodeSize / 2;
                    }else{
                        minNodeSize = (nodeSize + 1) / 2;
                    }
                    
                    if (numberOfNodeEntries == 0){
                        //Node has no more elements
                        numberOfNodes--;
                        if (rootPageNum != -1){
                            deleteFromParentInd = true;
                            childPageNumber = h->pageNum;
                            nextLeafPageNumber = nextLeafNodePointer;
                        }
                    }else{
                        if (numberOfNodeEntries < minNodeSize){
                            //Node underflows
                        }
                    }
                }
                break;
        }
        free(nodeEntries);
        free(nextNodePointers);
        free(recordPointers);
        
    }while(breakLoopInd == 0);

    btm->curLastPage = curLastPage;
    btm->numberOfEntries = numberOfEntries;
    btm->numberOfNodes = numberOfNodes;

    if (deleteFromParentInd == true){
        updateNextLeafPtr(tree, childPageNumber, nextLeafPageNumber);
        deleteFromParent(tree, key, childPageNumber);
    }
    
    free(compVal);
    free(compVal2);
    free(newLeafKey);
    free(h);

    
    return RC_OK;
}

RC updateNextLeafPtr(BTreeHandle *tree, PageNumber childPageNumber, PageNumber nextLeafPageNumber){
    
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    h->data = (char *) malloc(PAGE_SIZE * sizeof(char));
    int nodeSize, nodeType, numberOfNodeEntries, nextLeafNodePointer;
    int bytesParsed, lengthParsed, pageRequested;
    int i, j, breakLoopInd;
    Value tempVal;
    RID tempRID;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    bm = btm->bTreeBM;
    pageRequested = 1;
    
    Value *nodeEntries = malloc(sizeof(Value) * (nodeSize + 1));
    RID *recordPointers = malloc(sizeof(RID) * (nodeSize + 1));

    do{
        breakLoopInd = 0;
        lengthParsed = 0;
        bytesParsed = 0;
        CHECK(pinPage(bm, h, pageRequested));
        sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
        lengthParsed = bytesParsed;
    
        for(i=0; i < nodeSize; i++){
            sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
            nodeEntries [i] = tempVal;
            lengthParsed += bytesParsed;
        }
        tempVal.dt = tree->keyType;
        tempVal.v.intV = -1;
        nodeEntries [nodeSize] = tempVal;

        if(nodeType == 1){
            for (i=0; i < nodeSize; i++){
                sscanf(h->data + lengthParsed, "%i;%i;%n", &tempRID.page, &tempRID.slot, &bytesParsed);
                lengthParsed += bytesParsed;
                recordPointers [i] = tempRID;
            }
            tempRID.page = -1;
            tempRID.slot = -1;
            recordPointers [nodeSize] = tempRID;
            sscanf(h->data + lengthParsed, "%i;%n", &nextLeafNodePointer, &bytesParsed);
            lengthParsed += bytesParsed;
            
            if (nextLeafNodePointer == childPageNumber){
                breakLoopInd = 1;
                nextLeafNodePointer = nextLeafPageNumber;
                
                lengthParsed = 0;
                bytesParsed = 0;
                bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
                lengthParsed += bytesParsed;
                for (i=0; i < nodeSize; i++){
                    tempVal = nodeEntries [i];
                    bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                    lengthParsed += bytesParsed;
                }
                for (i=0; i < nodeSize; i++){
                    tempRID  = recordPointers [i];
                    bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempRID.page, tempRID.slot);
                    lengthParsed += bytesParsed;
                }
                bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextLeafNodePointer);
                lengthParsed += bytesParsed;
                CHECK(markDirty(bm,h));
            }else{
                if (nextLeafNodePointer == -1){
                    breakLoopInd = 1;
                }else{
                    pageRequested = nextLeafNodePointer;
                }
            }
        }
        CHECK(unpinPage(bm,h));
    }while(breakLoopInd == 0);

    free(nodeEntries);
    free(recordPointers);
    free(h);
    return RC_OK;
}

RC deleteFromParent(BTreeHandle *tree, Value *key, PageNumber childPageNumber){
    
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_PageHandle *newNonLeaf = MAKE_PAGE_HANDLE();
    int nodeSize, minNodeSize, numberOfNodeEntries, numberOfNodes, curLastPage, rootPageNum;
    int nodeType, splitpos, deletepos;
    int bytesParsed, lengthParsed, pageRequested;
    int i, j, breakLoopInd, insertIntoParentInd;
    int tempNodePointer;
    PageNumber leftNonLeafChild, rightNonLeafChild;
    bool recordInsert, deleteFromThisNode, deleteFromParentInd;
    Value *compVal = (Value *) malloc (sizeof(Value));
    Value *compVal2 = (Value *) malloc (sizeof(Value));
    Value tempVal, deletedKey;

    btm = tree->mgmtData;
    nodeSize = btm->nodeSize;
    curLastPage = btm->curLastPage;
    bm = btm->bTreeBM;
    rootPageNum = btm->rootPageNum;
    numberOfNodes = btm->numberOfNodes;
    if (rootPageNum == -1){
        pageRequested = 1;
    }else{
        pageRequested = rootPageNum;
    }
    recordInsert = false; 
    breakLoopInd = 0;
    insertIntoParentInd = 0;

    Value *nodeEntries = (Value *) malloc(sizeof(Value) * (nodeSize + 1));
    int *nextNodePointers = (int *) malloc(sizeof(int) * (nodeSize + 2));

    do{
        deleteFromThisNode = false;
        lengthParsed = 0;
        bytesParsed = 0;
        CHECK(pinPage(bm, h, pageRequested));
        sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
        lengthParsed = bytesParsed;
        if (nodeType == 0){
            for(i=0; i < nodeSize; i++){
                sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
                nodeEntries [i] = tempVal;
                lengthParsed += bytesParsed;
            }
            tempVal.dt = tree->keyType;
            tempVal.v.intV = -1;
            nodeEntries [nodeSize] = tempVal;
            for(i=0; i < nodeSize + 1; i++){
                sscanf(h->data + lengthParsed, "%i;%n", &tempNodePointer, &bytesParsed);
                lengthParsed += bytesParsed;
                nextNodePointers [i] = tempNodePointer;
            }
            nextNodePointers [nodeSize + 1] = -1;
             
            for (i=0; i < nodeSize; i++){
                    
                valueEquals(key, &nodeEntries [i], compVal);
                if (compVal->v.boolV == true){
                    if (nextNodePointers [i] == childPageNumber){
                        deleteFromThisNode = true;
                        deletepos = i;
                    }else{
                        pageRequested = nextNodePointers [i];
                    }
                    i = nodeSize;
                }else{
                    valueSmaller(key, &nodeEntries [i], compVal);
                    valueSmaller(key, &nodeEntries [i+1], compVal2);
                    if (compVal->v.boolV == true){
                        if (nextNodePointers [i] == childPageNumber){
                            deleteFromThisNode = true;
                            deletepos = i;
                        }else{
                            pageRequested = nextNodePointers [i];
                        }
                        i = nodeSize;
                    }else{
                        if (compVal->v.boolV == false){
                            if (compVal2->v.boolV == true){
                                if (nextNodePointers [i+1] == childPageNumber){
                                    deleteFromThisNode = true;
                                    deletepos = i;
                                }else{
                                    pageRequested = nextNodePointers [i+1];
                                }
                                i = nodeSize;
                            }else{
                                if ((compVal2->v.boolV == false) && (nodeEntries [i+1].v.intV == -1)){
                                    if (nextNodePointers [i+1] == childPageNumber){
                                        deleteFromThisNode = true;
                                        deletepos = i;
                                    }else{
                                        pageRequested = nextNodePointers [i+1];
                                    }
                                    i = nodeSize;
                                }   
                            }
                        }
                    }    
                }
            }
            
            if (deleteFromThisNode == true){
                breakLoopInd= 1;
                deletedKey = nodeEntries [deletepos];
                for (i=deletepos; i<nodeSize; i++){
                    nodeEntries[i] = nodeEntries[i+1];
                }
                for (i=deletepos; i<nodeSize + 1; i++){
                    nextNodePointers[i] = nextNodePointers[i+1];
                }
                numberOfNodeEntries--;
                
                lengthParsed = 0;
                bytesParsed = 0;
                bytesParsed = sprintf(h->data, "%i;%i;", nodeType, numberOfNodeEntries);
                lengthParsed += bytesParsed;
                for (i=0; i < nodeSize; i++){
                    tempVal = nodeEntries [i];
                    bytesParsed = sprintf(h->data + lengthParsed, "%i;%i;", tempVal.dt, tempVal.v.intV);
                    lengthParsed += bytesParsed;
                }
                for (i=0; i < nodeSize + 1; i++){
                    bytesParsed = sprintf(h->data + lengthParsed, "%i;", nextNodePointers [i]);
                    lengthParsed += bytesParsed;
                }
                CHECK(markDirty(bm,h));

                if (nodeSize % 2 == 0){
                    minNodeSize = nodeSize / 2;
                }else{
                    minNodeSize = (nodeSize - 1) / 2;
                }
                
                if (numberOfNodeEntries == 0){
                    numberOfNodes--;
                    if (rootPageNum == h->pageNum){
                        rootPageNum = -1;
                    }/*else{
                        deleteFromParentInd = true;
                        curPageNum = h->pageNum;
                    }*/
                }else{
                    if (numberOfNodeEntries < minNodeSize){
                        //Node underflows
                    }
                }
            }
        }
        CHECK(unpinPage(bm,h));
    }while(breakLoopInd == 0);

    if (deleteFromParentInd == true){
        deleteFromParent(tree, key, childPageNumber);
    }
    
    return RC_OK;
}

RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle){
    BT_ScanHandle *bts = (BT_ScanHandle *) malloc (sizeof(BT_ScanHandle));
    BT_ScanHandleMgmtInfo *btsm = (BT_ScanHandleMgmtInfo *) malloc (sizeof(BT_ScanHandleMgmtInfo));
    Value val;
    PageNumber page;
    val.dt = tree->keyType;
    val.v.intV = -1;
    page = -1;
    
    btsm->lastValuePrinted = val;
    btsm->lastVisitedPage = page;
    bts->tree = tree;
    bts->mgmtData = btsm;
    
    *handle = bts;
    return RC_OK;
}

RC nextEntry (BT_ScanHandle *handle, RID *result){
    BT_ScanHandleMgmtInfo *btsm;
    BTreeHandle *tree;
    BTreeMgmtInfo *btm;
    BM_BufferPool *bm;
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    PageNumber pageRequested, nextLeafNodePointer;
    bool nextEntryFound, breakLoopInd;
    int nodeSize, nodeType, numberOfNodeEntries;
    int i, lengthParsed, bytesParsed;
    Value *compVal = (Value *) malloc(sizeof(Value));
    Value tempVal;
    RID tempRID;

    btsm = handle->mgmtData;
    tree = handle->tree;
    btm = tree->mgmtData;
    bm = btm->bTreeBM;
    nodeSize = btm->nodeSize;
    
    Value *nodeEntries = malloc(sizeof(Value) * nodeSize);
    int *nextNodePointers = malloc(sizeof(int) * (nodeSize + 1));
    RID *recordPointers = malloc(sizeof(RID) * nodeSize);
    
    if (btsm->lastVisitedPage == -1){
        pageRequested = btm->rootPageNum;
    }else{
        pageRequested = btsm->lastVisitedPage;
    }

    do{
        lengthParsed = 0;
        bytesParsed = 0;
        nextEntryFound = false;
        breakLoopInd = false;
        CHECK(pinPage(bm, h, pageRequested));
        sscanf(h->data, "%i;%i;%n", &nodeType, &numberOfNodeEntries, &bytesParsed);
        CHECK(unpinPage(bm,h));
        lengthParsed = bytesParsed;
    
        for(i=0; i < nodeSize; i++){
            sscanf(h->data + lengthParsed, "%i;%i;%n", &tempVal.dt, &tempVal.v.intV, &bytesParsed);
            nodeEntries [i] = tempVal;
            lengthParsed += bytesParsed;
        }
    
        switch(nodeType){
            case 0:
                for (i=0; i < (nodeSize + 1); i++){
                    sscanf(h->data + lengthParsed, "%i;%n", &nextNodePointers [i], &bytesParsed);
                    lengthParsed += bytesParsed;
                }
                pageRequested = nextNodePointers [0];
                break;
            case 1:
                for (i=0; i < nodeSize; i++){
                    sscanf(h->data + lengthParsed, "%i;%i;%n", &tempRID.page, &tempRID.slot, &bytesParsed);
                    lengthParsed += bytesParsed;
                    recordPointers[i] = tempRID;
                }
                sscanf(h->data + lengthParsed, "%i;%n", &nextLeafNodePointer, &bytesParsed);
                lengthParsed += bytesParsed;
                
                if (btsm->lastVisitedPage == -1){
                    btsm->lastValuePrinted = nodeEntries[0];
                    btsm->lastVisitedPage = h->pageNum;
                    *result = recordPointers [0];
                    nextEntryFound = true;
                    breakLoopInd = true;
                }else{
                    for(i=0; i<nodeSize; i++){
                        valueSmaller(&nodeEntries [i], &btsm->lastValuePrinted, compVal);
                        if (compVal->v.boolV == false){
                            valueEquals(&nodeEntries [i], &btsm->lastValuePrinted, compVal);
                            if (compVal->v.boolV == false){
                                btsm->lastValuePrinted = nodeEntries [i];
                                btsm->lastVisitedPage = h->pageNum;
                                *result = recordPointers [i];
                                nextEntryFound = true;
                                breakLoopInd = true;
                                i = nodeSize;
                            }
                        }
                    }
                    
                    if (nextEntryFound == false){
                        if (nextLeafNodePointer == -1){
                            breakLoopInd = true;
                        }else{
                            pageRequested = nextLeafNodePointer;
                        }
                    }
                }
                break;
        }
    }while(breakLoopInd == false);
    
    free(nodeEntries);
    free(nextNodePointers);
    free(recordPointers);
    free(compVal);
    free(h);
    
    if (nextEntryFound == true){
        return RC_OK;
    }else{
        return RC_IM_NO_MORE_ENTRIES;
    }
}

RC closeTreeScan (BT_ScanHandle *handle){
    BT_ScanHandleMgmtInfo *btsm;
    btsm = handle->mgmtData;
    free(btsm);
    return RC_OK;
}

// debug and test functions
char *printTree (BTreeHandle *tree){
    return "Done";
}
