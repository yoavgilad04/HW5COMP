#ifndef HW3_TABLE_STACK_H
#define HW3_TABLE_STACK_H
#include <string>
#include <iostream>
#include "symbols/symbol_table.hpp"
#include "staff/hw3_output.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "source.hpp"

using namespace std;

extern int yylineno;

class Exp;

class TableStack
{
    std::vector<SymbolTable*> tables;
    FuncSymbol* last_added_func_sym;
public:
    TableStack():tables(), last_added_func_sym(nullptr){}
    void openNewScope();
    void closeScope();
    void addSymbolToLastTable(string name, string type, bool is_func_arg=false, string llvm_name="");
    void addSymbolToLastTable(string name, string type, string exp_type, string llvm_name="");
    void addFuncSymbol(string name, string type, string args, string is_override, string llvm_name="");
    vector<FuncSymbol*> getAllFunctionsWithName(string name);
    Symbol* searchForSymbol(string name);
    void compareType(string id_name, string exp_type);
    void updateSymbolLLVMName(string id_name, string llvm_name);
    void printStack();
    bool isFuncExist(string func_name);
    void checkReturnType(string type);
    FuncSymbol* searchForFuncSymbol(string func_name);
    FuncSymbol* getLastFuncSymbol() const {return last_added_func_sym;}
    ~TableStack();

    static std::vector<std::string> splitString(const std::string& inputString);
    static bool isValidConvert(string type_1, string type_2);
    static bool isVectorsEqual(const std::vector<string>& v1,const std::vector<string>& v2);
    static bool isVectorEqualWithConvert(const std::vector<string>& v1, std::vector<Exp>& v2);
};



#endif //HW3_TABLE_STACK_H
