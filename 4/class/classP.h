#ifndef CLASS_P
#define CLASS_P
#include <iostream>
using namespace std;

class father
{
public:
    father()
    {
        this->number = 110;
    }
    father(int a)
    {
        this->number = a;
    }
    void print()
    {
        cout << "I am father " <<  this->number << endl;
    }

protected:
    int number;
};

#endif
