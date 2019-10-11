#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    if(argc != 2){
        printf("Error: Invalid number of arguments, usage sleep <number>\n");
        exit();
    }
    if(atoi(argv[1]) < 0) {
        printf("Error: Argument must be positive number\n");
        exit();
    }
    sleep(atoi(argv[1]));
    exit();
}