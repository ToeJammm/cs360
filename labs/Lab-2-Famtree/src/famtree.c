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
    struct person *father;
    struct person *mother;
    Dllist childrenList;
    int visited;
    int unParents;
} Person;

// use IS to read standard input, store structs in "people" RB tree keyed on name, val is the struct

char *readName(IS is)
{
    char *name = (char *)malloc(256);
    name[0] = '\0';
    for (int i = 1; i < is->NF; i++) // read in full name
    {

        strcat(name, is->fields[i]);
        if (i + 1 < is->NF)
        {
            strcat(name, " ");
        }
    }
    return name;
}

int findChild(char *name, Dllist list) {
    Dllist tmp;
    Person *p;
dll_traverse(tmp, list) {
    p = (Person *) tmp->val.v;
    if(strcmp(p->name, name) == 0) {
        return 1;
    } 
}
return 0;
}

void printDllist(Dllist list) {
    Dllist tmp;
    Person *p;
dll_traverse(tmp, list) {
    p = (Person *) tmp->val.v;
    printf("    %s\n", p->name);
}
}

int main()
{
    int i = 0;
    JRB people, rNode, temp;
    Dllist tmp;
    IS is;
    Person *p;
    char *name;
    is = new_inputstruct(NULL);
    people = make_jrb(); //FREE

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
                p->childrenList = new_dllist();
                //printf("%s's struct is being created\n", p->name);

                while (get_line(is) >= 0 && is->NF > 0)
                {

                    if (strcmp(is->fields[0], "SEX") == 0) //if sex is available, set sex
                    {
                        p->sex = *is->fields[1];
                       // printf("%s's sex: %c\n", p->name, p->sex);
                    }
                    if (strcmp(is->fields[0], "FATHER") == 0) //if father field is available
                    {
                         char *dadName = readName(is);
                        // printf("%s's father is %s\n", p->name, dadName);
                         rNode = jrb_find_str(people, dadName);
                         if(rNode != NULL) { //if father is already in tree
                            p->father = (Person *) rNode->val.v;
                            printf("%s's father is already in the tree\n", p->name); 
                            if(findChild(p->name, p->father->childrenList) == 0) { //if p is not in childrenList of father
                             dll_append(p->father->childrenList, new_jval_v((p)));
                            } else {
                            p->father =  rNode->val.v;
                            }
                         } else { //father is not in tree
                             printf("%s's father is not in the tree...making father node\n", p->name);
                             Person *father = (Person *) malloc(sizeof(Person));
                             p->father = father;
                             p->father->sex = 'M';
                             //free(father); //figure out how to free this
                             p->father->name = dadName;
                             printf("setting %s to as %s's father\n", p->father->name, p->name);
                             p->father->childrenList = new_dllist();
                             dll_append(p->father->childrenList, new_jval_v((p))); //add p to childList
                             printf("Inserting %s into the family tree\n", p->father->name);
                             jrb_insert_str(people, p->father->name, new_jval_v((p->father))); //insert into map
                         }
                     }
                     if (strcmp(is->fields[0], "MOTHER") == 0) //if mother field is available
                    {
                         char *dadName = readName(is);
                        // printf("%s's mother is %s\n", p->name, dadName);
                         rNode = jrb_find_str(people, dadName);
                         if(rNode != NULL) { //if mother is already in tree
                            p->mother = (Person *) rNode->val.v; 
                            printf("%s's mother is already in the tree\n", p->name); 
                            if(findChild(p->name, p->mother->childrenList) == 0) { //if p is not in childrenList of mother
                             dll_append(p->mother->childrenList, new_jval_v((p)));
                            } else {
                            p->mother =  rNode->val.v;
                            }
                         } else { //mother is not in tree
                             printf("%s's mother is not in the tree...making mother node\n", p->name);
                             Person *mother = (Person *) malloc(sizeof(Person));
                             p->mother = mother;
                             p->mother->sex = 'F';
                             //free(mother); //figure out how to free this
                             p->mother->name = dadName;
                             printf("setting %s to as %s's mother\n", p->mother->name, p->name);
                             p->mother->childrenList = new_dllist();
                             dll_append(p->mother->childrenList, new_jval_v((p))); //add p to childList
                             printf("Inserting %s into the family tree\n", p->mother->name);
                             jrb_insert_str(people, p->mother->name, new_jval_v((p->mother))); //insert into map
                         }
                     }                   
                    if(strcmp(is->fields[0], "FATHER_OF") == 0) {
                        p->sex = 'M';
                        char* childName = readName(is);
                        rNode = jrb_find_str(people, childName);
                        if(rNode != NULL) { //if child already in tree
                        printf("%s's child %s is already in the tree\n");
                            if(findChild(childName, p->childrenList) == 0) { //if child not in child list
                                    Person *child = (Person *) rNode->val.v;
                                    child->father = p;
                                    dll_append(p->childrenList, new_jval_v(child));
                            }                             
                        } else { //child not in tree (create child);
                            printf("%s's child is not in the tree...making mother node\n", p->name); 
                            Person *child = (Person *) malloc(sizeof(Person)); //figure out how to free
                            child->name = childName;
                            child->father = p;
                            child->childrenList = new_dllist();
                            dll_append(p->childrenList, new_jval_v(child));
                            jrb_insert_str(people, child->name, new_jval_v((child)));
                        }
                    }
                    

                }
                printf("Inserting %s into the family tree\n", p->name);
                jrb_insert_str(people, p->name, new_jval_v(p));
            }
        } else { //if found in tree;

        }
    }

    //print family tree
printf("\n\n");
    jrb_traverse(temp, people) {
    p = (Person *) temp->val.v;
    printf("%s\n", p->name);
    if(p->sex == 'M') {
        printf("  Sex: Male\n");
    } else if(p->sex =='F') {
        printf("  Sex: Female\n");
    } else {
        printf("  Sex: Unknown\n");
    }
    if(p->mother == NULL) {
        printf("  Mother: unknown\n");
    } else {
        printf("  Mother: %s\n", p->mother->name);
    }
    if(p->father == NULL) {
        printf("  Father: unknown\n");
    } else {
        printf("  Father: %s\n", p->father->name);
    }
    if(dll_empty(p->childrenList) == 0) {
        printf("  children:\n");
        printDllist(p->childrenList);
    } else {
        printf("  children: None\n");
    }
    printf("\n");
  }
    return 0;
}
