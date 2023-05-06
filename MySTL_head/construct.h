//
// Created by ZYK on 2023/4/14.
//

/**
 * @file construct.h
 * @brief 实现模板函数construct（在内存上构造对象）destory (析构对象)
 *
 * @date 2023年4月14日
 * @author ZYK
 */

#ifndef MYSTL_CONSTRUCT_H
#define MYSTL_CONSTRUCT_H

#include <new>
#include "util.h"
#include "iterator.h"

namespace mystl
{

    /// ================================================================================================================
    /// @brief mystl::construct
    /// ================================================================================================================

    /**
     * @brief 模板函数construct，用于在给定地址处构造对象
     * @param[in] ptr 需要构造对象的地址
     * @param[in] value 需要传递给构造函数的值引用
     * @param[in] args  需要传递给构造函数的可变数量的参数(通过完美转发传递给构造函数)
     * */
    template<typename T1>
    void construct(T1 *ptr)
    {
        ::new((void *) ptr) T1();
    }

    template<typename T1, typename T2>
    void construct(T1 *ptr, const T2 &value)
    {
        ::new((void *) ptr) T1(value);
    }

    template<typename T1, typename... Args>
    void construct(T1 *ptr, Args &&... args)
    {
        ::new((void *) ptr) T1(mystl::forward<Args>(args)...);
    }

    /// ================================================================================================================
    /// @brief mystl::destory
    /// ================================================================================================================

    /**
     * @brief 模板函数destroy_one
     * @param[in] pointer 指向对象
     * @param[in] std::true_type 不做操作
     * @param[in] std::false_type 调用析构函数
     * */

    template<typename T>
    void destory_one(T *, std::true_type) {}

    template<typename T>
    void destroy_one(T *pointer, std::false_type)
    {
        if (pointer != nullptr)
        {
            pointer->~T();
        }
    }

    /**
     * @brief 模板函数destroy_cat
     * @param[in] ForwardIter 迭代器
     * @param[in] std::true_type 不做操作
     * @param[in] std::false_type 调用析构函数
     * */

    template<typename ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

    template<typename ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }

    /**
     * @brief 模板函数destroy，用于在给定内存处调用析构函数
     * @param[in] pointer 指向内存空间
     * */

    template<typename T>
    void destroy(T *pointer)
    {
        // is_trivially_destructible 判断是否易销毁 (返回false_type or true_type类型)
        destroy_one(pointer, std::is_trivially_destructible<T>{});
    }

    template<typename ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type>{});
    }

}

#endif //MYSTL_CONSTRUCT_H
