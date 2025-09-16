/*
Name : Karthick I
Date : 30/08/25
Title : Lexical Analyzer Project
*/

#include <stdio.h>
#include "lexical.h"
#include "enum.h"

int main(int argc, char *argv[]) // CLA to get input file name
{
    if(argc == 2) // Argument Numbers should be 2
    {
        FILE *fptr;
        if(validate_and_open_file(argv[1], &fptr) == e_success) // Valid file name and opening file
        {
            if(do_scan(fptr) == e_success) // Scanning the file for tokens
                puts(G "INFO: Tokens Verified!!" Re);
            else
                printf(R "ERROR:" Re " Tokens Not Verified!!\n");
        }
    }
    else
        puts(R "ERROR:" Re " Argument Numbers should be 2");
    return 0;
}
