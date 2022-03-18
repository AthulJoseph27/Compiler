%{
    #include "parser.h"
    #include "y.tab.h"
    #include <string.h>
    int yylex(void);
    int yyerror(char* s);

%}

%%
"return"    {return RETURN;}
"FUN"       {return FUN;}
"read"      {return READ_VALUE;}
"write"     {return WRITE_VALUE;}
"if"        {return IF;}
"else"      {return ELSE;}
">="        {return GE;}
"<="        {return LE;}
"=="        {return EE;}
"!="        {return NE;}
"&&"        {return AND;}
"||"        {return OR;}

[-|+|*|/|^|>|<|!|(|)|{|}|=|;]           {return yytext[0];};
[a-zA-Z][_a-zA-Z0-9]*   {yylval.id=strdup(yytext); return VAR;}
[0-9][0-9]*             {yylval.num = atoi(yytext); return NUM;}
[ \t\n]                 ;
.                       {ECHO, yyerror("unexpected character\n");}

%%

int yywrap(void)
{
    return 1;
}