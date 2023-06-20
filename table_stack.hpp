#ifndef HW3_TABLE_STACK_H
#define HW3_TABLE_STACK_H
#include <string>
#include <iostream>
#include "symbols/symbol_table.hpp"
#include "staff/hw3_output.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

extern int yylineno;

class TableStack
{
    std::vector<SymbolTable*> tables;
public:
    TableStack():tables(){}
    void openNewScope();
    void closeScope();
    void addSymbolToLastTable(string name, string type, bool is_func_arg=false);
    void addSymbolToLastTable(string name, string type, string exp_type);
    void addFuncSymbol(string name, string type, string args, string is_override);
    vector<FuncSymbol*> getAllFunctionsWithName(string name);
    Symbol* searchForSymbol(string name);
    void compareType(string id_name, string exp_type);
    void printStack();
    bool isFuncExist(string func_name);
    void checkReturnType(string type);
    FuncSymbol* searchForFuncSymbol(string func_name);
    ~TableStack();

    static std::vector<std::string> splitString(const std::string& inputString);
    static bool isValidConvert(string type_1, string type_2);
    static bool isVectorsEqual(const std::vector<string>& v1,const std::vector<string>& v2);
    static bool isVectorEqualWithConvert(const std::vector<string>& v1,const std::vector<string>& v2);
};



#endif //HW3_TABLE_STACK_H
