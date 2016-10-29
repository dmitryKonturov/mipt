#ifndef _STATICJAVA_H
#define _STATICJAVA_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>
#include <cstdlib>

using namespace std;

class Expr {
public:
    virtual int eval() = 0;
};

class Stmt {
public:
    virtual void run() = 0;
};

class ListStmt {
public:
    ListStmt() { }
    void add(Stmt *s) { list.push_back(s); }
    void run();
private:
    vector<Stmt*> list;
};

class ListExpr {
public:
    ListExpr() { }
    void add(Expr *e) {list.push_back(e); }
    
    vector<Expr*> list;
};

class ListName {
public:
    ListName() { }
    void add(string s) { list.push_back(s); }

    vector<string> list;
};

class ListParam {
public:
    ListParam() { }
    void add(int i) { list.push_back(i); }
    vector<int> list;
};

/*--------------------------EXPRESSIONS-------------------------------------------*/

class ExprArith : public Expr { /* evaluate either bool or int expression */
public:
    ExprArith(int op_, Expr *e1_, Expr *e2_) : op(op_), e1(e1_), e2(e2_) { }
    int eval();
private:
    int op;
    Expr *e1, *e2;
};

class ExprLiteral : public Expr {
public:
    ExprLiteral(int val_) : val(val_) { }
    int eval() { return val; }
private:
    int val;
};

class ExprVariable : public Expr {
public:
   ExprVariable(const char *name_) : name(name_) { }
   int eval();
private:
   string name;
};

class ExprInvoke : public Expr {
public:
    ExprInvoke(const char *name_, ListExpr *args_) : method_name(name_), args(args_) { }
    int eval();
private:
    string method_name;
    ListExpr *args;
};

/*--------------------------STATEMENTS-------------------------------------------*/

class StmtAssign : public Stmt {
public:
    StmtAssign(const char *name_, Expr *e_) : name(name_), e(e_) { }
    void run();
private:
    string name;
    Expr *e;
};

class StmtIf : public Stmt {
public:
    StmtIf(Expr *e_, ListStmt *L1_, ListStmt *L2_) : e(e_), L1(L1_), L2(L2_) { }
    void run();
private:
    ListStmt *L1, *L2;
    Expr *e;
};

class StmtWhile : public Stmt {
public:
    StmtWhile(Expr *e_, ListStmt *L_) : e(e_), L(L_) { }
    void run();
private:
    Expr *e;
    ListStmt *L;
};

class StmtInvoke : public Stmt {
public:
    StmtInvoke(Expr *e_) : e(e_) { }
    void run();
private:
    Expr *e;
};

class StmtReturn : public Stmt {
public:
    StmtReturn(Expr *e_) : e(e_) { }
    void run();
private:
    Expr *e;
};

class StmtPrint : public Stmt {
public:
    StmtPrint(Expr *e_) : e(e_) { }
    void run();
private:
    Expr *e; 
};

class StmtRead: public Stmt {
public:
    StmtRead(const char *name_) : name(name_) { }
    void run();
private:
    string name;
};
/*---------------------------Method-------------------------------------------*/

class MethodBody {
public:
    MethodBody(ListName *decl_list_, ListStmt *stmt_list_) : decl_list(decl_list_), stmt_list(stmt_list_) { }
    ListName *decl_list;
    ListStmt *stmt_list;
};

class Method {
public:
    virtual int calculate(vector<int> *param) = 0; 
};

class MethodImpl: public Method {
public:
    MethodImpl(string name_, ListName *arg_name_, MethodBody *body, bool isVoid_) 
        :name(name_), arg_name(arg_name_), decl_list(body->decl_list), stmt_list(body->stmt_list), isVoid(isVoid_) { 

    }

    int calculate(vector<int> *param);
private:
    bool isVoid;
    string name;
    ListName *arg_name;
    ListStmt *stmt_list;
    ListName *decl_list;
};

/*-------------------------------Class----------------------------------------*/

class LocalInfo {
public:
    LocalInfo() {
        map<string, int> *base = new map<string, int>();
        localVar.push(base);
    }
   
    void startLocalInfo() { 
        map<string, int> *base = new map<string, int>();
        localVar.push(base);    
    }
   
    bool contains(string name) {
        map<string, int> *local = localVar.top();
        map<string, int>::iterator it = local->find(name);
        return (it != local->end());
    }

    int getValue(string name) { 
        map<string, int> *local = localVar.top();
        map<string, int>::iterator it = local->find(name);
        if (it == local->end()) {
            cerr << "Error: local variable " << name << " is undefined" << endl;
            exit(1);
        }
        return it->second;
    } 
   
    void changeValue(string name, int value) { 
        map<string, int> *local = localVar.top();
        (*local)[name] = value;
    }
   
    void endLocalInfo() { 
        localVar.top()->clear();
        delete localVar.top();
        localVar.pop();
    }
private:
    stack< map<string, int> *> localVar;
};

class GlobalInfo {
public:
    GlobalInfo() {
        isReturned = false;
        returnedValue = 0; 
    }

    int getValue(string name) {
        if (info.contains(name)) {
            return info.getValue(name);
        } else {
            map<string, int>::iterator it = variables.find(name);
            if (it == variables.end()) {
                cerr << "Error: undefined variable: " << name << endl;
                exit(1);
            }
            return it->second;
        }
    }

    void changeValue(string name, int value) {
        if (info.contains(name)) {
            info.changeValue(name, value);
        } else if (variables.find(name) != variables.end()) {
            variables[name] = value;
        } else {
            cerr << "Error: variable \'" <<  name << "\' was not declared" << endl;
            exit(1);
        }
    }

    Method* getMethod(string name) {
        map<string, Method *>::iterator it = methods.find(name);
        if (it == methods.end()) {
            return NULL;
        } else {
            return it->second;
        }
    }

    bool isReturned;
    int returnedValue;
    LocalInfo info;
    map<string, Method *> methods;
    map<string, int> variables; 
    Method *main_method;
};

/*-----------------------------------------------------------------------------*/
/*
class MyLogger {
public:
    MyLogger(FILE *file) : logFile(file) { }
    
    void print(string message) {
        fprintf(logFile, "%s\n", message.c_str());
    }

    void printValue(string message, int value) {
        fprintf(logFile, "%s:  %d\n", message.c_str(), value);
    }
private:
    FILE *logFile;
};
*/
/*----------------------------------------------------------------------------*/

//extern ListStmt *Program;

#include "static_java.tab.h"

/*
struct id_tab {
    string name;
    int value;
};

extern struct id_tab Table[];

struct id_tab *get_id(string name);
void add_id(string name, int value);
int pow_int(int x, int n);
extern MyLogger *logger;
*/

extern GlobalInfo *Program;

int yylex();
void yyerror(string s);

#endif /* _STATICJAVA_H */
