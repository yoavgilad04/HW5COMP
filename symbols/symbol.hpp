//
// Created by yoavgilad on 5/26/23.
//

#ifndef HW3_SYMBOL_HPP
#define HW3_SYMBOL_HPP

#include <string>
#include <iostream>
#include <vector>
#include "../staff/hw3_output.hpp"



using namespace std;

class Symbol
{
protected:
    string name;
    string type;
    string llvm_name;
    int offset;
    bool is_func;

public:
    Symbol(string name, string type, int offset=0, bool is_func=false, string llvm_name="Def"):
    name(name), type(type), llvm_name(llvm_name), offset(offset), is_func(is_func){}
    int getOffset() const {return this->offset;}
    string getLLVMName() const {return this->llvm_name;}
    void setLLVMName(string llvm_name){this->llvm_name = llvm_name;}
    string getName() const {return this->name;}
    string getType() const {return this->type;}
    void setOffset(int offset){this->offset = offset;}
    virtual ostream& printSymbol(std::ostream& os) const
    {
        os << this->getName() << " " << this->getType() <<  " " << this->getOffset() << " " << this->getLLVMName();
        return os;
    }
    friend ostream& operator<<(ostream& os, const Symbol& s)
    {
        s.printSymbol(os);
        return os;
    }
};

class FuncSymbol : public Symbol
{
    vector<string> input_args;
    bool is_override;
public:
    FuncSymbol(string name, string type, int offset, bool is_func, vector<string> input_args, bool is_override, string llvm_name):
            Symbol(name, type, offset, is_func, llvm_name), input_args(input_args), is_override(is_override){};
    bool isOverride(){return this->is_override;}
    vector<string> getArgs() const {return this->input_args;}
    ostream& printSymbol(std::ostream& os) const override
    {
        os << this->getName() << " ";
        os << "(";
        for(int i = 0; i < this->input_args.size(); ++i)
        {
            os << this->input_args[i];
            if (i + 1 < this->input_args.size())
                os << ",";
        }
        os << ")";
        os << "->" << this->getType() << " " << this->getOffset() << " " << this->getLLVMName();
        return os;
    }
    friend ostream& operator<<(ostream& os, const FuncSymbol& s)
    {
        s.printSymbol(os);
        return os;
    }
};

#endif //HW3_SYMBOL_HPP
