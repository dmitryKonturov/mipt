#include "static_java.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//void yyparse(void);

extern FILE* yyin;

int main(int argc, char **argv)
{
	FILE *handle;
	if (argc != 2) {
		cerr << "Usage error: filename to interpret expected" << endl;
		exit(1);
	}
	if (!(handle = fopen(argv[1], "r"))) {
		cerr << "Error: cannot open file\n";
		exit(1);
	}
	yyin = handle;
	Program = new GlobalInfo();
	cerr << "Before parse" << endl;

	yyparse();

	cerr << "After parse" << endl;

	Method *mainMethod = Program->main_method;
	if (mainMethod != NULL) {
		mainMethod->calculate(NULL);
	} else {
		cerr << "Error: Main not Found" << endl;
		exit(1);
	}
	
}


void yyerror(string s)
{
	extern int yychar;
	cerr << "Parser error: " << s << "\n";
	cerr << "token: " << yychar << "  char: " << (char) yychar << endl;
	exit(1);
}
