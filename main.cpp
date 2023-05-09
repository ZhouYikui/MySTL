#include "MySTL_head/uninitialized.h"
#include "MySTL_head/vector.h"
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

int main()
{
    mystl::vector<int> v1;
    mystl::uninitialized_move(v1.begin(), v1.end(), v1.begin());
    return 0;
}
