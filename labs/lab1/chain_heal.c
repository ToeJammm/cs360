/*
chain_heal.c
Jake Marlow
cs360
1/28/2024
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct node // node for each player
{
    char *name;
    int x, y;
    int cur_PP, max_PP;
    struct node *prev;
    struct node **adj;
    int adj_size;
    int visited; //for dfs
} Node;

int DFS(Node *index, int power, int jumpsLeft, int powReduction, int hop) {


    if(index->visited == 1) { //check if visited
        printf("returned because already visited %s \n", index->name);
        return 0;
    }

    index->visited = 1; //set visited field

    printf("visited: %s  hop: %d \n\n", index->name, hop);
    

    power *= powReduction; //reduce power, jumps, and increase hop
    hop += 1;
    jumpsLeft -= 1;
    
    printf("number of jumps left at %s: %d \n", index->name, jumpsLeft);

    if(jumpsLeft == 0) {
        //printf("returned because no jumps left at %s \n", index->name);
        return 0;
    }

    
    for(int i = 0; i < index->adj_size; i++) {
        DFS(index->adj[i], power, jumpsLeft, powReduction, hop);
    }


    return 0;
}

void printNode(Node node)
{
    printf("x: %d\n", node.x);
    printf("y: %d\n", node.y);
    printf("Current PP: %d\n", node.cur_PP);
    printf("Max PP: %d\n", node.max_PP);
    printf("Name: %s\n", node.name);
    if (node.prev != NULL)
    {
        printf("prev: %s", node.prev->name);
    }
    printf("\n\n");
}

void resetNodes(Node **nodeArray, int nodeCount) {
    for(int i = 0; i < nodeCount; i++) {
        nodeArray[i]->visited = 0;
    }
}

// use scanf to read in words rather than lines
int main(int argc, char **argv) // initial range, jump range, num jumps, initial power, power reduction
{
    if (argc != 6)
    {
        printf("incorrect number of arguements");
        return 0;
    }
    int nodeSize = sizeof(Node); // for malloc
    int initRange, jumpRange, numJumps, initPow;
    double powReduction;
    initRange = atoi(argv[1]);
    jumpRange = atoi(argv[2]);
    numJumps = atoi(argv[3]);
    initPow = atoi(argv[4]);
    powReduction = atof(argv[5]);

    printf("init Range: %d \njumpRange: %d \nnumJumps: %d \ninitPow: %d \npowReduction: %.2f",
           initRange, jumpRange, numJumps, initPow, powReduction);

    printf("\nnow reading in nodes\n\n");

    Node node;
    Node *previous = &node; // keeps track of previous node
    node.name = malloc(256);
    node.prev = NULL;
    scanf("%d %d %d %d %255s", &node.x, &node.y, &node.cur_PP, &node.max_PP, node.name);
    int nodeCount = 1; // urgosa's node

    // printNode(node);

    int x, y, curr_PP, max_PP;
    char name[256];
    while (scanf("%d %d %d %d %255s", &x, &y, &curr_PP, &max_PP, name) == 5)
    {
        Node *newNode = malloc(nodeSize);
        newNode->x = x;
        newNode->y = y;
        newNode->cur_PP = curr_PP;
        newNode->max_PP = max_PP;
        newNode->name = malloc(256); // Allocate memory for the name
        strcpy(newNode->name, name); // Copy the name into the newNode
        newNode->prev = previous;
        previous = newNode;
        nodeCount++;
        // printNode(*newNode);
    }

    Node **nodeArray = (Node **)malloc(nodeCount * sizeof(Node *)); // initialize array

    for (int i = nodeCount - 1; i >= 0; i--)
    { // assign array the node pointers backwards
        nodeArray[i] = previous;
        previous = previous->prev;
    }

    for (int i = 0; i < nodeCount; i++)
    { // testing if array is in order
        printNode(*nodeArray[i]);
    }
    int xVal, yVal;
    for (int i = 0; i < nodeCount; i++)
    {                               // find sizes of the adj_array
        nodeArray[i]->adj_size = 0; // initialize value
        for (int j = 0; j < nodeCount; j++)
        {
            xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
            yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
            if ((sqrt(xVal * xVal) + (yVal * yVal)) <= jumpRange && (i != j))
                nodeArray[i]->adj_size++;
        }
        printf(" %s's adj list size set to %d \n", nodeArray[i]->name, nodeArray[i]->adj_size);
    }

    int adjListIndex = 0;
    for (int i = 0; i < nodeCount; i++)
    {
        nodeArray[i]->adj = malloc(nodeArray[i]->adj_size * sizeof(Node *)); // allocate adjacency list size
        for (int j = 0; j < nodeCount && adjListIndex < nodeArray[i]->adj_size; j++)
        {            
                xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
                yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
                if ((sqrt(xVal * xVal) + (yVal * yVal)) <= jumpRange && (i != j)) {
                    nodeArray[i]->adj[adjListIndex] = nodeArray[j];
                    //printf("inserting %s into %s's adj list\n\n", nodeArray[j]->name, nodeArray[j]->name);
                    adjListIndex++;
                } //if valid, add to list
                    
            }
            adjListIndex = 0; // reset index (this is all to avoid a triple for loop)
        }
        

        printf("\n  \n");

       for(int i = 0; i < nodeCount; i++) {
       printf("%s's adjacency list:\n", nodeArray[i]->name);
            for(int j = 0; j < nodeArray[i]->adj_size; j++) {
                printf("- %s\n", nodeArray[i]->adj[j]->name);
            }
            printf("\n");
       }


        printf("commencing DFS\n\n");
        //dfs - all visited fields = 0 at this point

        //int maxHealing;
        for(int i = 0; i < nodeCount; i++) {
              xVal = abs(nodeArray[0]->x - nodeArray[i]->x);
              yVal = abs(nodeArray[0]->y - nodeArray[i]->y);
                if(sqrt(xVal * xVal) + (yVal * yVal) <= initRange)  {
                    resetNodes(nodeArray, nodeCount);
                    printf("calling DFS \n");
                    DFS(nodeArray[i], initPow, numJumps, powReduction, 1);
                }
            }
           return 0;
        }

        /*
        Doing the DFS: Now you should add a visited field to each node for your DFS. 
        You should write a DFS() procedure, which has three arguments:

        The node.
        The hop number.
        A pointer to a struct that contains global information (such as num_jumps, and power_reduction).
        Now write a DFS that traverses all of the paths from each starting node. Have it print out [node,hop] 
        for each time that it visits a node.
        
        Check n's visited field. If true, then return.
        Set n's visited field to true.
        Optionally do some activity on n.
        Then, for all edges of the form (n,u) call DFS on u.
        Optionally Do some final activity on n.
        Return.
        
        */


        
// if(index->cur_PP < index->max_PP && (index->max_PP - index->cur_PP) < power) { //set new health value
    //     index->cur_PP = index->max_PP;
    // } else if (index->cur_PP < index->max_PP && (index->max_PP - index->cur_PP) > power) {
    //     index->cur_PP += power;
    // } //saved this for later in dfs


    