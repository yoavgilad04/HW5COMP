#ifndef _SOURCE_H
#define _SOURCE_H

#include <string>
#include <iostream>
#include "table_stack.hpp"
#include "staff/hw3_output.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>

extern int yylineno;
using namespace std;


class Node {
protected:
    string type;
public:
    Node(string type=""): type(type){};
    string getType(){return this->type;}
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Exp : public Node
{
    string llvm_var;

public:
    Exp(string type, string value);
    Exp(string type);
    Exp(Node& exp_1, string operation_val, Node& exp_2);
    Exp(Node& n);
    Exp(Node& exp, const string& conversion_type);
    string getLLVMVar(){return this->llvm_var;}
    friend ostream& operator<<(ostream& os, const Exp& e)
    {
        os << "Type: " << e.type;
        return os;
    }
};

class FormalList : public Node
{
public:
    FormalList():Node(){}
    FormalList(Node& formal): Node(formal) {}
    FormalList(Node& formal, Node& old_list)
    {
        string formal_list = formal.getType() + "," + old_list.getType();
        // todo: delete pre nodes
        this->type = formal_list;
    }

};

class ExpList : public Node
{
    vector<string> exp_list;
public:
    ExpList(Node& exp)
    {
            this->exp_list.insert(this->exp_list.begin(), exp.getType());
    }
    ExpList(Node& exp, Node* exp_list){
        ExpList* old_exp_list = dynamic_cast<ExpList*>(exp_list);
        this->exp_list = *(old_exp_list->getExpList());
        delete old_exp_list;
        this->exp_list.insert(this->exp_list.begin(),exp.getType());
    }
    void insert(Exp& exp)
    {
        this->exp_list.insert(this->exp_list.begin(),exp.getType());
    }
    vector<string>* getExpList()
    {
        return &(this->exp_list);
    }
};

class Call : public Node
{
public:
    Call(Node& function_name, Node* exp_list);
    Call(Node& function_name);
};

#endif