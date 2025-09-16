#include "constant.h"

int float_flag = 0;
char val[50];
extern int line;

status num_const(char *ch, FILE *fptr) // function to check for numerical constants
{
    int i = 0;

    do
    {
        val[i++] = *ch;
        if((*ch = fgetc(fptr)) == '.')  // checks for point to identify float constants
            float_flag = 1;             // set the float flag
    }
    while(isalnum(*ch) || *ch == '.');  // loop till alphanumeric or dot for float

    fseek(fptr, -1, SEEK_CUR);  // moving the file pointer one step back
    *ch = -1;                   // setting ch to -1 to avoid further checks in main loop
    val[i] = '\0';

    int j=0;
    while(val[j])
    {
        if((val[1] != 'X' || val[1] != 'x' || !float_flag) && (isalpha(val[j] && j != 1 && j != i)))
        {
            printf(R "Line %d Error:" Re "invalid suffix %c%s%c on integer constant\n", line, 34, val+j, 34);
            return e_failure;
        }
        j++;
    }

    /* ---------------- VALIDATION ---------------- */
    for(int j = 0; j < i; j++)
    {
        if(((val[1] == 'X' || val[1] == 'x') && (j != 1)) && ((val[j] >= 'G' && val[j] <= 'Z') || (val[j] >= 'g' && val[j] <= 'z')))
        {
            printf(R "Line %d Error:" Re " invalid suffix %c%c%c on integer constant\n", line, 39, val[j], 39);
            return e_failure;
        }

        else if((val[1] == 'B' || val[1] == 'b') && (!(val[j] == '0' || val[j] == '1') && j != 1))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in binary constant\n", line, 34, val[j], 34);
            return e_failure;
        }

        else if((val[0] == '0' && val[1] != 'x' && val[1] != 'X' && !float_flag) && !(val[j] >= '0' && val[j] <= '7'))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in octal constant\n", line, 34, val[j], 34);
            return e_failure;
        }

        else if((val[0] != '0' || (val[0] == '0' && val[1] == '\0')) && !float_flag && !(val[j] >= '0' && val[j] <= '9'))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in decimal constant\n", line, 34, val[j], 34);
            return e_failure;
        }

        else if(float_flag && !((val[j] >= '0' && val[j] <= '9') || val[j] == '.' || val[j] == 'f' || val[j] == 'F'))
        {
            printf(R "Line %d Error:" Re " invalid suffix %c%c%c on floating constant\n", line, 34, val[j], 34);
            return e_failure;
        }
    }

    if(float_flag) // checks if float flag is set or not
    {
        printf(C "Float Const      : " Re);
        float_flag = 0;
    }
    else // if not, it is an integer constant
    {
        printf(C "Integer Const    : " Re);
    }

    for(int j = 0; j < i; j++) // printing the constant's characters by character
        printf("%c", val[j]);
    printf("\n");

    return e_success;
}
status char_const(char *ch, FILE *fptr) // function to check for character constants
{
    int i=0;
    do
    {
        val[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(*ch != 39 && *ch != '\n'); // loop till single quote or new line character
    val[i++] = 39; // adding the closing single quote
    val[i] = '\0'; // adding the null charater
    if(*ch == '\n') // checks the new line character to find the error
    {
        line++; // incrementing the line count
        printf(R "Line %d Error:" Re " missing terminating %c character\n", line, 39);
        return e_failure;
    }
    if(i == 3) // if the i == 3, the it is a valid charater constant
        printf(C "Character Const  :" Re " %s\n", val);
    else
    {
        if(!strcmp(val, "'\n'") || !strcmp(val, "'\0'") || !strcmp(val, "'\t'") || !strcmp(val, "'\''")); // checking for escape sequences
        else if(i == 4) // if i == 4, then it is a multi-character constant
        {
            printf(C "Character Const  :" Re " %s\n", val);
            return e_success;
        }
        printf(Y "Line %d Warning:" Re " Multi-character character constant\n", line);
        return e_failure;
    }
    return e_success;
}
