#include "static_java.h"
#include <cstdio>
#include <iostream>

void ListStmt::run() {
    vector<Stmt*>::iterator i;
    for (i = list.begin(); i != list.end(); ++i) {
        if (!Program->isReturned) {
            (*i)->run();
        }
    }
}


/*--------------------------EXPRESSIONS-------------------------------------------*/

int ExprArith::eval() {
    int first, second;
    first = e1->eval();
    if (e2) 
        second = e2->eval();
    else 
        second = 0;

    switch (op) {
        case UMINUS : return -first;
        case UPLUS  : return first;
        case UNOT   : return !first;
        case '>'    : return first > second;
        case '<'    : return first < second;
        case GREATER_OR_EQUAL : return first >= second;
        case EQUAL            : return first == second;
        case LESS_OR_EQUAL    : return first <= second;
        case NOT_EQUAL        : return first != second;
        case '+'              : return first + second;
        case '-'              : return first - second;
        case '*'              : return first * second;
        case '/'              : 
                if (second == 0) {
                    cerr << "Runtime error: division by zero" << endl;
                    exit(1);
                }         
                return first / second;

        case '%'               :
                if (second == 0) {
                    cerr << "Runtime error: division by zero" << endl;
                    exit(1);
                } 
                return first % second;

        case AND_OP : return first & second;
        case OR_OP  : return first | second; 
    }
    cerr << "Error: operation not found" << endl;
    exit(1);
    return 0;
}

int ExprVariable::eval() {
    return Program->getValue(name);
}

int ExprInvoke::eval() {
    vector<int> cur;
    for (int i = 0; i < (args->list).size(); ++i) {
        cur.push_back(((args->list)[i])->eval());
    }
    Method *toInvoke = Program->getMethod(method_name);
    if (toInvoke == NULL) {
        cerr << "Error: method was not found: " << method_name << endl;
        exit(1);
    }
    return toInvoke->calculate(&cur);
}

/*--------------------------STATEMENTS-------------------------------------------*/

void StmtAssign::run() {
    int value = e->eval();
    Program->changeValue(name, value);
}

void StmtIf::run() {
    int flag = e->eval();
    if (flag) {
        if (L1) L1->run();
    } else {
        if (L2) L2->run();
    }
}

void StmtWhile::run() {
    int flag = e->eval();
    while (flag) {
        L->run();
        flag = e->eval();
    }
}

void StmtInvoke::run() {
    e->eval();
}

void StmtReturn::run() {
    int value = e->eval();
    Program->isReturned = true;
    Program->returnedValue = value;
}

void StmtPrint::run() {
    int value = e->eval();
    cout <<  value << endl;
}

void StmtRead::run() {
    int toSet;
    cout << "Enter value for " << name << ": " << endl;
    cin >> toSet;
    Program->changeValue(name, toSet);
}

/*---------------------------Method-------------------------------------------*/
int MethodImpl::calculate(vector<int> *arguments) {
    if (arguments == NULL && arg_name != NULL || arguments != NULL && arg_name == NULL) {
        cerr << "Error: Null method" << endl;
        exit(1);
    }
    //initialize
    if (arguments != NULL) {
        if ((*arguments).size() !=  arg_name->list.size()) {
            cerr << "Error: cannot calculate method: arguments and parametrs dont match: " << name << endl;
            exit(1);
        }
        Program->info.startLocalInfo();
        for (size_t i = 0; i < (*arguments).size(); ++i) {
            Program->info.changeValue(arg_name->list[i], (*arguments)[i]);
        }
    }
    if (decl_list != NULL) {    
        for (size_t i = 0; i < decl_list->list.size(); ++i) {
            if (Program->info.contains(decl_list->list[i])) {
                cerr << "Warning: local variable shadows parametr: " << decl_list->list[i] << endl;
            }
            Program->info.changeValue(decl_list->list[i], 0);
        }
    }

    //body
    int toReturn = 0;
    if (stmt_list) stmt_list->run();
    if (Program->isReturned) {
        if (isVoid) {
            cerr << "Error: void method return value" << endl;
            exit(1);
        }
        Program->isReturned = false;
        toReturn = Program->returnedValue;
    } else {
        if (!isVoid) {
            cerr << "Error: method " << name << " finished without return" << endl;
            exit(1);
        } 
    }

    Program->info.endLocalInfo();

    return toReturn;
}
/*----------------------------------------------------------------------------*/

GlobalInfo *Program;

