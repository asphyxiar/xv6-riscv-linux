#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAX_ARG 512
#define OUTPUT (O_WRONLY | O_CREATE)
#define INPUT O_RDONLY

char buffer[MAX_ARG];
char * args[MAX_ARG];
int args_length = 0;

// Runs a command, args[0] = application
void run_command(char ** args)
{
    exec(args[0], args);
    fprintf(2, "Error: Unknown command <%s>.\n", args[0]);
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

void redirect(char * input, char * output)
{
    if(input != 0)
    {
        close(0);
        int fd = open(input, INPUT);
        if(fd < 0)
            fprintf(2, "Error: Input redirection failed.\n");
    }

    if(output != 0)
    {
        close(1);
        int fd = open(output, OUTPUT);
        if(fd < 0)
            fprintf(2, "Error: Output redirection failed.\n");
    }
}

void parse(char ** args)
{
    char ** rightpipe = 0;
    int haspipe = 0;
    int hasredirect = 0;
    char * input = 0;
    char * output = 0;

    for (int i = 0; i < args_length; i++)
    {
        if((strcmp(args[i], "|") == 0) && (args_length > i+1))
        {
            haspipe = 1;
            rightpipe = args + i + 1;
            args[i] = 0;
        }
        if((strcmp(args[i], "<") == 0) && (args_length > i+1))
        {
            hasredirect = 1;
            input = args[i+1];
            args[i] = 0;
        }
        if((strcmp(args[i], ">") == 0) && (args_length > i+1))
        {
            hasredirect = 1;
            output = args[i+1];
            args[i] = 0;
        }
    }

    if(hasredirect == 1 && haspipe == 0)
    {
        int pid = fork();
        if (pid == 0)
        {
            redirect(input,output);
            run_command(args);
        }
        else
        {
            wait(0);
        }
    }

    else if(haspipe == 1)
    {
        int fds[2];
        pipe(fds);
        int pid = fork();
        if (pid == 0)
        {
            close(0);
            dup(fds[0]);
            close(fds[0]);
            redirect(input,output);
            run_command(args);
        }
        int pid2 = fork();
        if (pid2 == 0)
        {
            close(1);
            dup(fds[1]);
            close(fds[1]);
            run_command(rightpipe);
        }
        wait(0);
        wait(0);
    }

    else
    {
        int pid = fork();
        if (pid == 0)
            run_command(args);
        else
            wait(0);
    }
}




int main() {

    printf("@ ");

    char * tmp;
    // Citam vystup konzole do bufferu
    while((tmp = gets(buffer, MAX_ARG)) && (tmp[0] != '\0'))
    {
        tokenize(buffer);

        parse(args);

        printf("@ ");
    }

    exit(0);
}
