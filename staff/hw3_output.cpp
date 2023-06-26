#include <iostream>
#include "hw3_output.hpp"
#include <sstream>

using namespace std;

void output::endScope(){
    //cout << "---end scope---" << endl;
}

void output::printID(const string& id, int offset, const string& type) {
    cout << id << " " << type <<  " " << offset <<  endl;
}

string typeListToString(const std::vector<string>& argTypes) {
    stringstream res;
    res << "(";
    for(int i = 0; i < argTypes.size(); ++i) {
        res << argTypes[i];
        if (i + 1 < argTypes.size())
            res << ",";
    }
    res << ")";
    return res.str();
//    return "";
}

string valueListsToString(const std::vector<string>& values) {
    stringstream res;
    res << "{";
    for(int i = 0; i < values.size(); ++i) {
        res << values[i];
        if (i + 1 < values.size())
            res << ",";
    }
    res << "}";
    return res.str();
//    return "";
}

string output::makeFunctionType(const string& retType, std::vector<string>& argTypes) {
    stringstream res;
    res << typeListToString(argTypes) << "->" << retType;
    return res.str();
//    return "";
}

void output::errorLex(int lineno){
    cout << "line " << lineno << ":" << " lexical error" << endl;
    exit(0);
}

void output::errorSyn(int lineno){
    cout << "line " << lineno << ":" << " syntax error" << endl;
    exit(0);
}

void output::errorUndef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " variable " << id << " is not defined" << endl;
    exit(0);
}

void output::errorDef(int lineno, const string& id){
    cout << "line " << lineno << ":" << " identifier " << id << " is already defined" << endl;
    exit(0);
}

void output::errorUndefFunc(int lineno, const string& id) {
    cout << "line " << lineno << ":" << " function " << id << " is not defined" << endl;
    exit(0);
}

void output::errorMismatch(int lineno){
    cout << "line " << lineno << ":" << " type mismatch" << endl;
    exit(0);
}

void output::errorPrototypeMismatch(int lineno, const string& id) {
    cout << "line " << lineno << ": prototype mismatch, function " << id << endl;
    exit(0);
}

void output::errorUnexpectedBreak(int lineno) {
    cout << "line " << lineno << ":" << " unexpected break statement" << endl;
    exit(0);
}

void output::errorUnexpectedContinue(int lineno) {
    cout << "line " << lineno << ":" << " unexpected continue statement" << endl;
    exit(0);
}

void output::errorMainMissing() {
    cout << "Program has no 'void main()' function" << endl;
    exit(0);
}

void output::errorByteTooLarge(int lineno, const string& value) {
    cout << "line " << lineno << ": byte value " << value << " out of range" << endl;
    exit(0);
}

void output::errorFuncNoOverride(int lineno, const string& id) {
    cout << "line " << lineno << ": function " << id << " was declared before as non-override function" << endl;
    exit(0);
}

void output::errorOverrideWithoutDeclaration(int lineno, const string& id) {
    cout << "line " << lineno << ": function " << id << " attempt to override a function without declaring the current function as override" << endl;
    exit(0);
}

void output::errorAmbiguousCall(int lineno, const string& id) {
    cout << "line " << lineno << ": ambiguous call to overloaded function " << id << endl;
    exit(0);
}

void output::errorMainOverride(int lineno){
    cout << "line " << lineno << ": main is not allowed to be overridden" << endl;
    exit(0);
}

// TODO: DELETE
const std::string output::rules[] = {
        "Program -> Funcs",
        "Funcs -> epsilon",
        "Funcs -> FuncDecl Funcs",
        "FuncDecl -> OverRide RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE",
        "OverRide -> epsilon",
        "OverRide -> OVERRIDE",
        "RetType -> Type",
        "RetType ->  VOID",
        "Formals -> epsilon",
        "Formals -> FormalsList",
        "FormalsList -> FormalDecl",
        "FormalsList -> FormalDecl COMMA FormalsList",
        "FormalDecl -> Type ID",
        "Statements -> Statement",
        "Statements -> Statements Statement",
        "Statement -> LBRACE Statements RBRACE",
        "Statement -> Type ID SC",
        "Statement -> Type ID ASSIGN Exp SC",
        "Statement -> ID ASSIGN Exp SC",
        "Statement -> Call SC",
        "Statement -> RETURN SC",
        "Statement -> RETURN Exp SC",
        "Statement -> IF LPAREN Exp RPAREN Statement",
        "Statement -> IF LPAREN Exp RPAREN Statement ELSE Statement",
        "Statement -> WHILE LPAREN Exp RPAREN Statement",
        "Statement -> BREAK SC",
        "Statement -> CONTINUE SC",
        "Call -> ID LPAREN ExpList RPAREN",
        "Call -> ID LPAREN RPAREN",
        "ExpList -> Exp",
        "ExpList -> Exp COMMA ExpList",
        "Type -> INT",
        "Type -> BYTE",
        "Type -> BOOL",
        "Exp -> LPAREN Exp RPAREN",
        "Exp -> Exp BINOP Exp",
        "Exp -> ID",
        "Exp -> Call",
        "Exp -> NUM",
        "Exp -> NUM B",
        "Exp -> STRING",
        "Exp -> TRUE",
        "Exp -> FALSE",
        "Exp -> NOT Exp",
        "Exp -> Exp AND Exp",
        "Exp -> Exp OR Exp",
        "Exp -> Exp RELOP Exp",
        "Exp -> LPAREN Type RPAREN Exp"
};

void output::printProductionRule(const int ruleno) {
    std::cout << ruleno << ": " << output::rules[ruleno-1] << "\n";
}