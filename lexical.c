#include "lexical.h"

// Global Variables
int line = 1, i=0, round_bracket = 0, square_bracket = 0, curly_bracket = 0;
char arr[200];

status validate_and_open_file(char *file_name, FILE **fptr) // Validating and opening the file
{
    printf(C "INFO:" Re " Opening %s File\n", file_name);
    char *ptr;
    if((ptr = strstr(file_name, ".c")) != NULL) // Checking for .c extension
    {
        if(!strcmp(ptr, ".c")) // Checking if .c is at the end
        {
            if((*fptr = fopen(file_name, "r")) == NULL) // Opening the file in read mode
            {
                printf(R "ERROR:" Re " File %s not present\n", file_name);
                return e_failure;
            }
            puts(G "INFO:" Re " Done");
            return e_success;
        }
        else
        {
            printf(R "ERROR:" Re " File %s has extension misplaced\n", file_name);
            return e_failure;
        }
    }
    else
    {
        printf(R "ERROR:" Re " Invalid File Name %s\n", file_name);
        return e_failure;
    }
}

status do_scan(FILE *fptr) // Scanning the file for tokens
{
    char ch;
    while((ch = fgetc(fptr)) != EOF) // Reading character by character until EOF
    {
#if 1
        if(check_special(ch) == e_success) // Checking for special characters
            printf(C "Symbol           :" Re " %c\n", ch); // Printing the special character

        if(ch == '\n') // checks for new line
        {
            line++; // Incrementing line count
            continue; // Continuing to next iteration to avoid further checks
        }

        if(ch == '/') // Checking for comments
        {
            ch = fgetc(fptr);
            if(ch == '/') // Checking for single line comment
            {
                do
                {
                    ch = fgetc(fptr);
                }
                while(ch != '\n' && ch != EOF);
                if(ch == '\n')
                    line++;
                continue;
            }
            else if(ch == '*') // Checking for multi line comment
            {
                char pre;
                do
                {
                    pre = ch;
                    ch = fgetc(fptr);
                }
                while(!(ch == '/' && pre == '*') && ch != EOF);
                if(ch == EOF)
                {
                    printf(R "Line %d Error:" Re " Unterminated comment\n", line);
                    fclose(fptr);
                    return e_failure;
                }
                continue;
            }
            else // if not a comment, then it is a division operator
            {
                fseek(fptr, -1, SEEK_CUR); // Moving the file pointer one step back
                ch = '/'; // Assigning ch to '/' to check for operator
            }
        }

        if(ch == '#') // Checking for preprocessor directives
            remove_preprocessor(&ch, fptr); // funcion call to ignore preprocessor directives

        if(ch == 34) // Checking for string literals
            if(literal(&ch, fptr) == e_failure) // function call to check string literal
                return e_failure;
#endif
        //printf("hi");
        if(single_operator_check(ch) == e_success) // Checking for single character operators
            if(check_operator(&ch, fptr) == e_failure) // function call to check for operators
                return e_failure;

        if(ch >= '0' && ch <= '9') // Checking for numerical constants
            if(num_const(&ch, fptr) == e_failure) // function call to check numerical constants
                return e_failure;

        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9')) // Checking for keywords and identifiers
            if(keyword(&ch, fptr) == e_failure) // function call to check for keywords and identifiers
                return e_failure;
#if 1
        if(ch == 39) // Checking for character constants
            if(char_const(&ch, fptr) == e_failure) // function call to check for character constants
                return e_failure;

        if(ch == '$' || ch == '@') // Checking for invalid characters
        {
            printf(R "Line %d Error:" Re " stray ‘%c’ in program\n", line, ch); // Printing error message
            return e_failure;
        }
#endif
    }

    // Checking for unbalanced brackets
    if(round_bracket != 0)
    {
        printf(R "Error:" Re " expected ‘)’\n");
        return e_failure;
    }
    if(square_bracket != 0)
    {
        printf(R "Error:" Re " expected ‘]’\n");
        return e_failure;
    }
    if(curly_bracket != 0)
    {
        printf(R "Error:" Re " expected ‘}’\n");
        return e_failure;
    }

    // Closing the file
    fclose(fptr);

    return e_success;
}

status check_keyword(char *buffer) // Checking for keywords
{
    // List of keywords in C
    char *key[] = {"char", "int", "float", "double", "signed", "unsigned", "short", "long", "const", "volatile", "for", "while", "do", "goto", "break", "continue", "if", "else", "switch", "case", "default", "auto", "registor", "static", "extern", "struct", "union", "enum", "typedef", "void", "return", "sizeof"};

    for(int i=0; i<32; i++) // loop to check if the buffer matches any keyword or not
        if(!strcmp(buffer, key[i])) // Comparing the buffer with each keyword
            return e_success;
    return e_failure;
}

status check_special(char ch) // Checking for special characters
{
    // checks for the brackets
    if(ch == '(')
        round_bracket++;
    else if(ch == ')')
        round_bracket--;
    else if(ch == '[')
        square_bracket++;
    else if(ch == ']')
        square_bracket--;
    else if(ch == '{')
        curly_bracket++;
    else if(ch == '}')
        curly_bracket--;
    // checks for other special characters
    else if(ch == ':' || ch == ';')
        return e_success;
    else
        return e_failure;
    return e_success;
}

status remove_preprocessor(char *ch, FILE *fptr) // function to ignore preprocessor directives
{
    do
    {
        *ch = fgetc(fptr);
    }
    while(*ch != '\n'); // loop till new line character
    line++; // incrementing the line count
    return e_success;
}

status literal(char *ch, FILE *fptr)
{
    int i=0;
    do
    {
        arr[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(*ch != 34 && *ch != EOF && *ch != '\n');
    if(*ch == EOF || *ch == '\n')
    {
        if(*ch == '\n')
            line++;
        printf(R "Line %d Error:" Re " Missing terminating %c character\n", line, 34);
        return e_failure;
    }
    arr[i++] = 34;
    printf(C "Literal          : " Re);
    for(int j=0; j<i; j++)
        printf("%c", arr[j]);
    printf("\n");
    return e_success;
}

status keyword(char *ch, FILE *fptr) // function to check for keywords and identifiers
{
    int i=0;
    do
    {
        arr[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(isalnum(*ch) || *ch == '_'); // loop till alphanumeric or underscore
    arr[i] = '\0'; // assgining null character at the end of the string
    fseek(fptr, -1, SEEK_CUR); // moving the file pointer one step back

    if(check_keyword(arr) == e_success) // checking if the string is a keyword
        printf(C "Keyword          :" Re " %s\n", arr);
    else if(isalpha(arr[0]) || arr[0] == '_') // checking if the string is a valid identifier
        printf(C "Identifier       :" Re " %s\n", arr);
    else
    {
        if(strlen(arr) == 1) // if the length of the string is 1, then it is a number constant
        {
            *ch = arr[0];
            return e_success;
        }
        printf(R "Line %d Error:" Re " invalid identifier %s \n", line, arr);
        return e_failure;
    }
    return e_success;
}
