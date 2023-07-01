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
public:
    Node(string type=""): type(type), line(-1){};
    Node(int line): type(""), line(line){};

    string getType(){return this->type;}
    int getLine(){return this->line;}
    void setLine(int line){this->line = line;}
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
    ExpList(Node& exp)
    {
//        this->exp_list.insert(this->exp_list.begin(), exp.getType());
        Exp* e = dynamic_cast<Exp*>(&exp);
        this->insert(*e);
    }
    ExpList(Node& exp, Node* exp_list){
        ExpList* old_exp_list = dynamic_cast<ExpList*>(exp_list);
        this->exp_list = *(old_exp_list->getExpList());
        delete old_exp_list;
        Exp* e = dynamic_cast<Exp*>(&exp);
        this->insert(*e);
//        this->exp_list.insert(this->exp_list.begin(),exp.getType());
    }
    void insert(Exp exp)
    {
        this->exp_list.push_back(exp);
    }
    vector<Exp>* getExpList()
    {
        return &(this->exp_list);
    }
};

class Call : public Node
{
    string llvm_var;
public:
    Call(Node& function_name, Node* exp_list);
    Call(Node& function_name);
    string getLLVMName(){return this->llvm_var;}
};

class BreakOrCont
{
    int cond_line;
    int loop_num;
public:
    BreakOrCont(int cond_line, int loop_num):cond_line(cond_line), loop_num(loop_num){}
    int getLine(){return cond_line;}
    int getLoopNum(){return loop_num;}
};
#endif