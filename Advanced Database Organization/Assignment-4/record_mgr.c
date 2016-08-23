#include "dberror.h"
#include "expr.h"
#include "tables.h"
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <string.h>

#define INT_STRING_LEN 10

#define DATA_STRING_LEN 25

RC initRecordManager (void *mgmtData){
    //BM_BufferPool *bm = MAKE_POOL();
    //CHECK(initBufferPool(bm, rel->name, 3, RS_FIFO, NULL));
    //mgmtData = bm;
    return RC_OK;
}

RC shutdownRecordManager (){
    return RC_OK;
}

RC createTable (char *name, Schema *schema){
    CHECK(createPageFile(name));
    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    int i, returnCode, lengthParsed, curLastPage;
    int numOfRecs, numOfDelRecs, nextDelRecPos, numOfDelRecsPossible;
    char dataString [DATA_STRING_LEN], schemaString [PAGE_SIZE];
    //char *intString = (char *) malloc (sizeof(int) + 1);
    curLastPage = 1;
    numOfRecs = 0;
    numOfDelRecs = 0;
    
    memset(schemaString, '\0', PAGE_SIZE);
    lengthParsed =0;
    
    //itoa(schema->numAttr, integerString, INT_STRING_LEN);
    memcpy (schemaString, &schema->numAttr, sizeof(int));
    lengthParsed += sizeof(int);
    
    for (i=0; i<schema->numAttr; i++){
        strncpy (dataString, schema->attrNames [i], DATA_STRING_LEN);
        //strcat (schemaString, dataString);
        memcpy(schemaString + lengthParsed, dataString, DATA_STRING_LEN);
        lengthParsed += DATA_STRING_LEN;
        
        //itoa(schema->dataTypes [i], integerString, INT_STRING_LEN);
        //memcpy(intString, schema->dataTypes [i], sizeof(int));
        //strcat (schemaString, intString);
        memcpy(schemaString + lengthParsed, &schema->dataTypes [i], sizeof(int));
        lengthParsed += sizeof(int);
        
        
        //itoa(schema->typeLength [i], integerString, INT_STRING_LEN);
        //memcpy(intString, schema->typeLength [i], sizeof(int));
        //strcat (schemaString, intString);
        memcpy(schemaString + lengthParsed, &schema->typeLength [i], sizeof(int));
        lengthParsed += sizeof(int);
        
    }
    
    //itoa(schema->keySize, integerString, INT_STRING_LEN);
    //memcpy(intString, schema->keySize, sizeof(int));
    //strcat (schemaString, intString);
    memcpy(schemaString + lengthParsed, &schema->keySize, sizeof(int));
    lengthParsed += sizeof(int);
    
    for (i=0; i<schema->keySize; i++){
        //itoa(schema->keyAttrs [i], integerString, INT_STRING_LEN);
        //memcpy(intString, schema->keyAttrs [i], sizeof(int));
        //strcat(schemaString, intString);
        memcpy(schemaString + lengthParsed, &schema->keyAttrs [i], sizeof(int));
        lengthParsed += sizeof(int);
        
    }
    
    CHECK(initBufferPool(bm, name, 3, RS_FIFO, NULL));
    CHECK(pinPage(bm, h, 0));
    returnCode = sprintf(h->data, "%s", schemaString);
    CHECK(markDirty(bm, h));
    CHECK(unpinPage(bm,h));
    
    CHECK(pinPage(bm, h, 1));
    memset(schemaString, '\0', PAGE_SIZE);
    lengthParsed = 0;
    //memcpy(intString, curLastPage, sizeof(int));
    //strcat(schemaString, intString);
    memcpy(schemaString, &curLastPage, sizeof(int));
    lengthParsed += sizeof(int);
    
    //memcpy(intString, numOfRecs, sizeof(int));
    //strcat(schemaString, intString);
    memcpy(schemaString + lengthParsed, &numOfRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    //memcpy(intString, numOfDelRecs, sizeof(int));
    //strcat(schemaString, intString);
    memcpy(schemaString + lengthParsed, &numOfDelRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    nextDelRecPos = 0;
    memcpy(schemaString + lengthParsed, &nextDelRecPos, sizeof(int));
    lengthParsed += sizeof(int);
    
    numOfDelRecsPossible = (PAGE_SIZE - lengthParsed) / PAGE_SIZE;
    
    //RID *delRecList = malloc (sizeof(RID) * numOfDelRecsPossible);
    RID delRecList [numOfDelRecsPossible];
    
    for (i=0; i<numOfDelRecsPossible; i++){
        //delRecList [i]->page = -1;
        //delRecList [i]->slot = -1;
        delRecList [i].page = -1;
        delRecList [i].slot = -1;
    }
    
    memcpy(schemaString + lengthParsed, &delRecList, (sizeof(RID) * numOfDelRecsPossible));
    
    returnCode = sprintf(h->data, "%s", schemaString);
    
    CHECK(markDirty(bm, h));
    CHECK(unpinPage(bm,h));
    
    CHECK(shutdownBufferPool(bm));
    
    free(bm);
    free(h);
    //free(intString);
    return RC_OK;
}

RC openTable (RM_TableData *rel, char *name){
    Schema *result;
    int i, lengthParsed, numAttrs, keySize;
    char dataString [DATA_STRING_LEN], schemaString [PAGE_SIZE];
    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    //char *intString = (char *) malloc (sizeof(int) + 1);
    
    CHECK(initBufferPool(bm, name, 3, RS_FIFO, NULL));
    CHECK(pinPage(bm, h, 0));
    strcpy(schemaString, h->data);
    CHECK(unpinPage(bm,h));
    lengthParsed = 0;
    
    //strncpy(intString, schemaString, sizeof(int));
    //numAttrs = atoi(intString);
    memcpy(&numAttrs, schemaString, sizeof(int));
    lengthParsed += sizeof(int);
    
    char **cpNames = (char **) malloc(sizeof(char*) * numAttrs);
    DataType *cpDt = (DataType *) malloc(sizeof(DataType) * numAttrs);
    //DataType dummyDataType [] = malloc (sizeof(DataType) * numAttrs);
    DataType dummyDataType [numAttrs];
    int *cpSizes = (int *) malloc(sizeof(int) * numAttrs);
    //int dummyDataLen [] = malloc (sizeof(int) * numAttrs);    
    int dummyDataLen [numAttrs];
    
    for (i=0; i<numAttrs; i++){
        strncpy(dataString, schemaString + lengthParsed, DATA_STRING_LEN);
        lengthParsed += DATA_STRING_LEN;        
        cpNames [i] = (char *) malloc(strlen(dataString) + 1);
        strcpy (cpNames [i], dataString);
        
        //strncpy (intString, schemaString + lengthParsed, sizeof(int));
        memcpy(&dummyDataType [i], schemaString + lengthParsed, sizeof(int));
        lengthParsed += sizeof(int);
        //dummyDataType [i] = atoi(intString);
        
        //strncpy (intString, schemaString + lengthParsed, sizeof(int));
        memcpy(&dummyDataLen [i], schemaString + lengthParsed, sizeof(int));
        lengthParsed += sizeof(int);
        //dummyDataLen [i] = atoi(intString);
    }
    
    //strncpy (intString, schemaString + lengthParsed, sizeof(int));
    memcpy(&keySize, schemaString + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    //keySize = atoi(intString);
    
    int *cpKeys = (int *) malloc(sizeof(int));
    //int keyAttrs [] = malloc (sizeof(int) * keySize);
    int keyAttrs [keySize];
    
    for (i=0; i<keySize; i++){
        //strncpy (intString, schemaString + lengthParsed, sizeof(int));
        memcpy(&keyAttrs [i], schemaString + lengthParsed, sizeof(int));
        lengthParsed += sizeof(int);
        //keyAttrs [i] = atoi(intString);
    }
    
    memcpy(cpDt, dummyDataType, sizeof(DataType) * numAttrs);
    memcpy(cpSizes, dummyDataLen, sizeof(int) * numAttrs);
    memcpy(cpKeys, keyAttrs, sizeof(int) * keySize);
    
    result = createSchema(numAttrs, cpNames, cpDt, cpSizes, keySize, cpKeys);
    
    rel->name = name;
    rel->schema = result;
    rel->mgmtData = bm;
    free(h);
    //free(intString);
    return RC_OK;
}

RC closeTable (RM_TableData *rel){
    BM_BufferPool *bm;
    bm = rel->mgmtData;
    CHECK(shutdownBufferPool(bm));
    return RC_OK;
}

RC deleteTable (char *name){
    CHECK(destroyPageFile(name));
    return RC_OK;
}

int getNumTuples (RM_TableData *rel){
    
}

RC insertRecord (RM_TableData *rel, Record *record){
    int lengthParsed, curLastPage, numOfRecs;
    int numOfDelRecs, recPageLengthParsed, numOfSlotsFilled, nextDelRecPos, numOfDelRecsPossible;
    int recordSize, remSpace, returnCode, pageNum, slotNum;
    BM_PageHandle *tableData = MAKE_PAGE_HANDLE();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_BufferPool *bm;
    char tableDataPage [PAGE_SIZE], recPage [PAGE_SIZE];
    //char *intString = (char *) malloc (sizeof(int));
    
    recordSize = getRecordSize(rel->schema);
    bm = rel->mgmtData;
    
    CHECK(pinPage(bm, tableData, 1));
    strcpy (tableDataPage, tableData->data);
    
    lengthParsed = 0;
    //strncpy (intString, tableDataPage + lengthParsed, sizeof(int));
    memcpy(&curLastPage, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    //curLastPage = atoi(intString);
    
    memcpy(&numOfRecs, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(&numOfDelRecs, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(&nextDelRecPos, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    numOfDelRecsPossible = (PAGE_SIZE - lengthParsed) / PAGE_SIZE;
    
    //RID *delRecList = malloc(sizeof(RID) * numOfDelRecsPossible);
    //memcpy(delRecList, tableDataPage + lengthParsed, (sizeof(RID) * numOfDelRecsPossible));
    RID delRecList [numOfDelRecsPossible];
    memcpy(&delRecList, tableDataPage + lengthParsed, (sizeof(RID) * numOfDelRecsPossible));
    lengthParsed += (sizeof(RID) * numOfDelRecsPossible);
    
    if (numOfRecs == 0){
        CHECK(pinPage(bm, h, curLastPage + 1));
        numOfSlotsFilled = 1;
        recPageLengthParsed = 0;
        memset (recPage, '\0', PAGE_SIZE);
        memcpy(recPage, &numOfSlotsFilled, sizeof(int));
        recPageLengthParsed += sizeof(int);
        memcpy(recPage + recPageLengthParsed, record->data, recordSize);
        returnCode = sprintf(h->data, "%s", recPage);
        CHECK(markDirty(bm, h));
        CHECK(unpinPage(bm,h));
        numOfRecs = 1;
    }else{
        if (numOfDelRecs = 0){
            CHECK(pinPage(bm, h, curLastPage));
            strcpy(recPage, h->data);
            recPageLengthParsed = 0;
            memcpy(&numOfSlotsFilled, recPage, sizeof(int));
            recPageLengthParsed += sizeof(int);
            recPageLengthParsed += (recordSize * numOfSlotsFilled);
            remSpace = PAGE_SIZE - recPageLengthParsed;
            if (remSpace >= recordSize){
                memcpy(recPage + recPageLengthParsed, record->data, recordSize);
                CHECK(markDirty(bm, h));
                CHECK(unpinPage(bm, h));
            }else{
                CHECK(unpinPage(bm, h));
                curLastPage += 1;
                CHECK(pinPage(bm, h, curLastPage));
                numOfSlotsFilled = 1;
                recPageLengthParsed = 0;
                memset (recPage, '\0', PAGE_SIZE);
                memcpy(recPage, &numOfSlotsFilled, sizeof(int));
                recPageLengthParsed += sizeof(int);
                memcpy(recPage + recPageLengthParsed, record->data, recordSize);
                returnCode = sprintf(h->data, "%s", recPage);
                CHECK(markDirty(bm, h));
                CHECK(unpinPage(bm,h));
                CHECK(markDirty(bm, tableData));
                numOfRecs += 1;
            }
        }else{
            //pageNum = delRecList->page [nextDelRecPos];
            //slotNum = delRecList->slot [nextDelRecPos];
            //delRecList->page [nextDelRecPos] = -1;
            //delRecList->slot [nextDelRecPos] = -1;
            pageNum = delRecList [nextDelRecPos].page;
            slotNum = delRecList [nextDelRecPos].slot;
            delRecList [nextDelRecPos].page = -1;
            delRecList [nextDelRecPos].slot = -1;
            if ((pageNum != -1) && (slotNum != -1)){
                CHECK(pinPage(bm, h, pageNum));
                strcpy(recPage, h->data);
                recPageLengthParsed = 0;
                memcpy(&numOfSlotsFilled, recPage, sizeof(int));
                numOfSlotsFilled++;
                memcpy(recPage, &numOfSlotsFilled, sizeof(int));
                recPageLengthParsed += sizeof(int);
                recPageLengthParsed += (recordSize * (slotNum - 1));
                memcpy(recPage + recPageLengthParsed, record->data, recordSize);
                returnCode = sprintf(h->data, "%s", recPage);
                CHECK(markDirty(bm, h));
                CHECK(unpinPage(bm,h));
                numOfRecs++;
                numOfDelRecs--;
            }
            nextDelRecPos++;
            if(nextDelRecPos > numOfDelRecsPossible){
                nextDelRecPos = 0;
            }
        }
    }
    
    lengthParsed = 0;
    memcpy(tableDataPage + lengthParsed, &curLastPage, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &numOfRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &numOfDelRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &nextDelRecPos, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, delRecList, (sizeof(RID) * numOfDelRecsPossible));
    lengthParsed += (sizeof(RID) * numOfDelRecsPossible);
    
    CHECK(markDirty(bm, tableData));
    CHECK(unpinPage(bm,tableData));
    free(delRecList);
    free(tableData);
    free(h);
    return RC_OK;
}

RC deleteRecord (RM_TableData *rel, RID id){
    int lengthParsed, curLastPage, numOfRecs;
    int numOfDelRecs, recPageLengthParsed, numOfSlotsFilled, nextDelRecPos, numOfDelRecsPossible;
    int recordSize, returnCode, pageNum, slotNum;
    BM_PageHandle *tableData = MAKE_PAGE_HANDLE();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_BufferPool *bm;
    char tableDataPage [PAGE_SIZE], recPage [PAGE_SIZE];
    
    recordSize = getRecordSize(rel->schema);
    bm = rel->mgmtData;
    
    CHECK(pinPage(bm, tableData, 1));
    strcpy (tableDataPage, tableData->data);
    
    lengthParsed = 0;
    memcpy(&curLastPage, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(&numOfRecs, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(&numOfDelRecs, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(&nextDelRecPos, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);
    
    numOfDelRecsPossible = (PAGE_SIZE - lengthParsed) / PAGE_SIZE;
    
    RID delRecList [numOfDelRecsPossible];
    memcpy(&delRecList, tableDataPage + lengthParsed, (sizeof(RID) * numOfDelRecsPossible));
    lengthParsed += (sizeof(RID) * numOfDelRecsPossible);
    
    pageNum = id.page;
    slotNum = id.slot;
    
    if (pageNum <= curLastPage){
        CHECK(pinPage(bm, h, pageNum));
        strcpy(recPage, h->data);
        recPageLengthParsed = 0;
        memcpy(&numOfSlotsFilled, recPage, sizeof(int));
        if (slotNum <= numOfSlotsFilled){
            numOfSlotsFilled--;
            memcpy(recPage, &numOfSlotsFilled, sizeof(int));
            recPageLengthParsed += sizeof(int);
            recPageLengthParsed += (recordSize * (slotNum - 1));
            memset(recPage + recPageLengthParsed, '\0', recordSize);
            returnCode = sprintf(h->data, "%s", recPage);
            CHECK(markDirty(bm, h));
            numOfRecs--;
            delRecList [numOfDelRecs].page = pageNum;
            delRecList [numOfDelRecs].slot = slotNum;
            numOfDelRecs++;
        }    
        CHECK(unpinPage(bm,h));
    }
    
    lengthParsed = 0;
    memcpy(tableDataPage + lengthParsed, &curLastPage, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &numOfRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &numOfDelRecs, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, &nextDelRecPos, sizeof(int));
    lengthParsed += sizeof(int);
    
    memcpy(tableDataPage + lengthParsed, delRecList, (sizeof(RID) * numOfDelRecsPossible));
    lengthParsed += (sizeof(RID) * numOfDelRecsPossible);
    
    CHECK(markDirty(bm, tableData));
    CHECK(unpinPage(bm,tableData));
    free(delRecList);
    free(tableData);
    free(h);
    return RC_OK;
}

RC updateRecord (RM_TableData *rel, Record *record){
    int lengthParsed, curLastPage;
    int recPageLengthParsed, numOfSlotsFilled;
    int recordSize, returnCode, pageNum, slotNum;
    BM_PageHandle *tableData = MAKE_PAGE_HANDLE();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_BufferPool *bm;
    RID id;
    char tableDataPage [PAGE_SIZE], recPage [PAGE_SIZE];
    
    recordSize = getRecordSize(rel->schema);
    bm = rel->mgmtData;
    
    CHECK(pinPage(bm, tableData, 1));
    strcpy (tableDataPage, tableData->data);
    CHECK(unpinPage(bm,tableData));
    
    lengthParsed = 0;
    memcpy(&curLastPage, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);

    id = record->id;
    pageNum = id.page;
    slotNum = id.slot;
    
    if (pageNum <= curLastPage){
        CHECK(pinPage(bm, h, pageNum));
        strcpy(recPage, h->data);
        recPageLengthParsed = 0;
        memcpy(&numOfSlotsFilled, recPage, sizeof(int));
        recPageLengthParsed += sizeof(int);
        if (slotNum <= numOfSlotsFilled){
            recPageLengthParsed += (recordSize * (slotNum - 1));
            memset(recPage + recPageLengthParsed, &record->data, recordSize);
            returnCode = sprintf(h->data, "%s", recPage);
            CHECK(markDirty(bm, h));
        }    
        CHECK(unpinPage(bm,h));
    }
    
    free(tableData);
    free(h);
    return RC_OK;
}

RC getRecord (RM_TableData *rel, RID id, Record *record){
    int lengthParsed, curLastPage;
    int recPageLengthParsed, numOfSlotsFilled;
    int recordSize, pageNum, slotNum;
    int recordFoundInd;
    BM_PageHandle *tableData = MAKE_PAGE_HANDLE();
    BM_PageHandle *h = MAKE_PAGE_HANDLE();
    BM_BufferPool *bm;
    char tableDataPage [PAGE_SIZE], recPage [PAGE_SIZE];
    
    recordSize = getRecordSize(rel->schema);
    bm = rel->mgmtData;
    recordFoundInd = 0;
    
    CHECK(pinPage(bm, tableData, 1));
    strcpy (tableDataPage, tableData->data);
    CHECK(unpinPage(bm,tableData));
    
    lengthParsed = 0;
    memcpy(&curLastPage, tableDataPage + lengthParsed, sizeof(int));
    lengthParsed += sizeof(int);

    pageNum = id.page;
    slotNum = id.slot;
    
    if (pageNum <= curLastPage){
        CHECK(pinPage(bm, h, pageNum));
        strcpy(recPage, h->data);
        recPageLengthParsed = 0;
        memcpy(&numOfSlotsFilled, recPage, sizeof(int));
        recPageLengthParsed += sizeof(int);
        if (slotNum <= numOfSlotsFilled){
            recPageLengthParsed += (recordSize * (slotNum - 1));
            memset(&record->data, recPage + recPageLengthParsed, recordSize);
            record->id = id;
            recordFoundInd = 1;
        }else{
            recordFoundInd = 2;
        }
        CHECK(unpinPage(bm,h));
    }else{
        recordFoundInd = 3;
    }
    
    free(tableData);
    free(h);
    if (recordFoundInd == 1){
        return RC_OK;
    }else{
        if (recordFoundInd == 2){
            return RC_READ_NON_EXISTING_RECORD;
        }else{
            if (recordFoundInd == 3){
                return RC_READ_NON_EXISTING_PAGE;
            }
        }
    }
}

RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){
    RID scanRID;
    RM_Scan_MgmtInfo *scan_MgmtInfo = malloc (sizeof(RM_Scan_MgmtInfo));
    scanRID.page = 2;
    scanRID.slot = 1;
    scan_MgmtInfo->cond = cond;
    scan_MgmtInfo->lastRID = scanRID;
    scan->mgmtData = scan_MgmtInfo;
    scan->rel = rel;
    return RC_OK;
}

RC next (RM_ScanHandle *scan, Record *record){
    RM_Scan_MgmtInfo *scan_MgmtInfo;
    RID scanRID;
    Expr *expr;
    
    RM_TableData *rel;
    Schema *schema;
    
    Value *result = malloc (sizeof(Value));
    int nextRecInd;
    RC returnCode;
    
    scan_MgmtInfo = scan->mgmtData;
    scanRID = scan_MgmtInfo->lastRID;
    expr = scan_MgmtInfo->cond;
    
    rel = scan->rel;
    schema = rel->schema;
    
    nextRecInd = 0;
    
    do{
        returnCode = getRecord(rel, scanRID, record);
        if (returnCode == RC_OK){
            CHECK(evalExpr(record, schema, expr, &result));
            if (result->v.boolV){
                nextRecInd = 1;
            }else{
                scan_MgmtInfo->lastRID.slot++;
            }
        }else{
            if (returnCode == RC_READ_NON_EXISTING_RECORD){
                scan_MgmtInfo->lastRID.page++;
                scan_MgmtInfo->lastRID.slot = 1;
            }else{
                if (returnCode == RC_READ_NON_EXISTING_PAGE){
                    nextRecInd = 2;
                }
            }
        }
    }while(nextRecInd == 0);
    
    free(result);
    
    if (nextRecInd == 1){
        return RC_OK;
    }else{
        return RC_RM_NO_MORE_TUPLES;
    }
}

RC closeScan (RM_ScanHandle *scan){
    RM_Scan_MgmtInfo *scan_MgmtInfo;
    scan_MgmtInfo = scan->mgmtData;
    free(scan_MgmtInfo);
    free(scan);
    return RC_OK;
}

int getRecordSize (Schema *schema){
    int i, recordSize;
    recordSize = 0;
    
    for (i=0; i<schema->numAttr; i++){
        switch (schema->dataTypes [i]){
            case DT_INT:
                recordSize += sizeof(int);
                break;
            case DT_FLOAT:
                recordSize += sizeof(float);
                break;
            case DT_STRING:
                recordSize += (sizeof(char) * schema->typeLength [i]);
                break;
            case DT_BOOL:
                recordSize += sizeof(bool);
                break;
	}
    }
    
    return recordSize;
}

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys){
    Schema *result = malloc (sizeof(Schema));
    
    result->numAttr = numAttr;
    result->attrNames = attrNames;
    result->dataTypes = dataTypes;
    result->typeLength = typeLength;
    result->keySize = keySize;
    result->keyAttrs = keys;
    
    return result;
}

RC freeSchema (Schema *schema){
    free(schema);
    return RC_OK;
}

RC createRecord (Record **record, Schema *schema){
    Record *rec;
    //rec->id = (RID) malloc (sizeof(RID));
    rec->data = (char *) malloc (getRecordSize(schema));
    *record = rec;
    return RC_OK;
}

RC freeRecord (Record *record){
    free(record->data);
    free(record);
    return RC_OK;
}

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value){
    int i, offset, attrSize;
    Value *val = (Value *) malloc(sizeof(Value));
    offset = 0;
    attrSize = 0;
    if (attrNum <= schema->numAttr){
        for (i=0;i<attrNum;i++){
            switch(schema->dataTypes[i]){
                case DT_INT:
                    offset += sizeof(int);
                    break;
                case DT_FLOAT:
                    offset += sizeof(float);
                    break;
                case DT_STRING:
                    offset += (sizeof(char) * schema->typeLength [i]);
                    break;
                case DT_BOOL:
                    offset += sizeof(bool);
                    break;
            }
        }
        switch(schema->dataTypes[attrNum]){
            case DT_INT:
                attrSize = sizeof(int);
                val->dt = DT_INT;
                memcpy(&val->v.intV, &record->data + offset, attrSize);
                break;
            case DT_FLOAT:
                attrSize = sizeof(float);
                val->dt = DT_FLOAT;
                memcpy(&val->v.floatV, &record->data + offset, attrSize);
                break;
            case DT_STRING:
                attrSize = (sizeof(char) * schema->typeLength [attrNum]);
                val->dt = DT_STRING;
                memcpy(&val->v.stringV, &record->data + offset, attrSize);
                break;
            case DT_BOOL:
                attrSize = sizeof(bool);
                val->dt = DT_BOOL;
                memcpy(&val->v.boolV, &record->data + offset, attrSize);
                break;
	}
        *value = val;
    }
    return RC_OK;
}

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value){
    int i, offset, attrSize;
    offset = 0;
    attrSize = 0;
    if (attrNum <= schema->numAttr){
        for (i=0;i<attrNum;i++){
            switch(schema->dataTypes[i]){
                case DT_INT:
                    offset += sizeof(int);
                    break;
                case DT_FLOAT:
                    offset += sizeof(float);
                    break;
                case DT_STRING:
                    offset += (sizeof(char) * schema->typeLength [i]);
                    break;
                case DT_BOOL:
                    offset += sizeof(bool);
                    break;
            }
        }
        switch(schema->dataTypes[attrNum]){
            case DT_INT:
                attrSize = sizeof(int);
                memcpy(&record->data + offset, &value->v.intV, attrSize);
                break;
            case DT_FLOAT:
                attrSize = sizeof(float);
                memcpy(&record->data + offset, &value->v.floatV, attrSize);
                break;
            case DT_STRING:
                attrSize = (sizeof(char) * schema->typeLength [attrNum]);
                memcpy(&record->data + offset, &value->v.stringV, attrSize);
                break;
            case DT_BOOL:
                attrSize = sizeof(bool);
                memcpy(&record->data + offset, &value->v.boolV, attrSize);
                break;
	}
    }
    return RC_OK;
}
