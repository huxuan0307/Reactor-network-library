#include "../network/base/Timestamp.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    Timestamp t{Timestamp::now()};
    cout<<t.toString()<<endl;
    t+=2;
    cout<<t.toString()<<endl;
}
