#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAX_ARG 512

char buffer[MAX_ARG];
char * args[MAX_ARG];
int args_length = 0;

struct cmd {

};

// Runs a command, args[0] = application
void run_command(char ** args)
{
    int pid = fork();

    if (pid > 0)
        wait();
    else if(pid == 0) {
        exec(args[0], args);
        fprintf(2, "Error: Unknown command <%s>.\n", args[0]);
    }
}

void tokenize(char * buffer)
{
    args_length = 0;
    int buffer_length = MAX_ARG;
    // Medzere a \n treba nahradit '\0', aby sme vedeli, kde zacina a konci argument
    for (int i = 0; i < buffer_length; i++)
    {
        if (buffer[i] == ' ')
            buffer[i] = '\0';

        else if (buffer[i] == '\n') {
            buffer[i] = '\0';
            buffer_length = i + 1;
            break;
        }
    }

    int pos = 0;
    for(int i = 0; i < buffer_length; i++)
    {
        // Ak najdem \0, tak som nasiel koniec stringu
        if (buffer[i] == '\0')
        {
            // Ulozim smernik na zaciatok stringu
            args[args_length++] = buffer + pos;
            pos = i + 1;
        }
    }
    // Pole, ktore odovzdavam execu musi obsahovat posledny prvok 0 == koniec
    args[args_length] = 0;
}




int main() {

    printf("@ ");

    char * tmp;
    // Citam vystup konzole do bufferu
    while((tmp = gets(buffer, MAX_ARG)) && (tmp[0] != '\0'))
    {
        tokenize(buffer);

        run_command(args);

        printf("@ ");
    }

    exit();
}