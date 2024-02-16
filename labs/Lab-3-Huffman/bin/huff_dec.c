
/*
Jake Marlow
cs360
huff_dec.c
*/
#include <stdio.h> //we will use fread() to read in both the decode and encoding files
#include <string.h>
#include <stdlib.h>

typedef struct huff_node
{
    struct huff_node *zero;
    struct huff_node *one;
    char *s_zero;
    char *s_one;
    int visited;
} Huff_node;

Huff_node *creatNode() // creates new node
{

    Huff_node *new = (Huff_node *)malloc(sizeof(Huff_node)); // create new Node
    new->s_one = NULL;
    new->s_zero = NULL;
    new->zero = NULL;
    new->one = NULL;
    new->visited = 0;
    return new;
}

void popFirstChar(char *str) // pops first char off the front of a sequence
{
    if (str == NULL || *str == '\0')
    {
        return;
    }

    // Shift all characters one position to the left
    for (int i = 0; str[i] != '\0'; i++)
    {
        str[i] = str[i + 1];
    }
}

void freeTree(Huff_node *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->visited == 1)
    {
        return;
    }
    node->visited = 1;

    if (node->one != NULL)
    {
        freeTree(node->one);
    }

    if (node->zero != NULL)
    {
        freeTree(node->zero);
    }

    free(node->s_one);
    free(node->s_zero);
    free(node);

    //node->visited = 0;
    return;
}

// this function finds where a word is to go in the tree and inserts it
void inTree(char *word, char *sequence, Huff_node *node)
{

    if ((sequence[1]) == '\0')
    { // if sequence is too short
        if (sequence[0] == '1')
        {
            node->s_one = strdup(word);
            return;
        }
        else
        {
            node->s_zero = strdup(word);
            return;
        }
    }

    if (sequence[2] == '\0')
    {                           // reached node
        if (sequence[0] == '1') // if node number is 1
        {
            if (sequence[1] == '1') // string goes in s_one
            {
                if (node->one != NULL)
                {
                    node->one->s_one = strdup(word);
                    return;
                }
                else
                { // node doesn't exist yet
                    Huff_node *new = creatNode();
                    node->one = new;
                    node->one->s_one = strdup(word);
                    return;
                }
            }
            else // string goes in s_zero
            {
                if (node->one != NULL)
                {
                    node->one->s_zero = strdup(word);
                    return;
                }
                else
                { // node doesn't exist yet
                    Huff_node *new = creatNode();
                    node->one = new;
                    node->one->s_zero = strdup(word);
                    return;
                }
            }
        }
        else
        {
            if (sequence[1] == '1') // string goes in s_one
            {
                if (node->zero != NULL)
                {
                    node->zero->s_one = strdup(word);
                    return;
                }
                else
                { // node doesn't exist yet
                    Huff_node *new = creatNode();
                    node->zero = new;
                    node->zero->s_one = strdup(word);
                    return;
                }
            }
            else // string goes in s_zero
            {
                if (node->zero != NULL)
                {
                    node->zero->s_zero = strdup(word);
                    return;
                }
                else
                { // node doesn't exist yet
                    Huff_node *new = creatNode();
                    node->zero = new;
                    node->zero->s_zero = strdup(word);
                    return;
                }
            }
        }
    }
    else // if length is too long still
    {
        if (sequence[0] == '1')
        {
            if (node->one == NULL)
            {                                 // node doesnt exist
                Huff_node *new = creatNode(); // create new Node
                node->one = new;
                popFirstChar(sequence);
                inTree(word, sequence, node->one);
            }
            else
            { // node exists
                popFirstChar(sequence);
                inTree(word, sequence, node->one);
            }
        }
        else
        {
            if (node->zero == NULL)
            {                                 // node doesnt exist
                Huff_node *new = creatNode(); // create new Node
                node->zero = new;
                popFirstChar(sequence);
                inTree(word, sequence, node->zero);
            }
            else
            { // node exists
                popFirstChar(sequence);
                inTree(word, sequence, node->zero);
            }
        }
    }

    return;
}


int main(int argc, char *argv[])
{
    FILE *file;
    Huff_node *it;
    char word[256], sequence[256];
    char buffer[256];
    char temp[2];
    int getRidofWarning = argc;
    getRidofWarning++;

    file = fopen(argv[1], "rb");

    int read = 0;
    int nullNum = 0;

    word[0] = '\0';
    sequence[0] = '\0';

    Huff_node *root = (Huff_node *)malloc(sizeof(Huff_node));
    root->s_one = NULL;
    root->s_zero = NULL;
    root->zero = NULL;
    root->one = NULL;
    root->visited = 0;

    while ((read = fread(buffer, 1, sizeof(buffer), file)))
    {
        for (int i = 0; i < read; i++)
        {

            if (nullNum % 2 == 0 && buffer[i] != 0)
            { // read in word
                temp[0] = buffer[i];
                temp[1] = '\0';
                strcat(word, temp);
            }
            else if (nullNum % 2 == 1 && buffer[i] != 0)
            { // read in sequence
                temp[0] = buffer[i];
                temp[1] = '\0';
                strcat(sequence, temp);
            }
            else if (buffer[i] == '\0' && nullNum % 2 == 1)
            { // print  if both are initialized

                inTree(word, sequence, root); // adds sequence into tree either in a new node or existing node

                memset(word, 0, sizeof(word));         // clear word array
                memset(sequence, 0, sizeof(sequence)); // clear sequence array

                word[0] = '\0';
                sequence[0] = '\0';
            }

            if (buffer[i] == 0)
            { // if char is null char, add 1 to switch read format
                nullNum++;
            }
        }
    }

    fclose(file);

    // printf("\n\n");

    file = fopen(argv[2], "rb");

    read = 0;

    // read in encoded message and decode it using the tree

    fseek(file, 0, SEEK_END);
    unsigned int fileSize = ftell(file);

    if (fileSize < 4)
    {
        fprintf(stderr, "Error: file is not the correct size.\n");
        fclose(file);
        return -1;
    }

    fseek(file, fileSize - 4, SEEK_SET);
    unsigned int totalBits = 0;
    int bitsToRead;
    fread(&totalBits, 4, 1, file); // find input length
    if (totalBits / 8 > fileSize - 4)
    {
        fprintf(stderr, "Error: Total bits = %u, but file's size is %d\n", totalBits, fileSize);
        fclose(file);
        return -1;
    }

    fseek(file, 0, SEEK_SET);
    it = root;
    char tmpC;
    // printf("input Size: %d\n", fileSize); //# 20 input size is 1 so it read a one and then 7 zeros
    // which comes out as one space and seven new lines
    for (int i = 0; i < (int)fileSize - 4; i++)
    {
        fread(&tmpC, 1, 1, file);

        if ((totalBits / 8) > 0)
        { // if input size is not a factor of 8
            totalBits -= 8;
            bitsToRead = 8;
        }
        else
        {
            bitsToRead = totalBits % 8;
        }
        // printf("2 bitsToRead: %d\n", bitsToRead);
        for (int j = 0; j < bitsToRead; j++)
        {
            if ((tmpC & (1 << j)) >> j)
            {

                if (it->s_one != NULL)
                { // check if word exists
                    printf("%s", it->s_one);
                    it = root;
                }
                else if (it->one != NULL)
                { // go to child 1
                    it = it->one;
                }
                else
                {
                    fprintf(stderr, "Unrecognized bits\n");
                    fclose(file);
                    return -1;
                }
            }
            else
            { // its a 0
                if (it->s_zero != NULL)
                { // check if word exists
                    printf("%s", it->s_zero);
                    it = root;
                }
                else if (it->zero != NULL)
                { // go to child 0
                    it = it->zero;
                }
                else
                {
                    fprintf(stderr, "Unrecognized bits\n");
                    fclose(file);
                    return -1;
                }
            }
        }
    }

    freeTree(root); //free memory

    fclose(file);
    return 0;
}
