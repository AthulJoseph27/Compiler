#include <stdio.h>
#include "myscanner.h"

extern int yylex();
extern int yylineno;
extern char *yytext;

char *names[] = {NULL, "FUN", "if", "else", "do", "while", "read", "write"};

int main(void)
{
    int ntoken, vtoken;

    ntoken = yylex();
    while (ntoken)
    {
        printf("%d\n", ntoken);
        ntoken = yylex();
    }

    return 0;
}