/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @Team Members:
 *	Baradi Sandeep– A20352613
 *	Bhagavatula Prasanth – A20355611
 *	Mamidi Sudheer Durga Venkata– A20344048
 */

public class NetworkTopology {
    private static int[][] inputMatrix;
    private static int numberOfRouters;
    
    public static void main(String args[]){
        getNetworkTopologyInput();
        System.out.println("\nRouter-1 : ");
        computeForRouter(0);
        /*System.out.println("\nRouter-2 : ");
        computeForRouter(1);
        System.out.println("\nRouter-3 : ");
        computeForRouter(2);
        System.out.println("\nRouter-4 : ");
        computeForRouter(3);
        System.out.println("\nRouter-5 : ");
        computeForRouter(4);*/
        System.out.println("\nRouter-6 : ");
        computeForRouter(5);
    }
    
    public static void getNetworkTopologyInput(){
        numberOfRouters = 6;
        inputMatrix = new int[6][6];
        int i,j;
        for (i=0; i< numberOfRouters; i++){
            for (j=0; j< numberOfRouters; j++){
                if (i == j){
                    inputMatrix[i][j] = -1;
                }else{
                    inputMatrix[i][j] = 0;
                }
            }
        }
        
        inputMatrix[0][1] = 3;
        inputMatrix[1][0] = 4;
        inputMatrix[1][2] = 6;
        inputMatrix[2][3] = 7;
        inputMatrix[3][4] = 11;
        inputMatrix[4][5] = 9;
        inputMatrix[5][1] = 8;
        inputMatrix[5][2] = 5;
        
        System.out.println("Input Matrix\n");
        //System.out.println("     R1   R2   R3   R4   R5   R6\n");

        System.out.print("     ");
        for (i=0; i<numberOfRouters; i++){
             j = i + 1;
            System.out.print(" R" + j + "  ");
        }
        System.out.print("\n");

        for (i=0; i< numberOfRouters; i++){
            i++;
            System.out.print(" R" + i);
            i--;
            for (j=0; j< numberOfRouters; j++){
                System.out.print("  " + inputMatrix[i][j] + "  ");
            }
            System.out.print("\n");
        }

    }
    
    public static void computeForRouter(int router){
        int iteration, newRouterFound, tempRouterNum, routerProcessed;
        int i, j, k ,nextNewRouterToBeProcessed, newOutRtrListPos, newAllRtrListPos, lastNewRouterPos;
        int[] newOutRtrList = new int[numberOfRouters];
        int[] newAllRtrList = new int[numberOfRouters];
        int rowRouter, colRouter, rtrOutInd, rtrInInd;
        iteration = -1;        
        
        for(i=0; i< numberOfRouters; i++){
            newOutRtrList [i] = -1;
            newAllRtrList [i] = -1;
        }
        newRouterFound = 0;
        newOutRtrListPos = 0;
        newAllRtrListPos = 0;
        nextNewRouterToBeProcessed=0;
        newOutRtrList[newOutRtrListPos] = router;
        newAllRtrList[newOutRtrListPos] = router;
        newOutRtrListPos++;
        newAllRtrListPos++;
        do{
            newRouterFound = 0;
            iteration++;
            lastNewRouterPos = newOutRtrListPos;
            for (i=nextNewRouterToBeProcessed; i < lastNewRouterPos; i++){
                tempRouterNum = newOutRtrList[i];
                for (j=0; j<numberOfRouters; j++){
                    if (inputMatrix[tempRouterNum][j] > 0){
                        routerProcessed = 0;
                        for (k=0; k < numberOfRouters; k++){
                            if (j == newOutRtrList[k]){
                                routerProcessed = 1;
                            }
                        }
                        if (routerProcessed == 0){
                            newOutRtrList[newOutRtrListPos] = j;
                            newOutRtrListPos++;
                            newRouterFound = 1;
                        }
                        routerProcessed = 0;
                        for (k=0; k < numberOfRouters; k++){
                            if (j == newAllRtrList[k]){
                                routerProcessed = 1;
                            }
                        }
                        if (routerProcessed == 0){
                            newAllRtrList[newAllRtrListPos] = j;
                            newAllRtrListPos++;
                        }
                    }
                    if (inputMatrix[j][tempRouterNum] > 0){
                        routerProcessed = 0;
                        for (k=0; k < numberOfRouters; k++){
                            if (j == newAllRtrList[k]){
                                routerProcessed = 1;
                            }
                        }
                        if (routerProcessed == 0){
                            newAllRtrList[newAllRtrListPos] = j;
                            newAllRtrListPos++;
                            newRouterFound = 1;
                        }
                    }
                }
            }
            System.out.println("\nIteration : " + iteration + "\n");
            if (iteration != 0){
                router++;
                System.out.print("Router - " + router + " asks ");
                router--;
                for (i=nextNewRouterToBeProcessed; i < lastNewRouterPos; i++){
                    tempRouterNum = newOutRtrList[i];
                    tempRouterNum++;
                    System.out.print(" Router - " + tempRouterNum + " ");
                }
                System.out.print("\n");
            }
            nextNewRouterToBeProcessed = lastNewRouterPos;
            System.out.print("      ");
            for (j=0; j<newAllRtrListPos; j++){
                colRouter = newAllRtrList[j];
                colRouter++;
                System.out.print(" R" + colRouter + "  ");
            }
            System.out.print("\n");
            for (i=0; i<newAllRtrListPos; i++){
                rowRouter = newAllRtrList[i];
                rowRouter++;
                System.out.print(" R" + rowRouter + "  ");
                rowRouter--;
                rtrOutInd = 0;
                for (k=0; k < lastNewRouterPos; k++){
                    if (rowRouter == newOutRtrList[k]){
                        rtrOutInd = 1;
                    }
                }
                for (j=0; j<newAllRtrListPos; j++){
                    colRouter = newAllRtrList[j];
                    if (rtrOutInd == 1){
                        System.out.print("  " + inputMatrix[rowRouter][colRouter] + "  ");
                    }else{
                        rtrInInd = 0;
                        for (k=0; k < lastNewRouterPos; k++){
                            if (colRouter == newOutRtrList[k]){
                                rtrInInd = 1;
                            }
                        }
                        if (rtrInInd == 1){
                            System.out.print("  " + inputMatrix[rowRouter][colRouter] + "  ");
                        }else{
                            if (i == j){
                                System.out.print("  -1  ");
                            }else{
                                System.out.print("  0  ");
                            }
                        }
                    }
                }
                System.out.print("\n");
            }
        }while(newRouterFound == 1);
    }
}
