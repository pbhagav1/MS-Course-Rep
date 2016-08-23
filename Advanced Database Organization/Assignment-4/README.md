B+Tree Implementation.

This programming assignment is used to implement the B+ Tree Index Structure. The addtional structures used are mentioned below.

Structure - 1:
typedef struct BTreeMgmtInfo {
  int nodeSize;
  int numberOfNodes;
  int numberOfEntries;
  int rootPageNum;
  int curLastPage;
  BM_BufferPool *bTreeBM;
} BTreeMgmtInfo;

	1. Node Size - stores the number of keys per node.
	2. Number of Nodes - Current number of Nodes in the B+Tree
	3. Number of Entries - Current number of Keys stored in the B+Tree
	4. Root Page Number - Field storing the value of page number of the root of B+Tree
	5. Current Last Page - Field storing the current last page number of the B+Tree
	6. Buffer pool pointer - Pointer to the buffer pool which is used to manage the B+Tree
	
	A pointer to this structure is stored in the "void *mgmtData" of "BTreeHandle".

Structure - 2:
typedef struct BT_ScanHandleMgmtInfo {
    PageNumber lastVisitedPage;
    Value lastValuePrinted;
} BT_ScanHandleMgmtInfo;

	1. Last Visited Page: Holds the last visited leaf node page number
	2. Last Value Printed: Holds the last value that was returned to the user.

	A pointer to this structure is stored in the "void *mgmtData" of "BT_ScanHandle".

Different Nodes:
	There are two types of nodes in the B+Tree. Leaf Nodes and Non-leaf nodes.
Leaf Nodes:
	The leaf nodes will have the following fields:
	1. Node Type - This field is used to differentiate between the Leaf nodes and Non-leaf nodes
	2. Number of Node Entries - Current number of keys stored in the node.
	3. Node Entries - An array of key values. The size of array is same as the node size.
	4. Record Pointers - An array of RID values. The size of array is same as the node size.
	5. Next Leaf Pointer - Field storing the value of the next child node.

Non-Leaf Nodes:
	The Non-leaf nodes will have the following fields:
	1. Node Type - This field is used to differentiate between the Leaf nodes and Non-leaf nodes
	2. Number of Node Entries - Current number of keys stored in the node.
	3. Node Entries - An array of key values. The size of array is same as the node size.
	4. Child Node pointers - Array of page numbers, each representing this nodes children. The size of the array is (node size + 1).
	