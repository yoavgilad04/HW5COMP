#ifndef HW5COMP_SINGLETON_H
#define HW5COMP_SINGLETON_H

#include <iostream>
#include <string>
#include "staff/bp.hpp"
#include "symbols/symbol.hpp"
#include "source.hpp"
using namespace std;


class Singleton {
private:
    int x;
    int curr_val;
    static Singleton* sin_instance;

    Singleton() : curr_val(0), code_buffer(new CodeBuffer()){}

public:
    CodeBuffer* code_buffer;

    static Singleton* getInstance() {
        if (sin_instance == nullptr) {
            sin_instance = new Singleton();
            std::cout << "Creating a new singleton instance." << std::endl;
        }
        return sin_instance;
    }
    Singleton(const Singleton &obj) = delete;
    void setInt(int y) {
        this->x = y;
    }

    void print()
    {
        cout << this->x << endl;
    }

    string convertTypeToIType(string type)
    {
        string i_type = "something";
        if(type == "INT")
            i_type = "i32";
        if(type == "BOOL")
            i_type = "i1";
        if(type == "BYTE")
            i_type = "i8";
        if(type == "VOID")
            i_type = "void"; //todo: what i type is void?
        return i_type;
    }

    string retCommand(string type="", string var="")
    {
        string output_string = "ret";
        if(type != "")
        {
            output_string += " " + this->convertTypeToIType(type);
            if(this->startsWith(var, "var"))
                output_string += " %";
            output_string += var;
        }
        return output_string;

    }
    string getFreshVar()
    {
        string new_var_name = "var" + to_string(this->curr_val);
        this->curr_val++;
        return new_var_name;
    }

    string makeLoadCommand(string target, string type, string var)
    {
        string output_string = "%" + target + " = load " + this->convertTypeToIType(type) + "* %" + var;
        return output_string;
    }

    string storeCommand(string target, string type, string var)
    {
        string i_type = this->convertTypeToIType(type);
        string output_string = "store " + i_type + " ";
        if (this->startsWith(var, "var"))
            output_string +="%";
        output_string += var + ", " + i_type + "* %" + target;
        return output_string;
    }
    string allocCommand(string target, string type)
    {
        string output_string = "%" + target + " = alloca ";
        string i_type = this->convertTypeToIType(type);
        output_string += i_type;
        return output_string;
    }
    void addAssignmentCommand(string target, string value1)
    {
        Singleton* compi = Singleton::getInstance();
        string full_command = compi->makeBinaryStatement(target, "ADD", value1, "0");
        compi->code_buffer->emit(full_command);
    }

    string makeZextConvertStatement(string target, string type, string value, isZext=true)
    {
        string action;
        if (isZext)
            action = "zext";
        else
            action = "trunc";
    string output_string = "%" + target + " = " + action " i";
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

    void printBuffer()
    {
        cout << "===================================================================================================================" << endl;
        this->code_buffer->printGlobalBuffer();
        this->code_buffer->printCodeBuffer();
    }

    void addCheckIfZeroFunction()
    {
        vector<string> emit_strings = vector<string>{
                "",
                "define void @divByZero() {",
                "    %spec_ptr = getelementptr [24 x i8], [24 x i8]* @.divide_by_zero_error, i32 0, i32 0",
                "    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)",
                "    %exitCode = add i32 0, 0",
                "    call void @exit(i32 %exitCode)",
                "}",
                "",
                };
        for(int i=0; i<emit_strings.size();i++)
        {
            code_buffer->emit(emit_strings[i]);
        }

    }
    void checkIfZero(string var)
    {
        string new_var = this->getFreshVar();
        this->code_buffer->emit(this->makeCompStatement(new_var, "==", var, "0"));
        int line = this->code_buffer->emit(this->makeGoToCondStatement(new_var));
        string true_label = this->code_buffer->genLabel();
        code_buffer->emit("call i32 @divByZero()");
        string false_label = this->code_buffer->genLabel();
        code_buffer->bpatch(code_buffer->makelist(pair<int,BranchLabelIndex>{line,FIRST}), true_label);
        code_buffer->bpatch(code_buffer->makelist(pair<int,BranchLabelIndex>{line ,SECOND}), false_label);
    }

    void addFunction(FuncSymbol* s)
    {
        vector<string> args = s->getArgs();
        string base_string = this->convertTypeToIType(s->getType()) + " @" + s->getLLVMName() + "_" + s->getName()  + "(";
//        string declare_string = "declare " + base_string;
//        for(int i=0; i<args.size(); i++)
//        {
//            if (i == args.size()-1)
//                declare_string += this->convertTypeToIType(args[i]);
//            else
//                declare_string += this->convertTypeToIType(args[i]) + ", ";
//        }
//        declare_string += ")";
//        this->code_buffer->emit(declare_string);
        string define_string = "define " + base_string;
        int args_size = args.size();
        for(int i=0; i<args_size - 1; i++)
        {
            define_string +=  "i32, ";
        }
        if(args_size > 0)
        {
            define_string += "i32";
        }
        define_string += ")";
        this->code_buffer->emit(define_string);
    }

    string makeBinaryStatement(string target, string op, string var1, string var2, bool isByte=false)
    {
        string output_string = "%" + target + " = ";
        if(op == "ADD")
            output_string += "add ";
        if(op == "SUB")
            output_string += "sub ";
        if(op == "MUL")
            output_string += "mul ";
        if(op == "DIV")
        {
            this->checkIfZero(var2);
            output_string += "udiv ";
        }
        if (this->startsWith(var1, "var"))
            var1 = "%" + var1;
        if (this->startsWith(var2, "var"))
            var2 = "%" + var2;
        string type = "i32";
        if (isByte)
            type = "i8";
        output_string += type + " " + var1 + ", " + var2;
        return output_string;
    }

    string makeCompStatement(string target, string comp_op, string var1, string var2)
    {
        string output_string = "%" + target + " = icmp ";
        if (comp_op == "<")
            output_string += "ult ";
        if (comp_op == ">")
            output_string += "ugt ";
        if (comp_op == "<=")
            output_string += "ule ";
        if (comp_op == ">=")
            output_string += "uge ";
        if (comp_op == "==")
            output_string += "eq ";
        if (comp_op == "!=")
            output_string += "ne ";
        if (this->startsWith(var1, "var"))
            var1 = "%" + var1;
        if (this->startsWith(var2, "var"))
            var2 = "%" + var2;
        output_string += "i32 " + var1 + ", " + var2;
        return output_string;
    }

    string makeGoToCondStatement(string target)
    {
        string output_string = "br i1 %" + target + ", label @, label @";
        return output_string;
    }

    string makeGoToStatement()
    {
        string output_string = "br label @";
        return output_string;
    }

    bool startsWith(const std::string& str, const std::string& prefix)
    {
        if (str.length() < prefix.length()) {
            return false;  // String is shorter than the prefix
        }

        return str.substr(0, prefix.length()) == prefix;
    }

    void addStringStatement(string target, string input_string)
    {
        string output_string = "%" + target + " = alloca [" + std::to_string(int(input_string.size()) + 2) + "x i8] c\"";
        output_string += input_string + "\\0A\\00\"";
        this->code_buffer->emit(output_string);
    }

    void makePrintImplementation()
    {
        vector<string> emit_strings = vector<string>{
                "",
                "define void @printi(i32) {",
                "    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0",
                "    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)",
                "    ret void",
                "}",
                "",
                "define void @print(i8*) {",
                "    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0",
                "    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)",
                "    ret void",
                "}",
                ""
        };
        for(int i=0; i<emit_strings.size();i++)
        {
            code_buffer->emit(emit_strings[i]);
        }

    }
    void addFunctionCall(string target, string func_name, vector<Exp>& func_args)
    {
        string output_string = "%" + target + " = call i32 @" + func_name + "(";
        int last_index = func_args.size()-1;
        for(int i=0;i<last_index;i++)
        {
            output_string += "i32 %" + func_args[i].getLLVMName() + ", ";
        }
        if(last_index >= 0)
        {
            output_string += "i32 %" + func_args[last_index].getLLVMName();
        }
        output_string += ")";
        this->code_buffer->emit(output_string);
    }

};



#endif //HW5COMP_SINGLETON_H
