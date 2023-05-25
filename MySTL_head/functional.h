//
// Created by HP on 2023/5/25.
//

/**
 * @file functional.h
 * @brief 包含函数对象
 *
 * @date 2023年5月25日
 * @author ZYK
 */

#ifndef MYSTL_FUNCTIONAL_H
#define MYSTL_FUNCTIONAL_H

#include <cstddef>

namespace mystl
{
    /// @brief 定义一元函数的参数型别和返回值型别
    template<typename Arg, typename Result>
    struct unarg_function
    {
        typedef Arg argument_type;
        typedef Result result_type;
    };

    /// @brief 定义二元函数的参数型别和返回值型别
    template<typename Arg1, typename Arg2, typename Result>
    struct binary_function
    {
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argument_type;
        typedef Result result_type;
    };

    /// @brief 函数对象 等于
    template<typename T>
    struct equal_to : public binary_function<T, T, bool>
    {
        bool operator()(const T &x, const T &y) const { return x == y; }
    };
}

#endif //MYSTL_FUNCTIONAL_H
