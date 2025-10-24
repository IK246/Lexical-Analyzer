A Lexical Analyzer (also called a Lexer or Scanner) is the first phase of a compiler. 
Its main job is to read the source code, character by character and convert it into a sequence of tokens.
Tokens are like keywords, identifiers, operators, constants, literal.
And it will also find lexical errors like mssing terminating single quote ('), invalid constants (0xAGF).

For Example:
    Sample -->   int x = 10;
        Keyword: int
        Identifier: x
        Operator: = 
        Constant: 10
