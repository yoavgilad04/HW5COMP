#ifndef LLVM_UTILS_H
#define LLVM_UTILS_H

//#include "singleton.hpp"

#include <iostream>
#include <string>
using namespace std;

string makeZextConvertStatement(string target, string type, string value)
{
    string output_string = "%" + target + " = zext i";
    if (type == "BYTE")
    {
        output_string += "8 ";
    }
    if (type == "BOOL")
    {
        output_string += "1 ";
    }
    output_string += value + " to i32";
    return output_string;
}

string makeBinaryStatement(string target, string op, string var1, string var2)
{
    string output_string = "%" + target + " = ";
    if(op == "ADD")
    {
        output_string += "add ";
    }


    output_string += "i32 %" + var1 + ", " + var2;

    return output_string;
}

#endif //LLVM_UTILS_H