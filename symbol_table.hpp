#ifndef HW3_SYMBOL_TABLE_HPP
#define HW3_SYMBOL_TABLE_HPP
#include "symbol.hpp"
#include <bits/stdc++.h>
#include <vector>

using namespace std;
class SymbolTable
{
    multimap<string, Symbol*> map;
    vector<Symbol*> vec;
    int next_offset;
    int func_arg_offset;
    string table_return_type;
    bool is_func_scope;
public:
    SymbolTable(int next_offset=0, string table_return_type="VOID", bool is_func_scope = false):map(), vec(), next_offset(next_offset), func_arg_offset(-1){}
    void insert(string name, string type, bool is_func_arg=false);
    void insertFunc(string name, string type, vector<string> input_args, bool is_override);
    void popBySymbol(Symbol* s);
    void popByOffset(int offset);
    void setTableReturnType(string type){this->table_return_type=type;}
    void setScopeAsFunc(){this->is_func_scope=true;}
    Symbol* getSymbolByName(string name);
    Symbol* getSymbolByOffset(int offset);
    bool isEmpty();
    vector<Symbol*> getLastNSymbols(int n);
    bool isFunc(){return this->is_func_scope;}
    string getTableReturnType(){return this->table_return_type;}
    vector<FuncSymbol*> getAllFunctionWithName(string name);
    int getSymbolTableOffset(){return this->next_offset;}
    ~SymbolTable();


    friend ostream& operator<<(ostream& os, const SymbolTable& t)
    {
        for (Symbol* symbol : t.vec)
        {
            os << *(symbol) << endl;
        }
        return os;
    }
};

#endif //HW3_SYMBOL_TABLE_HPP
