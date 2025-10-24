#include "lexical.h"

// getting the external variables from other files
extern uint32_t line, round_bracket, square_bracket, curly_bracket;
extern parser event;
extern uint8_t buffer[100];

// function to get the preprocessor directives
status preprocessor(char *ch, FILE *fptr)
{
    int i=0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(*ch != EOF && *ch != '\n');

    // move the function pointer one step back
    fseek(fptr, -1, SEEK_CUR);
    buffer[i] = '\0';
    return e_success;
}

// function to get the string literals and header files
status literal(char *ch, FILE *fptr)
{
    char pre = *ch;
    int i=0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(*ch != 34 && *ch != EOF && *ch != '\n' && !(pre == 60 && *ch == 62));

    if(*ch == EOF || *ch == '\n')
    {
        if(*ch == '\n')
          line++;
        printf(R "Line %d Error:" Re " Missing terminating %c character\n", line, 34);
        return e_failure;
    }

    // if the literal is a header file, add '>' at the end
    if (pre == 60)
        buffer[i++] = 62;
    // if not add " at the end
    else
        buffer[i++] = 34;
    buffer[i] = '\0';
    *ch = -1;
    return e_success;
}

// function to check for keywords and identifiers
status keyword(char *ch, FILE *fptr)
{
    int i=0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while((isalnum(*ch) || *ch == '_') && *ch != EOF);
    buffer[i] = '\0';

    // move the function pointer one step back
    fseek(fptr, -1, SEEK_CUR);
    *ch = -1;

    // checking if the buffer matches any keyword or not
    if(check_keyword(buffer) == e_success)
        return e_success;
    else if(isalpha(buffer[0]) || buffer[0] == '_')
        event = e_identifier;
    else
    {
        if(strlen(buffer) == 1)
        {
            *ch = buffer[0];
            return e_success;
        }
        printf(R "Line %d Error:" Re " invalid identifier %s \n", line, buffer);
        return e_failure;
    }
    return e_success;
}

// list of keywords in C language
char *data_key[] = {"char", "int", "float", "long", "short", "double", "struct", "union", "typedef", "signed", "unsigned", "void", "auto", "extern", "registor", "static", "const", "volatile", "enum", "uint32_t", "uint8_t", "uint16_t"};

char *control_key[] = {"if", "else", "switch", "case", "default", "for", "while", "do", "goto", "break", "continue", "return", "sizeof"};

// function to check if the buffer matches any keyword or not
status check_keyword(char *buffer) // Checking for keywords
{
    for(int i=0; i<18; i++) // loop to check if the buffer matches any keyword or not
        if(!strcmp(buffer, data_key[i])) // Comparing the buffer with each keyword
        {
            event = e_keyword;
            return e_success;
        }

    for(int i=0; i<12; i++) // loop to check if the buffer matches any keyword or not
        if(!strcmp(buffer, control_key[i])) // Comparing the buffer with each keyword
        {
            event = e_keyword;
            return e_success;
        }
    return e_failure;
}

// function to check for numerical constants
status num_const(char *ch, FILE *fptr)
{
    uint8_t i=0, float_flag = 0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(isalnum(*ch) || *ch == '.');

    // move the function pointer one step back
    fseek(fptr, -1, SEEK_CUR);
    *ch = -1;
    buffer[i] = '\0';

    uint32_t j=0;
    while(buffer[j])
    {
        // checking if the number is a floating point number
        if((buffer[1] != 'X' || buffer[1] != 'x' || !float_flag) && (isalpha(buffer[j] && j != 1 && j != i)))
        {
            printf(R "Line %d Error:" Re "invalid suffix %c%s%c on integer constant\n", line, 34, buffer+j, 34);
            return e_failure;
        }
        j++;
    }

    for(int j = 0; j < i; j++)
    {
        // checking for multiple decimal points in a floating point number
        if(((buffer[1] == 'X' || buffer[1] == 'x') && (j != 1)) && ((buffer[j] >= 'G' && buffer[j] <= 'Z') || (buffer[j] >= 'g' && buffer[j] <= 'z')))
        {
            printf(R "Line %d Error:" Re " invalid suffix %c%c%c on integer constant\n", line, 39, buffer[j], 39);
            return e_failure;
        }

        // checking for the binary constant is valid or not
        else if((buffer[1] == 'B' || buffer[1] == 'b') && (!(buffer[j] == '0' || buffer[j] == '1') && j != 1))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in binary constant\n", line, 34, buffer[j], 34);
            return e_failure;
        }
        
        // checking if the octal constant is valid or not
        else if((buffer[0] == '0' && buffer[1] != 'x' && buffer[1] != 'X' && !float_flag) && !(buffer[j] >= '0' && buffer[j] <= '7'))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in octal constant\n", line, 34, buffer[j], 34);
            return e_failure;
        }

        // checking if the decimal constant is valid or not
        else if(buffer[j] == '.')
            float_flag++;
        else if((buffer[0] != '0' || (buffer[0] == '0' && buffer[1] == '\0')) && !float_flag && !(buffer[j] >= '0' && buffer[j] <= '9'))
        {
            printf(R "Line %d Error:" Re " invalid digit %c%c%c in decimal constant\n", line, 34, buffer[j], 34);
            return e_failure;
        }

        // checking if the floating constant is valid or not
        else if(float_flag && !((buffer[j] >= '0' && buffer[j] <= '9') || buffer[j] == '.' || buffer[j] == 'f' || buffer[j] == 'F'))
        {
            printf(R "Line %d Error:" Re " invalid suffix %c%c%c on floating constant\n", line, 34, buffer[j], 34);
            return e_failure;
        }
    }

    if(float_flag) // checks if float flag is set or not
        event = e_float_const;
    else // if not, it is an integer constant
        event = e_int_const;
    return e_success;
}

// function to check for character constants
status char_const(char *ch, FILE *fptr)
{
    int i=0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(*ch != 39 && *ch != '\n');

    // adding the ' at the end
    buffer[i++] = 39;
    buffer[i] = '\0';

    if(*ch == '\n') // checks the new line character to find the error
    {
        line++; // incrementing the line count
        printf(R "Line %d Error:" Re " missing terminating %c character\n", line, 39);
        return e_failure;
    }

    if(i == 3) // if the i == 3, the it is a valid charater constant
        return e_success;
    else
    {
        if(!strcmp(buffer, "'\n'") || !strcmp(buffer, "'\0'") || !strcmp(buffer, "'\t'") || !strcmp(buffer, "'\''")) // checking for escape sequences
            return e_success;
        else if(i == 4) // if i == 4, then it is a multi-character constant
            return e_success;
        printf(Y "Line %d Warning:" Re " Multi-character character constant\n", line);
        return e_failure;
    }
    return e_success;
}

// list of multi-character operators in C language
char *multi_operator[] = {"->", "++", "--", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "*=", "/=", "%=", "+=", "-=", "&=", "|=", "^=", "<<=", ">>=", "**"};

// function to check for operators
status check_operator(char *ch, FILE *fptr)
{
    int i=0;
    do
    {
        buffer[i++] = *ch;
        *ch = fgetc(fptr);
    }
    while(single_operator(*ch) == e_success);

    // move the function pointer one step back
    fseek(fptr, -1, SEEK_CUR);
    *ch = -1;
    buffer[i] = '\0';

    if(i == 1)
        return e_success;
    else if(i != 1)
    {
        int j;
        // loop to check if the buffer matches any multi-character operator or not
        for(j=0; j<22; j++)
        {
            if(!strcmp(buffer, multi_operator[j]))
                return e_success;
        }
        printf(R "Line %d Error:" Re " Invalid operator(%s)\n", line, buffer);
        return e_failure;
    }
    return e_success;
}

// function to check for single character operators
status single_operator(char ch) // function to check for single character operators
{
    // checks if the character matches with any of the operators below
    if(ch == '-' || ch == '.' || ch == '>' || ch == '+' || ch == '&' || ch == '|' || ch == '~' || ch == '*' || ch == '%' || ch == '/' || ch == '<' || ch == '=' || ch == '?' || ch == '!' || ch == ',')
        return e_success;
    else
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
    else if(ch == ':' || ch == ';');
    else
        return e_failure;

    buffer[0] = ch;
    buffer[1] = '\0';

    return e_success;
}