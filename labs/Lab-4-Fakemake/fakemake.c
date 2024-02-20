#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "fields.h"
#include "dllist.h"
#include "jrb.h"

//gcc -I/home/jplank/cs360/include -o fakemake fakemake.c /home/jplank/cs360/lib/libfdr.a

int main(int argc, char **argv) {

FILE *file;
Jval fileName;
IS is;
int read;



    if(argc > 1) {
        fileName.s = argv[1];
    } else {
        fileName.s = "fmakefile";
    }
    printf("description file: %s\n", fileName.s);

    is = new_inputstruct(fileName.s);

    if(file != NULL) {
        printf("file opened successfully\n");
    } else {
        printf("file failed to open\n");
        return -1;
    }

     while(get_line(is) >= 0) {
       printf("%s", is->text1);
  }

    
}
