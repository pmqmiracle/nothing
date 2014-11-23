#ifndef DAU_H
#define DAU_H
#include "classP.h"

class dau:public father
{
public:
    dau()
    {
        this->number = 10;
    }
    dau(int aa):father(aa)
    {
        this->number = aa*2;
    }
    void print()
    {
        cout << "I am daughter " << this->number << endl;
    }
};

#endif
