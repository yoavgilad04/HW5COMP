#ifndef HW5COMP_SINGLETON_H
#define HW5COMP_SINGLETON_H

#include <iostream>
#include <string>
using namespace std;

class Singleton {
private:
    int x;
    int curr_val;
    static Singleton *instance_pointer;


    Singleton() : curr_val(0), in_making_statement("") {}

public:
    string in_making_statement;

    Singleton(const Singleton &obj) = delete;

    static Singleton *getInstance() {
        if (instance_pointer == NULL) {
            instance_pointer = new Singleton();
            return instance_pointer;
        } else {
            return instance_pointer;
        }
    }

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
};

Singleton* Singleton ::instance_pointer = NULL;
//
//Singleton *compi = Singleton::getInstance();

#endif //HW5COMP_SINGLETON_H
