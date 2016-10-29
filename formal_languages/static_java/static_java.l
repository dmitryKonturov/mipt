%{
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include "static_java.h"
%}

NUMERAL ([1-9][0-9]*)|0
IDENT ([a-zA-Z]|"_"|"$")([a-zA-Z0-9]|"_"|"$")*

%option noyywrap

%%
"return"	{ return RETURN;  }
"public"    { return PUBLIC;  } 
"class"     { return CLASS;   }
"static"    { return STATIC;  }
"void"      { return VOID;    }
"main"      { return MAIN;    }
"String"    { return STRING;  }
"boolean"   { return BOOLEAN; }
"int"       { return INT;     }
"if"		{ return IF;      }
"else"      { return ELSE;	  }
"while"     { return WHILE;   }
"true"      { return TRUE;    }
"false"     { return FALSE;   }
"println"   { return PRINTLN; }
"readln"    { return READ;    }
"&&"        { return AND_OP;  }
"||"        { return OR_OP;   }
"=="        { return EQUAL;   }
"!="        { return NOT_EQUAL; }
"<="        { return LESS_OR_EQUAL; } 
">="        { return GREATER_OR_EQUAL; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }
"<"         { return '<'; }
">"         { return '>'; }
"!"         { return '!'; }
"("         { return '('; }
")"         { return ')'; }
"{"         { return '{'; }
"}"         { return '}'; }
"["         { return '['; }
"]"         { return ']'; }
";"         { return ';'; }
","         { return ','; }
"."         { return '.'; }
"=" 		{ return '='; }

{NUMERAL}   { yylval.int_val = atoi(yytext); return NUMBER; }
{IDENT}     { strcpy(yylval.ident_name, yytext); return IDENT; }

[[:space:]]     { /* space - do nothing */ }
.       { printf("Lexical error: symbol '%c'!\n", yytext[0]); exit(1); }

%%
