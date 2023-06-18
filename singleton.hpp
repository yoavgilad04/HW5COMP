#ifndef HW5COMP_SINGLETON_H
#define HW5COMP_SINGLETON_H

#include <iostream>
using namespace std;

class Singleton {
private:
    int x;
    static Singleton *instance_pointer;

    Singleton() {}

public:
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
};

Singleton* Singleton ::instance_pointer = NULL;

Singleton *compi = Singleton::getInstance();

#endif //HW5COMP_SINGLETON_H
