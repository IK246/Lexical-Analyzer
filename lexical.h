#ifndef LEXICAL_H
#define LEXICAL_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// color codes
#define R "\x1b[31m"
#define G "\x1b[32m"
#define Y "\x1b[33m"
#define C "\x1b[36m"
#define Re "\x1b[0m"

// enum for return status of the functions
typedef enum status
{

    e_failure, // --> 0
    e_success // --> 1

} status;

// enum for different types of tokens
typedef enum parser
{

    e_special,
    e_keyword,
    e_string,
    e_float_const,
    e_int_const,
    e_ascii_const,
    e_operator,
    e_identifier,
    e_others,
    e_idle,

} parser;

// Function Prototypes
status validate_and_open_file(char *, FILE **);
status do_scan(FILE *);
status tokens(FILE *);
status check_event();

// functions to get the tokens
status preprocessor(char *, FILE *);
status literal(char *, FILE *);
status keyword(char *, FILE *);
status check_keyword(char *);
status num_const(char *, FILE *);
status char_const(char *, FILE *);
status check_operator(char *, FILE *);
status single_operator(char);
status check_special(char);

#endif
