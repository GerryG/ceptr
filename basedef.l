%{
#include <stdio.h>
#include <stdlib.h>
#include "basedef.tab.h"

#define YYDEBUG 1

extern int yyerror();
%}

DIGIT [0-9]
WORDINITIAL [a-zA-Z_]
WORDREST [0-9a-zA-Z_]

%%
[ \t]*\n                   yylineno++ /* skip blank lines */;
#.*\n                      yylineno++ /* skip comment lines */;
Context                    printf("context\n"); return CONTEXT;
Data                       printf("data "); return DATA;
Declare                    printf("declare\n"); return DECLARE;
GlobalData                 printf("globaldata\n"); return GLOBALDATA;
Label                      printf("label\n"); return LABEL;
Process                    printf("process\n"); return PROCESS;
ProcessL                   printf("processl\n"); return PROCESSL;
Protocol                   printf("protocol\n"); return PROTOCOL;
Structure                  printf("structure\n"); return STRUCTURE;
Symbol                     printf("symbol\n"); return SYMBOL;
;[^\n]*\n                  yylineno++; printf(";to_eol\n"); return SEMI_EOL;
{WORDINITIAL}{WORDREST}*   printf("WD[%s] ", yytext); return WORD;
{DIGIT}+                   printf("NUM "); return NUMBER;
\"(\\.|[^\n"\\])*\"        printf( "STRING: %s\n", yytext ); return STRING;
\'(\\\'|[^\\\'])\'          printf( "CHAR: %s", yytext ); return CHAR;
\"                         printf("DQT ");   return D_QUOTE;
\'                         printf("SQT ");   return S_QUOTE;
\`                         printf("BQT ");   return B_QUOTE;
\(                         printf("OPAR ");  return OPEN_PAREN;
\)                         printf("CPAR ");  return CLOSE_PAREN;
\{                         printf("OBC ");   return OPEN_BRACE;
\}                         printf("CBC ");   return CLOSE_BRACE;
\[                         printf("OSQ ");   return OPEN_SQ_BRACKET;
\]                         printf("CSQ ");   return CLOSE_SQ_BRACKET;
\<                         printf("OAG ");   return OPEN_ANGLE_BRACKET;
\>                         printf("CAG ");   return CLOSE_ANGLE_BRACKET;
\;                         printf("SCLN ");  return SEMICOLON;
\,                         printf("CMA ");   return COMMA;
\:                         printf("CLN ");   return COLON;
\.                         printf("PRD ");   return PERIOD;
\?                         printf("Q ");     return QUESTION;
\*                         printf("STR ");   return STAR;
\+                         printf("PLS ");   return PLUS;
\-                         printf("MNS ");   return MINUS;
\=                         printf("EQL ");   return EQUAL;
\~                         printf("TLD ");   return TILDA;
\^                         printf("CRT ");   return CAROT;
\@                         printf("AT ");    return AT;
\|                         printf("BAR ");   return VERTICAL_BAR;
\&                         printf("AND ");   return AND;
\%                         printf("PCT ");   return PERCENT;
\!                         printf("BNG ");   return BANG;
\#                         printf("SHP ");   return SHARP;
\/                         printf("SL ");    return SLASH;
\\                         printf("BSL ");   return BACKSLASH;
[ \t]+                     printf("WS "); /* ignore whitespace */;
.	 { fprintf(stderr, "SCANNER "); yyerror(""); exit(1); }
%%
/* \n                         yylineno++; printf("NEWL ");return NEWLINE; */
