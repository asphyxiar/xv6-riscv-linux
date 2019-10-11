#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_ARG 500

char * args[MAX_ARG];
char buffer[MAX_ARG];
int args_length = 0;

// args konstrukcia 1. parameter, co sputam, posledny index args znak musi obsahovat "0" inak neprejde
void run_command(char ** args)
{
    int pid = fork();

    // dieta spusta prikaz
    if(pid == 0)
    {
        exec(args[0], args);
    }
    // rodic sluzi na zachovanie konzoly
    else if(pid > 0)
    {
        wait();
    }
}

void tokenize(char * buffer)
{
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

int main(int argc, char *argv[]){

    // Najprv ulozim fixne argumenty
    for (int i = 1; i < argc; i++)
    {
        args[args_length++] = argv[i];
    }

    char * tmp;
    // Citam vystup konzole do bufferu
    while((tmp = gets(buffer, MAX_ARG)) && (tmp[0] != '\0'))
    {
        // Tokenize teraz rozoberie buffer na casti
        tokenize(buffer);

        // Nastavit opat dlzku argumentov na povodny stav
        args_length = argc - 1;

        // Run Command
        run_command(args);
    }
    exit();
}