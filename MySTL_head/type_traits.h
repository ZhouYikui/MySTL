//
// Created by ZYK on 2023/4/17.
//

/**
 * @file type_traits.h
 * @brief 用于提取类型信息
 *
 * @date 2023年4月16日
 * @author ZYK
 */

#ifndef MYSTL_TYPE_TRAITS_H
#define MYSTL_TYPE_TRAITS_H

#include <type_traits>

namespace mystl
{

    /**
     * @brief 主要目的是定义一个value并被初始化为T v，且可以静态访问
     * @brief 在此基础上定义了两个别名模板，表示true and false
     * @note 在模板元编程中使用这些类型作为返回值或函数参数类型，从而实现在编译期间进行判断和操作
     * @note 还可以避免出现裸露的布尔类型，使代码更具可读性和可维护性
     * */

    template<typename T, T v>
    struct m_integral_constant
    {
        static constexpr T value = v;
    };

    template<bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    typedef m_bool_constant<true> m_true_type;
    typedef m_bool_constant<false> m_false_type;

    /**
     * @brief pair的前向声明
     * */
    template<typename T1, typename T2>
    struct pair;

    /**
     * @brief 模板特化的方法 对于非pair，默认初始化为false_type，对于pair，默认初始化为true_type
     * */

    template <typename T>
    struct is_pair : mystl::m_false_type {};

    template <typename T1, typename T2>
    struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};

}

#endif //MYSTL_TYPE_TRAITS_H
