#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include "MySTL_head/type_traits.h"
#include "MySTL_head/exceptdef.h"
#include "MySTL_head/util.h"

using namespace std;

void bar(int &&arg)
{
    cout << "RHS" << endl;
}

void bar(const int &arg)
{
    cout << "LHS" << endl;
}

template<typename T>
void foo(T &&arg)
{
    bar(mystl::forward<T>(arg));
}

int main()
{
    const int a = 10;
    foo(10);
    return 0;
}
