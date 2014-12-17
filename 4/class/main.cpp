#include <iostream>
#include "classP.h"
#include "dau.h"
using namespace std;

int main()
{
    father cc;
    cc.print();
    dau mq;
    mq.print();
    cc.print();

    dau mq2(24);
    mq2.print();
    father cc2;
    cc2.print();

    return 0;
}
