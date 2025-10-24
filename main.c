/*
Name : Karthick I
Date : 30/08/25
Title : Lexical Analyzer Project
*/

#include <stdio.h>
#include "lexical.h"

int main(int count, char *vector[]) // CLA to get input file name
{
    if(count == 2) // Argument Numbers should be 2
    {
        FILE *fptr;
        if(validate_and_open_file(vector[1], &fptr) == e_success) // Valid file name and opening file
        {
            if(do_scan(fptr) == e_success) // Scanning the file for tokens
                puts(G "INFO: " Re "Tokens Verified!!");
            else
                printf(R "ERROR:" Re " Tokens Not Verified!!\n");
        }
    }
    else
        puts(R "ERROR:" Re " Argument Numbers should be 2");
    return 0;
}