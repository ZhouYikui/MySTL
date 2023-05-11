cmake_minimum_required(VERSION 3.23)
project(MySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(MySTL main.cpp MySTL_head/vector.h MySTL_head/allocator.h MySTL_head/construct.h MySTL_head/util.h MySTL_head/iterator.h MySTL_head/type_traits.h MySTL_head/algobase.h MySTL_head/uninitialized.h MySTL_head/exceptdef.h MySTL_head/memory.h MySTL_head/algo.h)
