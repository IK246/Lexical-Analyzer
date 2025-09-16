#include "operator.h"

extern int line; // get line count variable from another file

// list of multi-character operators
char *multi_operator[] = {"->", "++", "--", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "*=", "/=", "%=", "+=", "-=", "&=", "|=", "^=", "<<=", ">>=", "**"}, oper[50];

status check_operator(char *ch, FILE *fptr) // function to check for operators
{
    int i=0;
    do
    {
        oper[i++] = *ch; // stores the operator in the buffer
        *ch = fgetc(fptr); // gets the next character from file
    }
    while(single_operator_check(*ch) == e_success); // loop till the character is not an operator
    fseek(fptr, -1, SEEK_CUR); // moving the file pointer one step back
    *ch = -1; // setting ch to -1 to avoid further checks in main loop
    oper[i] = '\0';
    if(i == 1) // if the operator is single character operaotr
        printf(C "Operator         :" Re " %s\n", oper);
    else
    {
        int j;
        for(j=0; j<22; j++) // loop to check if the multi-character operaotr is valid or not
            if(!strcmp(oper, multi_operator[j])) // comparing the operator with the list of multi-charater operator array
            {
                printf(C "Operator         :" Re " %s\n", oper);
                return e_success;
            }
        if(j == 22) // if the operator is not found in the list, then it is invalid operator
        {
            printf(R "Line %d Error:" Re " Invalid operator(%s)\n", line, oper);
            return e_failure;
        }
    }
}

status single_operator_check(char ch) // function to check for single character operators
{
    // checks if the character matches with any of the operators below
    if(ch == '-' || ch == '.' || ch == '>' || ch == '+' || ch == '&' || ch == '|' || ch == '~' || ch == '*' || ch == '%' || ch == '/' || ch == '<' || ch == '=' || ch == '?' || ch == '!' || ch == ',')
        return e_success;
    else
        return e_failure;
}
