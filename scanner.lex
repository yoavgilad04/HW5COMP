%{

/* Declarations section */
#include <stdio.h>
#include "source.hpp"
#include "parser.tab.hpp"
%}

%option yylineno
%option noyywrap
digit   		([0-9])
letter  		([a-zA-Z])
escape          ([\\])
quote           ([\x22])
newline         ([\x0a])
whitespace		([\r\n\t ])
instring       ([\x00-\x09\x0b-\x0c\x0e-\x21\x23-\x5b\x5d-\x7f])
incomment      ([^\x0a\x0d])

%x STR

%%
{quote}                                 BEGIN(STR);
<STR>{quote}                            BEGIN(INITIAL); return STRING;
<STR>({instring}|\\[nrt"\\])+           yylval=new Node(yytext);
<STR>.                                  output::errorLex(yylineno); exit(0);
{whitespace}				            ;
void                                    yylval=new Node("VOID"); return VOID;
int                                     yylval=new Node(yytext); return INT;
byte                                    yylval=new Node(yytext); return BYTE;
bool                                    yylval=new Node(yytext); return BOOL;
b                                       yylval=new Node(yytext); return B;
and                                     yylval=new Node(yytext); return AND;
or                                      yylval=new Node(yytext); return OR;
not                                     yylval=new Node(yytext); return NOT;
true                                    yylval=new Node(yytext); return TRUE;
false                                   yylval=new Node(yytext); return FALSE;
return                                  yylval=new Node(yytext); return RETURN;
else                                    yylval=new Node(yytext); return ELSE;
while                                   yylval=new Node(yytext); return WHILE;
if                                      yylval=new Node(yytext); return IF;
break                                   yylval=new Node(yytext); return BREAK;
continue                                yylval=new Node(yytext); return CONTINUE;
;                                       yylval=new Node(yytext); return SC;
,                                       yylval=new Node(yytext); return COMMA;
\(                                      yylval=new Node(yytext); return LPAREN;
\)                                      yylval=new Node(yytext); return RPAREN;
\{                                      yylval=new Node(yytext); return LBRACE;
\}                                      yylval=new Node(yytext); return RBRACE;
\=                                      yylval=new Node(yytext); return ASSIGN;
\<|\>|\<\=|\>\=                         yylval=new Node(yytext); return RELOP;
\=\=|\!\=                               yylval=new Node(yytext); return EQLOP;
\+|\-                                   yylval=new Node(yytext); return ADD;
\*|\/                                   yylval=new Node(yytext); return MUL;
override                                yylval=new Node(yytext); return OVERRIDE;
0|[1-9]{digit}*                         yylval=new Node(yytext); return NUM;
{letter}({letter}|{digit})*             yylval=new Node(yytext); return ID;
\/\/{incomment}*[\r|\n|\r\n]?           ;
.                                       output::errorLex(yylineno); exit(0);

%%