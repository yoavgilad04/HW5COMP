#include "table_stack.hpp"

std::vector<std::string> TableStack::splitString(const std::string& inputString) {
    std::vector<std::string> words;
    std::stringstream ss(inputString);
    std::string word;

    while (std::getline(ss, word, ',')) {
        words.push_back(word);
    }
    return words;
}

bool TableStack::isValidConvert(string left_type, string right_type)
{
    if (left_type != right_type)
    {
        if(left_type == "INT" && right_type == "BYTE")
            return true;
        return false;
    }
    return true;
}

bool TableStack::isVectorsEqual(const std::vector<string>& v1,const std::vector<string>& v2)
{
    if (v1.size() != v2.size())
        return false;
    for(int i=0; i<v1.size(); i++)
    {
        if (v1[i] != v2[i])
            return false;
    }
    return true;
}

bool TableStack::isVectorEqualWithConvert(const std::vector<string>& left_v, std::vector<Exp>& right_v)
{
    if (left_v.size() != right_v.size())
        return false;
    for(int i=0; i<left_v.size(); i++)
    {
        if (!isValidConvert(left_v[i],right_v[i].getType()))
            return false;
    }
    return true;
}

void TableStack::addFuncSymbol(string name, string type, string args, string is_override, string llvm_name)
{
    if (this->tables.empty())
        throw std::invalid_argument("Trying to add element when there are no scopes in stack");
    if (name == "main")
    {
        Symbol* potential_main = searchForSymbol(name);
        if (potential_main != nullptr)
            output::errorDef(yylineno,name);
        if(is_override == "OVERRIDE")
            output::errorMainOverride(yylineno);
    }
    vector<string> input_args = TableStack::splitString(args);
    if(input_args.size() > 0 && this->tables.size() > 1)
    {
        SymbolTable* last_table = this->tables[this->tables.size()-1];
        vector<Symbol*> last_args = last_table->getLastNSymbols(input_args.size());
        for (Symbol* symbol :last_args)
        {
            if(name == symbol->getName())
                output::errorDef(yylineno, name);
        }
    }
    bool is_over = true;
    if (is_override != "OVERRIDE")
    {
        is_over = false;
    }
    vector<FuncSymbol*> funcs = this->getAllFunctionsWithName(name);
    if (!funcs.empty())
    {
        if ((is_over) && (!funcs[0]->isOverride()))
            output::errorFuncNoOverride(yylineno, name);
        if ((!is_over) && (funcs[0]->isOverride()))
            output::errorOverrideWithoutDeclaration(yylineno, name);
        if ((!is_over) && (!funcs[0]->isOverride()))
            output::errorDef(yylineno, name);
        for (int i=0; i<funcs.size(); i++)
        {
            vector<string> func_args = funcs[i]->getArgs();
            if (isVectorsEqual(input_args, func_args))
                output::errorDef(yylineno, name);
        }
    }
    FuncSymbol* s;
    if (this->tables.size() > 1)
    {
        s = this->tables[this->tables.size() - 2]->insertFunc(name, type, input_args,
                                                          is_over, llvm_name); // we want the func symbol to enter previous scope
        this->tables[this->tables.size() - 2]->setTableReturnType(type);
        this->tables[this->tables.size() - 2]->setScopeAsFunc();
    }
    else
    {
        s = this->tables[this->tables.size() - 1]->insertFunc(name, type, input_args, is_over, llvm_name);
        this->tables[this->tables.size() - 1]->setTableReturnType(type);
        this->tables[this->tables.size() - 1]->setScopeAsFunc();
    }
    this->last_added_func_sym = s;
}

bool TableStack::isFuncExist(string func_name)
{
    FuncSymbol* func = this->searchForFuncSymbol(func_name);
     if (func == nullptr)
         return false;
     if (!func->isOverride())
         output::errorFuncNoOverride(yylineno, func_name);
     return true;
}

FuncSymbol* TableStack::searchForFuncSymbol(string func_name)
{
    FuncSymbol* func = dynamic_cast<FuncSymbol*>(this->searchForSymbol(func_name));
    return func; //if the cast failed func = nullptr
}

void TableStack::addSymbolToLastTable(string name, string type, bool is_func_arg, string llvm_name)
{
    if (this->tables.empty())
        throw std::invalid_argument("Trying to add element when there are no scopes in stack");
    Symbol* s = this->searchForSymbol(name);
    if (s != nullptr)
        output::errorDef(yylineno, name);
    this->tables[this->tables.size()-1]->insert(name, type, is_func_arg, llvm_name);
}

void TableStack::addSymbolToLastTable(string name, string type, string exp_type, string llvm_name)
{
    if (this->tables.empty())
        throw std::invalid_argument("Trying to add element when there are no scopes in stack");
    Symbol* s = this->searchForSymbol(name);
    if (s != nullptr)
        output::errorDef(yylineno, name);
    if (!isValidConvert(type, exp_type))
        output::errorMismatch(yylineno);
    this->tables[this->tables.size()-1]->insert(name, type, false, llvm_name);
}

vector<FuncSymbol*> TableStack::getAllFunctionsWithName(string name)
{
    vector<FuncSymbol*> funcs;
    for(int i=0; i<this->tables.size(); i++)
    {
        vector<FuncSymbol*> symbol_table_funcs = this->tables[i]->getAllFunctionWithName(name);
        if (!symbol_table_funcs.empty())
            funcs.insert(funcs.end(), symbol_table_funcs.begin(), symbol_table_funcs.end());
    }
    return funcs;
}

void TableStack::openNewScope()
{
    int new_offset;
    if (this->tables.empty() || this->tables.size() == 1)
    {
        new_offset = 0;
    }
    else
        new_offset = this->tables[this->tables.size() - 1]->getSymbolTableOffset();
    SymbolTable* t = new SymbolTable(new_offset);
    this->tables.push_back(t);
//    cout << "scope opened!" << endl;
}

void TableStack::closeScope()
{
    if (this->tables.empty())
        throw std::invalid_argument("Trying to close scope when there are no scopes in stack");
    SymbolTable* t_to_delete = this->tables[this->tables.size()-1];

    //this->printStack();
    if(this->tables.size() == 1)
    {
//        cout <<"I'm in last scope" << endl;
        FuncSymbol* main = dynamic_cast<FuncSymbol*>(this->searchForSymbol("main"));
        if (main == nullptr || main->getType() != "VOID" || (!main->getArgs().empty()))
            output::errorMainMissing();
    }

    output::endScope();
    this->tables.pop_back();
    delete t_to_delete;
}


Symbol* TableStack::searchForSymbol(string name)
{
    for(int i=this->tables.size()-1; i>=0 ; i--)
    {
        Symbol* sym = this->tables[i]->getSymbolByName(name);
        if (sym == nullptr)
            continue;
        return sym;
    }
    return nullptr;
}

void TableStack::printStack()
{
    for (int i = this->tables.size()-1; i>=0; i--)
    {
        cout << "Table "<< i << ": " << endl << *(this->tables[i]) << endl;
    }
}

TableStack::~TableStack()
{
//    if (this->tables.size() != 0)
//        cout <<"ERROR: More then one Scope when deleteing tableStack";
    for(int i=0; i<this->tables.size(); i++)
    {
        delete this->tables[i];
    }
}

void TableStack::compareType(string id_name, string exp_type)
{
    Symbol* s = this->searchForSymbol(id_name);
    if (s == nullptr)
        output::errorUndef(yylineno, id_name);
//    cout << "left type:" << s->getType() << "right type" << exp_type << endl;
    if (!isValidConvert(s->getType(), exp_type))
        output::errorMismatch(yylineno);
}

void TableStack::updateSymbolLLVMName(string id_name, string llvm_name)
{
    Symbol* s = this->searchForSymbol(id_name);
    s->setLLVMName(llvm_name);
}

void TableStack::checkReturnType(std::string type) {
    int current_scope = this->tables.size()-1;
    while(!this->tables[current_scope]->isFunc())
    {
        current_scope--;
    }
    if(!isValidConvert(this->tables[current_scope]->getTableReturnType(), type))
        output::errorMismatch(yylineno);
}