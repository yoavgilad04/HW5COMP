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
#include "staff/bp.hpp"


extern int yylineno;
using namespace std;


class Node {
protected:
    string type;
    int line;
    int str_length;
public:
    Node(string type=""): type(type), line(-1), str_length(0){};
    Node(int line): type(""), line(line){};

    string getType() const {return this->type;}
    int getLine(){return this->line;}
    void setLine(int line){this->line = line;}
    int getLength(){return this->str_length;}
    virtual ~Node() = default;
};

#define YYSTYPE Node*

class Exp : public Node
{
    string llvm_var;
    vector<pair<int,BranchLabelIndex>> trueList;
    vector<pair<int,BranchLabelIndex>> falseList;
    vector<pair<int,BranchLabelIndex>> nextList;

public:
    Exp(string type, string value);
    Exp(string type);
    Exp(string operation_val, Node& exp_1, Node& exp_2, string op="TihiyeLiBari");
    Exp(Node& exp_1, string operation_val, Node& exp_2, string op="TihiyeLiBari", string exp_2_label="");
    Exp(Node& n);
    Exp(Node& exp, const string& conversion_type);
    string getLLVMName(){return this->llvm_var;}
    void setLLVMName(string llvm_var){this->llvm_var = llvm_var;}
    vector<pair<int,BranchLabelIndex>> getTrueList(){return this->trueList;}
    vector<pair<int,BranchLabelIndex>> getFalseList(){return this->falseList;}
    void setFalseList(vector<pair<int,BranchLabelIndex>> lst){this->falseList = lst;}
    void setTrueList(vector<pair<int,BranchLabelIndex>> lst){this->trueList = lst;}


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
    std::vector<Exp> exp_list;
public:
    ExpList(Node& exp);
    ExpList(Node& exp, Node* exp_list);
    void insert(Exp exp) {this->exp_list.insert(this->exp_list.begin(), exp);}
    vector<Exp>* getExpList() {return &(this->exp_list);}
};

class Call : public Node
{
    string llvm_var;
public:
    Call(Node& function_name, Node* exp_list);
    Call(Node& function_name);
    string getLLVMName(){return this->llvm_var;}
};

class While
{
    string goto_label;
    vector<pair<int,BranchLabelIndex>> nextList;

public:
    While(string goto_label): goto_label(goto_label), nextList(){}
    string getLabel(){return goto_label;}
    vector<pair<int,BranchLabelIndex>> getList(){return nextList;}
    void setList(vector<pair<int,BranchLabelIndex>> v1){this->nextList = v1;}
};
#endif