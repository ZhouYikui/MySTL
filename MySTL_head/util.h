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
    /// ================================================================================================================
    /// @brief move
    /// ================================================================================================================

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

    /// ================================================================================================================
    /// @brief forward
    /// ================================================================================================================

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

    /// ================================================================================================================
    /// @brief swap
    /// ================================================================================================================

    /// @brief 模板函数swap，交换两个值
    template<typename T>
    void swap(T &lhs, T &rhs)
    {
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }

    /// ================================================================================================================
    /// @brief pair
    /// ================================================================================================================

    template<typename T1, typename T2>
    struct pair
    {
        typedef T1 first_type;
        typedef T2 second_type;

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 成员变量
        /// ------------------------------------------------------------------------------------------------------------

        first_type first;
        second_type second;

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 构造函数
        /// ------------------------------------------------------------------------------------------------------------

        /**
         * @brief pair中定义了默认构造函数，有参构造，拷贝构造，移动构造，且分别有允许显式和隐式调用的版本
         * */

        /// @brief 默认构造函数
        template<typename Other1 = T1, typename Other2 = T2, typename = typename std::enable_if<
                std::is_default_constructible<Other1>::value &&
                std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair() : first(), second() {}

        /// @brief 拷贝构造函数
        pair(const pair &lhs) = default;

        /// @brief 移动构造函数
        pair(pair &&rhs) noexcept = default;

        /// @brief 隐式 有参构造函数
        template<typename U1 = T1, typename U2 = T2, typename std::enable_if<
                std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                std::is_convertible<const U1 &, T1>::value &&
                std::is_convertible<const U2 &, T2>::value, int>::type = 0>
        constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

        /// @brief 显式 有参构造函数
        template<typename U1 = T1, typename U2 = T2, typename std::enable_if<
                std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                (!std::is_convertible<U1, T1>::value || !std::is_convertible<U2, T2>::value), int>::type = 0>
        explicit constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

        /// @brief 隐式 移动构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, Other1>::value &&
                std::is_constructible<T2, Other2>::value &&
                std::is_convertible<Other1 &&, T1>::value &&
                std::is_convertible<Other2 &&, T2>::value, int>::type = 0>
        constexpr pair(Other1 &&a, Other2 &&b) : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

        /// @brief 显式 移动构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, Other1>::value &&
                std::is_constructible<T2, Other2>::value &&
                (!std::is_convertible<Other1, T1>::value || !std::is_convertible<Other2, T2>::value), int>::type = 0>
        explicit constexpr
        pair(Other1 &&a, Other2 &&b) : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

        /// @brief 隐式 拷贝构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, const Other1 &>::value &&
                std::is_constructible<T2, const Other2 &>::value &&
                std::is_convertible<const Other1 &, T1>::value &&
                std::is_convertible<const Other2 &, T2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2> &other) : first(other.first), second(other.second) {}

        /// @brief 显式 拷贝构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, const Other1 &>::value &&
                std::is_constructible<T2, const Other2 &>::value &&
                (!std::is_convertible<const Other1 &, T1>::value ||
                 !std::is_convertible<const Other2 &, T2>::value), int>::type = 0>
        explicit constexpr
        pair(const pair<Other1, Other2> &other) : first(other.first), second(other.second) {}

        /// @brief 隐式 移动构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, Other1>::value &&
                std::is_constructible<T2, Other2>::value &&
                std::is_convertible<Other1, T1>::value &&
                std::is_convertible<Other2, T2>::value, int>::type = 0>
        constexpr pair(pair<Other1, Other2> &&other) : first(mystl::forward<Other1>(other.first)),
                                                       second(mystl::forward<Other2>(other.second)) {}

        /// @brief 显式 移动构造函数
        template<typename Other1, typename Other2, typename std::enable_if<
                std::is_constructible<T1, Other1>::value &&
                std::is_constructible<T2, Other2>::value &&
                (!std::is_convertible<Other1, T1>::value ||
                 !std::is_convertible<Other2, T2>::value), int>::type = 0>
        explicit constexpr
        pair(pair<Other1, Other2> &&other) : first(mystl::forward<Other1>(other.first)),
                                             second(mystl::forward<Other2>(other.second)) {}

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 赋值运算符重载
        /// ------------------------------------------------------------------------------------------------------------

        /// @brief 拷贝赋值运算
        pair &operator=(const pair &lhs)
        {
            if (this != &lhs)
            {
                first = lhs.first;
                second = lhs.second;
            }
            return *this;
        }

        /// @brief 移动赋值运算
        pair &operator=(pair &&rhs)
        {
            if (this != &rhs)
            {
                first = mystl::move(rhs.first);
                second = mystl::move(rhs.second);
            }
            return *this;
        }

        template <typename Other1, typename Other2>
        pair& operator=(const pair<Other1, Other2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        template<typename Other1, typename Other2>
        pair &operator=(pair<Other1, Other2> && other)
        {
            first = mystl::forward<Other1>(other.first);
            second = mystl::forward<Other2>(other.second);
            return *this;
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 析构函数
        /// ------------------------------------------------------------------------------------------------------------

        ~pair() = default;

    };

}

#endif //MYSTL_UTIL_H
