#include "lexical.h"

// Global Variables
uint8_t newline[] = {'<', 'b', 'r', '>', '\n', '\0'};
uint8_t space[] = {'&', 'n', 'b', 's', 'p', ';', '\0'}, buffer[100];
uint32_t flag1 = 0;
parser event;
uint32_t line = 1, i=0, round_bracket = 0, square_bracket = 0, curly_bracket = 0;

status validate_and_open_file(char *file_name, FILE **fptr) // Validating and opening the file
{
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
            printf(C "INFO:" Re " Opening %s File\n", file_name);
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

status file_close(FILE *fptr)
{
    // closing the opened files
    fclose(fptr);
    return e_success;
}

status do_scan(FILE *fptr)
{
    // set idle state
    event = e_idle;
    int8_t ch;

    // Reading character by character until EOF
    while((ch = fgetc(fptr)) != EOF)
    {
        // Handling new line character
        if(ch == '\n')
        {
            line++;
            continue;
        }
        // Checking for special characters
        if(check_special(ch) == e_success)
            event = e_special;
        // Checking for comments
        else if(ch == '/')
        {
            uint32_t i=0;
            buffer[i++] = ch;
            ch = fgetc(fptr);
            // Single line comment
            if(ch == '/')
            {
                do
                {
                    buffer[i++] = ch;
                    ch = fgetc(fptr);
                }
                while(ch != '\n' && ch != EOF);
                fseek(fptr, -1, SEEK_CUR);
                buffer[i] = '\0';
                continue;
            }
            // Multi line comment
            else if(ch == '*')
            {
                uint8_t pre;
                do
                {
                    buffer[i++] = ch;
                    pre = ch;
                    ch = fgetc(fptr);
                    if(ch == '\n')
                        line++;
                }
                while(!(ch == '/' && pre == '*') && ch != EOF);
                if(ch == EOF)
                {
                    line++;
                    printf(R "Line %d Error:" Re " Unterminated comment\n", line);
                    fclose(fptr);
                    return e_failure;
                }
                buffer[i++] = '/';
                buffer[i] = '\0';
                continue;
            }
            else
            {
                fseek(fptr, -1, SEEK_CUR);
                ch = '/';
            }
        }
        // Checking for string literal
        if(ch == 34)
        {
            if(literal(&ch, fptr) == e_failure)
                return e_failure;
            event = e_string;
        }
        // Checking for operators
        else if(single_operator(ch) == e_success)
        {
            if(check_operator(&ch, fptr) == e_failure)
                return e_failure;
            event = e_operator;
        }
        // Checking for numerical constants
        else if(isdigit(ch))
        {
            if(num_const(&ch, fptr) == e_failure)
                return e_failure;
        }
        // Checking for character constants
        else if(ch == 39)
        {
            if(char_const(&ch, fptr) == e_failure)
                return e_failure;
            event = e_ascii_const;
        }
        // Checking for keywords and identifiers
        else if(isalpha(ch) || ch == '_')
        {
            if(keyword(&ch, fptr) == e_failure)
                return e_failure;
        }
        // Checking for preprocessor directives
        else if(ch == '#')
            preprocessor(&ch, fptr);
        check_event();
    }

    // If no tokens are found in the file
    if(flag1 == 0)
    {
        puts(R "Error:" Re " Tokens not available in the file");
        return e_failure;
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

    printf(G "Info: " Re "%d lines of codes are tokenized!!\n", line);

    // Closing the file
    if(file_close(fptr) == e_failure)
        return e_failure;
    puts(G "Info: " Re "All the files closed!!");

    return e_success;
}

// function to write the tokens to the destination file
status check_event()
{
    switch(event)
    {
        case e_keyword:
            printf(C "Keyword          :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_string:
            printf(C "Literal          :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_int_const:
            printf(C "Integer Const    :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_float_const:
            printf(C "Float Const      :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_ascii_const:
            printf(C "Character Const  :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_operator:
            printf(C "Operator         :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_identifier:
            printf(C "Identifier       :" Re " %s\n", buffer);
            flag1 = 1;
            break;
        case e_special:
            printf(C "Symbol           :" Re " %s\n", buffer);
            flag1 = 1;
            break;
    }
    event = e_idle;
    return e_success;
}
