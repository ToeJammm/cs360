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
    int visited; // for dfs
    int health_added;
} Node;

typedef struct Global
{
    int initRange, jumpRange, numJumps, initPow;
    double powReduction;
    int bestHealing;
    Node **best_path;
    Node *curr;
    Node *from;
    int *healing;
    int bestPathLength;
} Global;



void DFS(Node *index, Global *info, int hop, int total_healing, Node *from)
{

    int power = rint(info->initPow * pow(1 - info->powReduction, hop - 1)); // sets power (works)

    if (index->visited == 1)
    { // check if visited
        return;
    }

    if (info->numJumps - (hop - 1) == 0)
    {
        return;
    }

    index->visited = 1; // set visited field

    int healingAdded;
    if ((index->max_PP - index->cur_PP) <= power)
    { // set new health value
        healingAdded = index->max_PP - index->cur_PP;
        total_healing += healingAdded; // adding healed amount
                                       // printf("1: healed %s %d HP\n", index->name, healingAdded);
        index->cur_PP = index->max_PP;
    }
    else if ((index->max_PP - index->cur_PP) > power)
    {
        healingAdded = power;
        //   printf("2: healed %s %d HP\n", index->name, healingAdded);
        total_healing += power;
        index->cur_PP += power;
    }

    index->health_added = healingAdded;
    // printf("current HP: %d/%d\n\n", index->cur_PP, index->max_PP);
    //printf("%s  hop: %d \n\n", index->name, hop);
    index->prev = from; //set prev
    for (int i = 0; i < index->adj_size; i++)
    {
      DFS(index->adj[i], info, hop + 1, total_healing, index);
    }

    

    
    
    if (info->bestHealing < total_healing)
    {
        info->bestHealing = total_healing;
        info->bestPathLength = hop - 1;
        Node *curr = index;
       // printf("found a best path length: %d \n", info->bestPathLength);
        for (int i = info->bestPathLength; i >= 0; i--) //adding nodes to best path array
        { // putting path into best_path in reverse order;
            info->best_path[i] = curr;
           // printf("adding %s to best path index: %d\n", curr->name, i);
            info->healing[i] = curr->health_added;
          //  printf("adding %d health to index %d of healing array\n", curr->health_added, i);
            curr = curr->prev;
        }
    }

    index->visited = 0;
    index->cur_PP -= healingAdded;
    return;
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

void resetNodes(Node **nodeArray, int nodeCount)
{
    for (int i = 0; i < nodeCount; i++)
    {
        nodeArray[i]->visited = 0;
    }
}

// use scanf to read in words rather than lines
int main(int argc, char **argv) // initial range, jump range, num jumps, initial power, power reduction
{

    Global *info = (Global *)malloc(sizeof(Global));
    info->bestHealing = 0;
    int nodeSize = sizeof(Node); // for malloc
    info->curr = (Node *)malloc(nodeSize);
    info->from = (Node *)malloc(nodeSize);
    info->bestHealing = 0;

    if (argc != 6)
    {
        printf("incorrect number of arguements");
        return 0;
    }

    info->initRange = atoi(argv[1]);
    info->jumpRange = atoi(argv[2]);
    info->numJumps = atoi(argv[3]);
    info->initPow = atoi(argv[4]);
    info->powReduction = atof(argv[5]);

    //printf("init Range: %d \njumpRange: %d \nnumJumps: %d \ninitPow: %d \npowReduction: %.2f",
          // info->initRange, info->jumpRange, info->numJumps, info->initPow, info->powReduction);

    //printf("\nnow reading in nodes\n\n");

    Node node;
    Node *previous = &node; // keeps track of previous node
    node.name = malloc(256);
    node.prev = NULL;
    node.health_added = 0;
    scanf("%d %d %d %d %255s", &node.x, &node.y, &node.cur_PP, &node.max_PP, node.name);
    int nodeCount = 1; // urgosa's node

    // printNode(node);

    int x, y, curr_PP, max_PP;
    char name[256];
    while (scanf("%d %d %d %d %255s", &x, &y, &curr_PP, &max_PP, name) == 5)
    {
        Node *newNode = malloc(nodeSize);
        newNode->health_added = 0;
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

    Node **nodeArray = (Node **)malloc(nodeCount * sizeof(Node *));   // initialize array
    info->best_path = (Node **)malloc(sizeof(Node) * info->numJumps); // create array for best path
    info->healing = (int *)malloc(sizeof(int) * info->numJumps);

    for (int i = nodeCount - 1; i >= 0; i--)
    { // assign array the node pointers backwards
        nodeArray[i] = previous;
        previous = previous->prev;
    }

    
    int xVal, yVal;
    for (int i = 0; i < nodeCount; i++)
    {                               // find sizes of the adj_array
        nodeArray[i]->adj_size = 0; // initialize value
        for (int j = 0; j < nodeCount; j++)
        {
            xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
            yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
            if ((sqrt(xVal * xVal) + (yVal * yVal)) <= info->jumpRange && (i != j))
                nodeArray[i]->adj_size++;
        }
       // printf(" %s's adj list size set to %d \n", nodeArray[i]->name, nodeArray[i]->adj_size);
    }

    int adjListIndex = 0;
    for (int i = 0; i < nodeCount; i++)
    {
        nodeArray[i]->adj = malloc(nodeArray[i]->adj_size * sizeof(Node *)); // allocate adjacency list size
        for (int j = 0; j < nodeCount && adjListIndex < nodeArray[i]->adj_size; j++)
        {
            xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
            yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
            if ((sqrt(xVal * xVal) + (yVal * yVal)) <= info->jumpRange && (i != j))
            {
                nodeArray[i]->adj[adjListIndex] = nodeArray[j];
                // printf("inserting %s into %s's adj list\n\n", nodeArray[j]->name, nodeArray[j]->name);
                adjListIndex++;
            } // if valid, add to list
        }
        adjListIndex = 0; // reset index (this is all to avoid a triple for loop)
    }

    // for (int i = 0; i < nodeCount; i++)
    // {
    //     printf("%s's adjacency list:\n", nodeArray[i]->name);
    //     for (int j = 0; j < nodeArray[i]->adj_size; j++)
    //     {
    //         printf("- %s\n", nodeArray[i]->adj[j]->name);
    //     }
    //     printf("\n");
    // }

   // printf("commencing DFS\n\n");
    // dfs - all visited fields = 0 at this point

    // int maxHealing;
    info->from = NULL;
    for (int i = 0; i < nodeCount; i++)
    {
        xVal = abs(nodeArray[0]->x - nodeArray[i]->x);
        yVal = abs(nodeArray[0]->y - nodeArray[i]->y);
        if (sqrt(xVal * xVal) + (yVal * yVal) <= info->initRange)
        {
            resetNodes(nodeArray, nodeCount);
           // printf("calling DFS \n");
            DFS(nodeArray[i], info, 1, 0, info->from);
        }
    }
    

    // printf("%s\n\n",info->best_path[0]->name);
    //printf("\nbestPathLength: %d\n\n", info->bestPathLength);


    for (int i = 0; i <= info->bestPathLength; i++) //print out best path
    {
       // printf("i: %d\n", i);
        printf("%s %d\n", info->best_path[i]->name, info->healing[i]);
    }
    
    printf("Total_Healing %d\n", info->bestHealing);


    return 0;
}