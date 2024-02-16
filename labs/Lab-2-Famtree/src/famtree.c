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
    int unprintedParents;
    int is_printed;
} Person;

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

int findChild(char *name, Dllist list) // find child in person's dllist
{
    Dllist tmp;
    Person *p;
    dll_traverse(tmp, list)
    {
        p = (Person *)tmp->val.v;
        if (strcmp(p->name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void resetNodes(JRB people) // reset nodes before dfs again
{
    JRB tmp;
    Person *p;
    jrb_traverse(tmp, people)
    {
        p = (Person *)tmp->val.v;
        p->visited = 0;
    }
    return;
}

void printDllist(Dllist list) // prints children of person
{
    Dllist tmp;
    Person *p;
    dll_traverse(tmp, list)
    {
        p = (Person *)tmp->val.v;
        printf("    %s\n", p->name);
    }
}

int cycleDetector(Person *p) // detects cycle via dfs
{
    Dllist tmp;
    Person *child;
    if (p->visited == 1)
    { // if already visited, then we have a cycle
        return 1;
    }

    p->visited = 1;

    dll_traverse(tmp, p->childrenList)
    {
        child = (Person *)tmp->val.v;
        if (cycleDetector(child) == 1)
        {
            return 1;
        }
    }

    p->visited = 0;

    return 0;
}

void topSort(JRB people)
{
    JRB tmp1;
    Dllist temp;
    Person *p;
    int num_people = 0, num_printed = 0;
    jrb_traverse(tmp1, people)
    {
        num_people++;
        ((Person *)tmp1->val.v)->is_printed = 0;
    }
    while (num_printed < num_people)
    { // while there are nodes left with unprinted parents, traverse and print

        jrb_traverse(tmp1, people)
        {
            p = (Person *)tmp1->val.v;
            if (!p->is_printed && (p->mother == NULL || p->mother->is_printed) && (p->father == NULL || p->father->is_printed))
            {
                printf("%s\n", p->name);
                if (p->sex == 'M')
                {
                    printf("  Sex: Male\n");
                }
                else if (p->sex == 'F')
                {
                    printf("  Sex: Female\n");
                }
                else
                {
                    printf("  Sex: Unknown\n");
                }
                if (p->father == NULL)
                {
                    printf("  Father: Unknown\n");
                }
                else
                {
                    printf("  Father: %s\n", p->father->name);
                }
                if (p->mother == NULL)
                {
                    printf("  Mother: Unknown\n");
                }
                else
                {
                    printf("  Mother: %s\n", p->mother->name);
                }
                if (dll_empty(p->childrenList) == 0)
                {
                    printf("  Children:\n");
                    printDllist(p->childrenList);
                }
                else
                {
                    printf("  Children: None\n");
                }
                printf("\n");
                num_printed++;
                dll_traverse(temp, p->childrenList)
                {
                    Person *d = (Person *)temp->val.v;
                    d->unprintedParents--;
                }
                p->is_printed = 1;
            }
        }
    }
}

int main()
{
    int i = 0;
    JRB people, rNode, temp, found;
    Dllist tmp;
    IS is;
    Person *p;
    char *name;
    is = new_inputstruct(NULL);
    people = make_jrb();

    while (get_line(is) >= 0)
    {
        if (is->NF > 0 && strcmp(is->fields[0], "PERSON") == 0) // if person line, read name
        {
            name = readName(is);
            found = jrb_find_str(people, name);
            if (found == NULL) // if not found in tree, make struct
            {
                p = (Person *)malloc(sizeof(Person));
                p->name = strdup(name); // make space for name
                p->sex = 'U';
                p->father = NULL;
                p->mother = NULL;
                p->visited = 0;
                p->childrenList = new_dllist();
                p->unprintedParents = 0;
                jrb_insert_str(people, p->name, new_jval_v(p));
            }
            else
            {
                p = (Person *)found->val.v; // set person pointer to found node
            }
            free(name);
            while (get_line(is) >= 0 && is->NF > 0)
            {

                if (strcmp(is->fields[0], "SEX") == 0) // if sex is available, set sex
                {
                    char *sex = (char *)strdup(is->fields[1]);
                    if (p->sex != 'U' && p->sex != *sex)
                    { // if person has a sex and sex intput doesn't match
                        fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                        return -1;
                    }
                    else
                    {
                        p->sex = *sex;
                    }
                    free(sex);
                }
                if (strcmp(is->fields[0], "FATHER") == 0) // if father field is available
                {
                    char *dadName = readName(is);
                    if (p->father != NULL && strcmp(dadName, p->father->name) != 0)
                    {
                        fprintf(stderr, "Bad input -- child with two fathers on line %d\n", is->line);
                        return -1;
                    }
                    rNode = jrb_find_str(people, dadName);
                    if (rNode != NULL)
                    { // if father is already in tree
                        p->father = (Person *)rNode->val.v;
                        if (p->father->sex != 'U' && p->father->sex != 'M')
                        { // if person has a sex and sex intput doesn't match
                            fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                            return -1;
                        }
                        p->father->sex = 'M';
                        if (findChild(p->name, p->father->childrenList) == 0)
                        { // if p is not in childrenList of father
                            dll_append(p->father->childrenList, new_jval_v((p)));
                        }
                        else
                        {
                            p->father = (Person *)rNode->val.v;
                        }
                    }
                    else
                    { // father is not in tree
                        Person *father = (Person *)malloc(sizeof(Person));
                        p->father = father;
                        p->father->sex = 'M';
                        p->father->father = NULL;
                        p->father->mother = NULL;
                        p->father->visited = 0;
                        p->father->unprintedParents = 0;
                        p->unprintedParents++;
                        p->father->name = dadName;
                        p->father->childrenList = new_dllist();
                        dll_append(p->father->childrenList, new_jval_v((p)));             // add p to childList
                        jrb_insert_str(people, p->father->name, new_jval_v((p->father))); // insert into map
                    }
                }
                if (strcmp(is->fields[0], "MOTHER") == 0) // if mother field is available
                {
                    char *momName = readName(is);
                    if (p->mother != NULL && strcmp(momName, p->mother->name) != 0)
                    {
                        fprintf(stderr, "Bad input -- child with two mothers on line %d\n", is->line);
                        return -1;
                    }
                    rNode = jrb_find_str(people, momName);
                    if (rNode != NULL)
                    { // if mother is already in tree
                        p->mother = (Person *)rNode->val.v;
                        p->unprintedParents++;
                        if (p->mother->sex != 'U' && p->mother->sex != 'F')
                        { // if person has a sex and sex intput doesn't match
                            fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                            return -1;
                        }
                        p->mother->sex = 'F';
                        if (findChild(p->name, p->mother->childrenList) == 0)
                        { // if p is not in childrenList of mother
                            dll_append(p->mother->childrenList, new_jval_v((p)));
                        }
                        else
                        {
                            p->mother = (Person *)rNode->val.v;
                        }
                    }
                    else
                    { // mother is not in tree
                        Person *mother = (Person *)malloc(sizeof(Person));
                        p->mother = mother;
                        p->mother->sex = 'F';
                        p->mother->father = NULL;
                        p->mother->mother = NULL;
                        p->mother->visited = 0;
                        p->mother->name = momName;
                        p->mother->unprintedParents = 0;
                        p->unprintedParents++;
                        p->mother->childrenList = new_dllist();
                        dll_append(p->mother->childrenList, new_jval_v((p)));             // add p to childList
                                                                                          //   printf("Inserting %s into the family tree\n", p->mother->name);
                        jrb_insert_str(people, p->mother->name, new_jval_v((p->mother))); // insert into map
                    }
                }
                if (strcmp(is->fields[0], "FATHER_OF") == 0)
                {
                    if (p->sex != 'U' && p->sex != 'M')
                    { // if person has a sex and sex intput doesn't match
                        fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                        return -1;
                    }
                    p->sex = 'M';
                    char *childName = readName(is);
                    rNode = jrb_find_str(people, childName);
                    if (rNode != NULL)
                    { // if child already in tree
                        if (findChild(childName, p->childrenList) == 0)
                        { // if child not in child list
                            Person *child = (Person *)rNode->val.v;
                            child->unprintedParents++;
                            if (child->father != NULL && strcmp(child->father->name, p->name) != 0)
                            {
                                fprintf(stderr, "Bad input -- tried to assign father of twice %d\n", is->line);
                                return -1;
                            }
                            child->father = p;
                            dll_append(p->childrenList, new_jval_v(child));
                        }
                        else
                        {
                            // do nothing
                        }
                        free(childName);
                    }
                    else
                    {                                                     // child not in tree (create child);
                                                                          //  printf("%s's child is not in the tree...making mother node\n", p->name);
                        Person *child = (Person *)malloc(sizeof(Person)); // figure out how to free
                        child->name = childName;
                        child->father = p;
                        child->mother = NULL;
                        child->sex = 'U';
                        child->visited = 0;
                        child->unprintedParents = 1;
                        child->childrenList = new_dllist();
                        dll_append(p->childrenList, new_jval_v(child));
                        jrb_insert_str(people, child->name, new_jval_v((child)));
                    }
                }
                if (strcmp(is->fields[0], "MOTHER_OF") == 0)
                {
                    if (p->sex != 'U' && p->sex != 'F')
                    { // if person has a sex and sex intput doesn't match
                        fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                        return -1;
                    }
                    p->sex = 'F';
                    char *childName = readName(is);
                    rNode = jrb_find_str(people, childName);
                    if (rNode != NULL)
                    { // if child already in tree
                        if (findChild(childName, p->childrenList) == 0)
                        { // if child not in child list
                            Person *child = (Person *)rNode->val.v;
                            child->unprintedParents++;
                            if (child->mother != NULL && strcmp(child->mother->name, p->name) != 0)
                            {
                                fprintf(stderr, "tried to assign second mother to child\n");
                                return -1;
                            }
                            child->mother = p;
                            dll_append(p->childrenList, new_jval_v(child));
                        }
                        else
                        {
                            // do nothing (child already exists and is in the list)
                        }
                        free(childName);
                    }
                    else
                    { // child not in tree (create child);

                        Person *child = (Person *)malloc(sizeof(Person)); // figure out how to free
                        child->name = childName;
                        child->mother = p;
                        child->father = NULL;
                        child->sex = 'U';
                        child->visited = 0;
                        child->unprintedParents = 1;
                        child->childrenList = new_dllist();
                        dll_append(p->childrenList, new_jval_v(child));
                        jrb_insert_str(people, child->name, new_jval_v((child)));
                    }
                }
            }
        }
    }

    jrb_traverse(temp, people) // cycle detect over all nodes
    {
        p = (Person *)temp->val.v;
        int cycle = cycleDetector(p);
        if (cycle == 1)
        {
            fprintf(stderr, "Bad input -- cycle in specification\n");
            return -1;
        }
        else
        {
            resetNodes(people);
        }
    }

    // top sort and print
    topSort(people);

    // free memory

    jrb_traverse(temp, people)
    {
        p = (Person *)temp->val.v;
        // free(p->childrenList);
        free_dllist(p->childrenList);
        free(p->name);
        free(p);
    }

    jrb_free_tree(people);

    jettison_inputstruct(is);

    return 0;
}
