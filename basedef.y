%{
/* from basedef.y */
#include <stdio.h>
#include <stdlib.h>
 
#define YYDEBUG 1

int yyerror(char *s);
int yylex(void);

extern int yyparse(void);
extern int yylineno;
extern char *yytext;

int yywrap() { return 1; } 
int yyerror(char *s) {
  fprintf(stderr, "ERROR: %s at symbol \"%s\" on line %d\n", s, yytext, yylineno);
  exit(1);
}
int main() { yyparse(); } 

%}

%start directives

%union {
  int int_val;
  char *str_val;
}

%token CONTEXT DATA DECLARE GLOBALDATA LABEL PROCESS PROCESSL PROTOCOL STRUCTURE
%token SYMBOL WORD ARGS COMMA COLON SEMI_EOL NUMBER D_QUOTE S_QUOTE B_QUOTE
%token OPEN_PAREN CLOSE_PAREN OPEN_BRACE CLOSE_BRACE OPEN_SQ_BRACKET CLOSE_SQ_BRACKET
%token OPEN_ANGLE_BRACKET CLOSE_ANGLE_BRACKET SEMICOLON PERIOD QUESTION STAR PLUS
%token MINUS EQUAL TILDA CAROT AT VERTICAL_BAR AND PERCENT BANG SHARP SLASH BACKSLASH
%token NEWLINE STRING CHAR

%%

directives: directives directive| directive;

directive: CONTEXT COLON WORD SEMI_EOL
         | DATA COLON WORD COMMA balanced SEMI_EOL
         | DECLARE COLON wordlist SEMI_EOL
         | GLOBALDATA COLON WORD COMMA balanced SEMI_EOL
         | LABEL COLON WORD COMMA WORD COMMA STRING SEMI_EOL
         | PROCESS COLON process_args SEMI_EOL
         | PROCESSL COLON process_args SEMI_EOL
         | PROTOCOL COLON wordlist SEMI_EOL
         | STRUCTURE COLON WORD COMMA balanced SEMI_EOL
         | SYMBOL COLON WORD COMMA WORD SEMI_EOL
         | SYMBOL COLON WORD COMMA bracketted SEMI_EOL
{
printf("%d: cmd: %s\n", yylineno, (char *)yytext);
  printf("%d[%d] Args:%d\n", yylineno, $<int_val>1, $<int_val>3);
} ;

wordlist: WORD
        | WORD COMMA wordlist
        ;

bracketted: OPEN_SQ_BRACKET balanced CLOSE_SQ_BRACKET ;

balanced: balanced_item
        | balanced_item balanced

balanced_item: arg_bal
        | OPEN_BRACE balanced CLOSE_BRACE
        | OPEN_PAREN balanced CLOSE_PAREN
        ;

arg_bal: NEWLINE | NUMBER | WORD | CHAR | STRING | COMMA | PERIOD | QUESTION | COLON
       | D_QUOTE | STAR | PLUS | MINUS | EQUAL | TILDA | CAROT | AT | VERTICAL_BAR
       | AND | PERCENT | BANG | SHARP | SLASH
       ;

arg_token: WORD | NUMBER | CHAR | STRING ;
process_args: arg_token
            | arg_token COMMA process_args ;

%%

