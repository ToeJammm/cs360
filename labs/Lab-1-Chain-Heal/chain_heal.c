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

typedef struct node // node for each character
{
    char *name;
    int x, y;
    int cur_PP, max_PP;
    struct node *prev;
    struct node **adj;
    int adj_size;
    int visited;
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

void DFS(Node *index, Global *info, int hop, int total_healing, Node *from) // search algorithm
{

    int power = rint(info->initPow * pow(1 - info->powReduction, hop - 1)); // sets power based on hop

    if (index->visited == 1) // check if visited
    {
        return;
    }

    if (info->numJumps - (hop - 1) == 0) // or out of jumprange
    {
        return;
    }

    index->visited = 1; // set visited field

    int healingAdded;
    if ((index->max_PP - index->cur_PP) <= power) // set new health value
    {
        healingAdded = index->max_PP - index->cur_PP;
        total_healing += healingAdded; // adding healed amount
        index->cur_PP = index->max_PP;
    }
    else if ((index->max_PP - index->cur_PP) > power)
    {
        healingAdded = power;
        total_healing += power;
        index->cur_PP += power;
    }

    index->health_added = healingAdded;
    index->prev = from; // set prev node to add to list if best healing has been achieved
    for (int i = 0; i < index->adj_size; i++)
    {
        DFS(index->adj[i], info, hop + 1, total_healing, index);
    }

    if (info->bestHealing < total_healing)
    {
        info->bestHealing = total_healing;
        info->bestPathLength = hop - 1;
        Node *curr = index;
        for (int i = info->bestPathLength; i >= 0; i--) // adding nodes to best path array
        {
            info->best_path[i] = curr;
            info->healing[i] = curr->health_added;
            curr = curr->prev;
        }
    }

    index->visited = 0;
    index->cur_PP -= healingAdded; // backtrack numbers and visited
    return;
}

void resetNodes(Node **nodeArray, int nodeCount) // for DFS
{
    for (int i = 0; i < nodeCount; i++)
    {
        nodeArray[i]->visited = 0;
    }
}

int main(int argc, char **argv)
{
    // allocate space for everything and read variables from stdin and input file
    Global *info = (Global *)malloc(sizeof(Global));
    info->bestHealing = 0;
    int nodeSize = sizeof(Node); // for malloc
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

    Node *node = (Node *)malloc(nodeSize);
    Node *previous = node;
    node->name = malloc(256);
    node->prev = NULL;
    node->health_added = 0;
    scanf("%d %d %d %d %255s", &node->x, &node->y, &node->cur_PP, &node->max_PP, node->name);
    int nodeCount = 1; // urgosa's node

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
    }

    Node **nodeArray = (Node **)malloc(nodeCount * sizeof(Node *));   // initialize array
    info->best_path = (Node **)malloc(sizeof(Node) * info->numJumps); // create array for best path
    info->healing = (int *)malloc(sizeof(int) * info->numJumps);

    for (int i = nodeCount - 1; i >= 0; i--)
    { // assign array the node pointers backwards
        nodeArray[i] = previous;
        previous = previous->prev;
    }

    long xVal, yVal;
    long distance;
    for (int i = 0; i < nodeCount; i++) // find sizes of the adj_array
    {
        nodeArray[i]->adj_size = 0;
        for (int j = 0; j < nodeCount; j++)
        {
            xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
            yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
            distance = sqrt((xVal * xVal) + (yVal * yVal));
            if (distance <= info->jumpRange && (i != j))
                nodeArray[i]->adj_size++;
        }
    }

    int adjListIndex = 0;
    for (int i = 0; i < nodeCount; i++)
    {
        nodeArray[i]->adj = malloc(nodeArray[i]->adj_size * sizeof(Node *)); // allocate adjacency list size
        for (int j = 0; j < nodeCount && adjListIndex < nodeArray[i]->adj_size; j++)
        {
            xVal = abs(nodeArray[i]->x - nodeArray[j]->x);
            yVal = abs(nodeArray[i]->y - nodeArray[j]->y);
            distance = sqrt((xVal * xVal) + (yVal * yVal));
            if (distance <= info->jumpRange && (i != j))
            { // if valid, add to list
                nodeArray[i]->adj[adjListIndex] = nodeArray[j];
                adjListIndex++;
            }
        }
        adjListIndex = 0; // reset index (this is to avoid a triple for loop)
    }

    info->from = NULL;                  // for the first call
    for (int i = 0; i < nodeCount; i++) // call DFS on every node reachable from initial range
    {
        xVal = abs(nodeArray[0]->x - nodeArray[i]->x);
        yVal = abs(nodeArray[0]->y - nodeArray[i]->y);
        distance = sqrt((xVal * xVal) + (yVal * yVal));
        if (distance <= info->initRange)
        {
            resetNodes(nodeArray, nodeCount);
            DFS(nodeArray[i], info, 1, 0, info->from);
        }
    }

    for (int i = 0; i <= info->bestPathLength; i++) // print out best path and best Healing
    {
        printf("%s %d\n", info->best_path[i]->name, info->healing[i]);
    }
    printf("Total_Healing %d\n", info->bestHealing);

    // healing, all the nodes, best path, info, curr, from

    //free all allocated memory
    for (int i = 0; i < nodeCount; i++)
    {
        free(nodeArray[i]->adj);
    }

    for (int i = 0; i < nodeCount; i++)
    {
        free(nodeArray[i]->name);
        free(nodeArray[i]);
    }

    free(info->best_path);
    free(nodeArray);
    free(info->healing);
    free(previous);
    free(info);

    return 0;
}
