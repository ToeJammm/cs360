/*
Jake Marlow
famtree.c
cs360
2/5/2024
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "fields.h"
#include "dllist.h"
#include "jrb.h"

typedef struct person
{
    char *name;
    char sex;
    char *father;
    char *mother;
    char *childrenList;
    int visited;
    int unParents;
} Person;

// use IS to read standard input, store structs in "people" RB tree keyed on name, val is the struct

char *readName(IS is)
{
    char *name = (char *)malloc(256);
    for (int i = 1; i < is->NF; i++) // read in full name
    {

        strcat(name, is->fields[i]);
        if (i <= is->NF)
        {
            strcat(name, " ");
        }
    }
    return name;
}

int main()
{
    int i = 0;
    JRB people;
    IS is;
    Person *p;
    char *name;
    is = new_inputstruct(NULL);
    people = make_jrb();

    while (get_line(is) >= 0)
    {
        // printf("this line: %s\n", is->text1);
        if (is->NF > 0 && strcmp(is->fields[0], "PERSON") == 0) //if person line, read name
        {
            name = readName(is);    //read name function, returns name
            if (jrb_find_str(people, name) == NULL) // if not found in tree, make struct
            {

                p = malloc(sizeof(Person));
                p->name = strdup(name); // make space for name
                printf("%s's struct is being created\n", p->name);

                while (get_line(is) >= 0 && is->NF > 0)
                {

                    if (strcmp(is->fields[0], "SEX") == 0)
                    {
                        p->sex = *is->fields[1];
                        printf("%s's sex: %c\n", p->name, p->sex);
                    }
                    if (strcmp(is->fields[0], "FATHER") == 0)
                    {
                        if(!p->sex) { //if sex wasn't included but father was
                        p->sex = 'M';
                        }

                        p->father = readName(is); //set fathers name
                        printf("%s's father is %s\n", p->name, p->father);
                        if (jrb_find_str(people, is->fields[1]) == NULL)
                        {
                            printf("%s's father is already in the tree\n", p->name);
                        }
                        else
                        {
                            printf("%s's father is not in the tree\n", p->name);
                            // add father's struct into the tree
                        }
                    }
                    // if(strcmp(is->fields[0], "MOTHER") == 0) {
                    //     p->mother = readName(is, name);
                    // }
                }
            }
        }
    }
    return 0;
}
