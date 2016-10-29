%{
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "static_java.h"
%}

%union {
    Expr *expr;
    Stmt *stmt;
    ListStmt *list_stmt;
    ListExpr *list_expr;
    Method *method;
    ListName *list_name;
    bool bool_val;
    MethodBody *methodbody;

    int int_val;
    char ident_name[256];   
};

%left OR_OP
%left AND_OP 
%nonassoc '<' LESS_OR_EQUAL EQUAL NOT_EQUAL '>' GREATER_OR_EQUAL
%left '+' '-' 
%left '*' '/' '%' 
%right UMINUS UPLUS UNOT

%token <int_val> NUMBER
%token <ident_name> IDENT

%token PUBLIC CLASS STATIC VOID MAIN STRING BOOLEAN INT IF ELSE WHILE TRUE FALSE PRINTLN READ RETURN

%type <expr>         expression
%type <stmt>         statement              if_statement
%type <list_stmt>    statement_list         probably_else
%type <expr>         invoke_exp
%type <list_expr>    argument_list          probably_argument_list
%type <ident_name>   local_declaration      param                             prefix  
%type <list_name>    params                 local_declaration_list            non_empty_params
%type <methodbody>   method_body


%%
    program 
        : class_declaration
        ;

    class_declaration
        : PUBLIC CLASS IDENT '{' main_method_declaration declaration_list '}' 
        ;
    
    main_method_declaration
        : PUBLIC STATIC VOID MAIN '(' STRING '[' ']' IDENT ')' '{' method_body '}'  {
                            string tmp("main");
                            Method *curMethod = new MethodImpl(tmp, NULL, $12, true);
                            Program->main_method = curMethod;
                        }
        ;

    declaration_list
        : /*empty*/
        | declaration_list field_or_method_declaration
        ;

    field_or_method_declaration
        : field_declaration
        | method_declaration
        ;

    field_declaration
        : static_type IDENT ';'             { Program->variables.insert(std::pair<string, int>($2, 0)); }
        ;

    method_declaration
        : static_type IDENT '(' params ')' '{' method_body '}'       {   
                
                        Method *curMethod = new MethodImpl($2, $4, $7, false); 
                        Program->methods.insert(std::pair<string, Method *>($2, curMethod));
                    }

        | static_void IDENT '(' params ')' '{' method_body '}'       {   
                
                        Method *curMethod = new MethodImpl($2, $4, $7, true); 
                        Program->methods.insert(std::pair<string, Method *>($2, curMethod));
                    }    
        ;

    type  
        : BOOLEAN 
        | INT
        ;

    static_type
        : STATIC type
        ;

    static_void
        : STATIC VOID
        ;

    params
        : /*empty*/                                     { $$ = NULL; }
        | non_empty_params                              { $$ = $1;   }
        ;

    non_empty_params
        : non_empty_params ',' param                    { $1->add($3); $$ = $1;             }
        | param                                         { $$ = new ListName(); $$->add($1); }
        ;

    param
        : type IDENT                                    { strcpy($$, $2); }
        ;

    method_body
        : local_declaration_list statement_list         { $$ = new MethodBody($1, $2); }
        ;

    local_declaration_list
        : /*empty*/                     { $$ = new ListName(); }
        | local_declaration_list local_declaration      { $1->add($2); $$ = $1; }
        ;

    local_declaration
        : type IDENT ';'                { strcpy($$, $2); }
        ;

    statement_list
        : /*empty*/                     { $$ = new ListStmt();  }
        | statement_list statement      { $1->add($2); $$ = $1; }
        ;

    statement
        : IDENT '=' expression ';'              { $$ = new StmtAssign($1, $3); }
        | if_statement                          { $$ = $1; }
        | invoke_exp ';'                        { $$ = new StmtInvoke($1); }

        | RETURN expression ';'                 { $$ = new StmtReturn($2); }
        | PRINTLN '(' expression ')' ';'        { $$ = new StmtPrint($3);  }
        | READ '(' IDENT ')' ';'                { $$ = new StmtRead($3);   }
        
        | WHILE '(' expression ')' '{' statement_list '}'           { $$ = new StmtWhile($3, $6); } 
        ;

    if_statement
        : IF '(' expression ')' '{' statement_list '}' probably_else    { $$ = new StmtIf($3, $6, $8); }
        ;

    probably_else
        : /*empty*/                         { $$ = NULL; }
        | ELSE '{' statement_list '}'       { $$ = $3;   }      
        ;


    expression
        : TRUE          { $$ = new ExprLiteral(1);   }                             
        | FALSE         { $$ = new ExprLiteral(0);   }
        | NUMBER        { $$ = new ExprLiteral($1);  }
        | IDENT         { $$ = new ExprVariable($1); }

        | '+' expression  %prec UPLUS      { $$ = new ExprArith(UPLUS,  $2, NULL); }
        | '-' expression  %prec UMINUS     { $$ = new ExprArith(UMINUS, $2, NULL); }
        | '!' expression  %prec UNOT       { $$ = new ExprArith(UNOT,   $2, NULL); }
        
        | expression '>' expression                     { $$ = new ExprArith('>'             , $1, $3); }
        | expression GREATER_OR_EQUAL expression        { $$ = new ExprArith(GREATER_OR_EQUAL, $1, $3); }
        | expression EQUAL expression                   { $$ = new ExprArith(EQUAL           , $1, $3); }
        | expression '<' expression                     { $$ = new ExprArith('<'             , $1, $3); }
        | expression LESS_OR_EQUAL expression           { $$ = new ExprArith(LESS_OR_EQUAL   , $1, $3); }
        | expression NOT_EQUAL expression               { $$ = new ExprArith(NOT_EQUAL       , $1, $3); }

        | expression '+' expression         { $$ = new ExprArith('+', $1, $3); }
        | expression '-' expression         { $$ = new ExprArith('-', $1, $3); }
        | expression '*' expression         { $$ = new ExprArith('*', $1, $3); }
        | expression '/' expression         { $$ = new ExprArith('/', $1, $3); }
        | expression '%' expression         { $$ = new ExprArith('%', $1, $3); }

        | expression AND_OP expression      { $$ = new ExprArith(AND_OP, $1, $3); }
        | expression OR_OP expression       { $$ = new ExprArith(OR_OP , $1, $3); }
        
        | '(' expression ')'                { $$ = $2; }
        
        | invoke_exp                        { $$ = $1; }         
        ;

    invoke_exp
        : prefix probably_argument_list ')'       { $$ = new ExprInvoke($1, $2); }
        ;

    prefix
        : IDENT '('                         { strcpy($$, $1); }
        | IDENT '.' IDENT '('               { strcpy($$, $3); }

    probably_argument_list
        : /*empty*/             { $$ = NULL; }
        | argument_list         { $$ = $1;   }
        ;

    argument_list
        : expression                        { $$ = new ListExpr; $$->add($1); }
        | argument_list ',' expression      { $1->add($3)      ; $$ = $1;     }
        ;

%%