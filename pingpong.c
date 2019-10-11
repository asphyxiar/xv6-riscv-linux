#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
   
   int parent_fd[2];
   int child_fd[2];

   pipe(parent_fd);
   pipe(child_fd);

   int pid = fork();
   if (pid == -1) {
       fprintf(2, "Error: Fork()\n");
       exit();
   }

   char buf[1];
   // child
   if (pid == 0) {
       read(parent_fd[0], buf, 1);
       fprintf(1, "%d: received ping\n", getpid());
       write(child_fd[1], "b", 1);
   }
   // parent
   else {
       write(parent_fd[1], "x", 1);
       read(child_fd[0], buf, 1);
       fprintf(1, "%d: received pong\n", getpid());
   }
   exit();
}