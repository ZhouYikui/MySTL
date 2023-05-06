//
// Created by ZYK on 2023/4/14.
//

/**
 * @file util.h
 * @brief 实现STL通用工具
 *
 * @date 2023年4月14日
 * @author ZYK
 */

#ifndef MYSTL_UTIL_H
#define MYSTL_UTIL_H

#include <type_traits>

namespace mystl
{

    /**
     * @brief 模板函数move，实现移动语义（也就是强制arg转化为右值引用）
     * @param[in] arg 引用折叠规则，既可以是左值，又可以是右值
     * @return std::remove_reference<T>::type&& 返回右值引用类型
     * @note 本质上是一个类型转换
     * */

    template<typename T>
    typename std::remove_reference<T>::type &&move(T &&arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type &&>(arg);
    }

    /**
     * @brief 模板函数forward，实现完美转发
     * @param[in] arg 要转发的参数
     * @return T&& 右值引用，保持参数的特性，即传递左值则返回左值引用，传递右值则返回右值引用
     * */
    template<typename T>
    T &&forward(typename std::remove_reference<T>::type &arg) noexcept
    {
        return static_cast<T &&> (arg);
    }

    template<typename T>
    T &&forward(typename std::remove_reference<T>::type &&arg) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T &&>(arg);
    }

    /**
     * @brief 模板函数swap，交换两个值
     *
     * */
    template<typename T>
    void swap(T &lhs, T &rhs)
    {
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }
}

#endif //MYSTL_UTIL_H
