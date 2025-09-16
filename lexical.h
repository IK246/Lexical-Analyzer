#ifndef LEXICAL_H
#define LEXICAL_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "enum.h"
#include "constant.h"
#include "operator.h"

// color codes
#define R "\x1b[31m"
#define G "\x1b[32m"
#define Y "\x1b[33m"
#define C "\x1b[36m"
#define Re "\x1b[0m"

// Function Prototypes
status validate_and_open_file(char *, FILE **);
status do_scan(FILE *);
status check_special(char);
status check_keyword(char *);
status literal(char *, FILE *);
status keyword(char *, FILE *);
status remove_preprocessor(char *, FILE *);

#endif
