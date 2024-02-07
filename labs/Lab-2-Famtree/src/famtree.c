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
    struct Person *father;
    struct Person *mother;
    JRB childrenList;
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
    JRB people, rNode;
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

                p = (Person *) malloc(sizeof(Person));
                p->name = strdup(name); // make space for name
                printf("%s's struct is being created\n", p->name);

                while (get_line(is) >= 0 && is->NF > 0)
                {

                    if (strcmp(is->fields[0], "SEX") == 0) //if sex is available, set sex
                    {
                        p->sex = *is->fields[1];
                        printf("%s's sex: %c\n", p->name, p->sex);
                    }
                    if (strcmp(is->fields[0], "FATHER") == 0) //if father field is available
                    {
                        char *dadName = readName(is);
                        printf("%s's father is %s\n", p->name, dadName);
                        rNode = jrb_find_str(people, dadName);
                        if(rNode != NULL) { //if father is already in tree
                           printf("%s's father is already in the tree\n", p->name); 
                           p->father = (Person *) rNode->val.v; //ISSUE
                           if(jrb_find_str(p->father->childrenList, p->name) == NULL) { //if p is not in childrenList of father
                            jrb_insert_str(p->father->childrenList, p->name, new_jval_v(p)); //insert into children list (ISSUE)
                           } else {
                            p->father = (Person *) rNode->val.v;
                           }
                        } else { //father is not in tree
                            printf("%s's father is not in the tree\n", p->name);
                            Person *father = (Person *) malloc(sizeof(Person));
                            p->father = father;
                            free(father);
                            p->father->name = dadName;
                            jrb_insert_str(p->father->childrenList, p->name, new_jval_v(p));
                            jrb_insert_str(people, p->father->name, new_jval_v(p->father));
                        }
                    }
                    // if(strcmp(is->fields[0], "MOTHER") == 0) {
                    //     char *momName = readName(is);
                    //     printf("%s's mother is %s", p->name, momName);
                    //     rNode = jrb_find_str(people, momName);
                    //     if(rNode != NULL) {
                    //         printf("%s's mother is already in the tree\n", p->name);
                    //         p->mother = (Person *) rNode->val.v;
                    //         if(jrb_find_str(p->mother->childrenList, p->name) == NULL) { //if p is not in childlist
                    //             jrb_insert_str(p->mother->childrenList, p->name, new_jval_v(p)); //isnert p into list of children
                    //         } else {
                    //             //do nothing
                    //             //don't need to check if P has mother set bc its a new node
                    //             p->mother = (Person *) rNode->val.v; //set p's mother
                    //         }
                    //     } else { //if mother does not yet exist
                    //         printf("%s's mother is not in the tree\n", p->name);
                    //         Person *mother = (Person *) malloc(sizeof(Person));
                    //         mother->name = momName; 
                    //         p->mother = mother; //set mothers name
                    //         free(mother); //free mother asap
                    //         jrb_insert_str(p->mother->childrenList, p->name, new_jval_v(p)); //insert p into list of children
                    //         jrb_insert_str(people, p->mother->name, new_jval_v(p->mother));
                    //     }
                        
                    // }
                    // if(strcmp(is->fields[0], "FATHER_OF") == 0) {
                    //     char* childName = readName(is);


                    // }

                }
                printf("Inserting %s into the family tree\n", p->name);
                jrb_insert_str(people, p->name, p);
            }
        }
    }
    return 0;
}
