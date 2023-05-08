#include "MySTL_head/vector.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    mystl::vector<int> v2;
    cout << v2.max_size() << endl;
    return 0;
}
