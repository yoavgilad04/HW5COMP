#ifndef HW5COMP_SINGLETON_H
#define HW5COMP_SINGLETON_H

#include <iostream>
#include <string>
#include "staff/bp.hpp"
using namespace std;


class Singleton {
private:
    int x;
    int curr_val;
    CodeBuffer* code_buffer;
    static Singleton* sin_instance;

    Singleton() : curr_val(0), code_buffer(){}

public:
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

    string getFreshVar()
    {
        string new_var_name = "var" + to_string(this->curr_val);
        this->curr_val++;
        return new_var_name;
    }

    void addNumToBuffer(string target, string value1)
    {
        Singleton* compi = Singleton::getInstance();
        string full_command = compi->makeBinaryStatement(target, "ADD", "0", value1);
        cout << full_command << endl;
//        int line = code_buffer->emit(full_command);
    }

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
//        if (this->startsWith(var1, "var"))
//            var1 = "%" + var1;
//        if (this->startsWith(var2, "var"))
//            var1 = "%" + var2;
        output_string += "i32 " + var1 + ", " + var2;
        return output_string;
    }

    bool startsWith(const std::string& str, const std::string& prefix)
    {
        if (str.length() < prefix.length()) {
            return false;  // String is shorter than the prefix
        }

        return str.substr(0, prefix.length()) == prefix;
    }

};



#endif //HW5COMP_SINGLETON_H
