//
// Created by ZYK on 2023/4/14.
//

/**
 * @file allocator.h
 * @brief 实现模板类allocator(内存分配回收，对象构造析构)
 *
 * @date 2023年4月14日
 * @author ZYK
 */

#ifndef MYSTL_ALLOCATOR_H
#define MYSTL_ALLOCATOR_H

#include <cstddef>
#include "construct.h"

namespace mystl
{
    template<typename T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief allocate 分配空间
        /// ------------------------------------------------------------------------------------------------------------

        static T *allocate();

        static T *allocate(size_type n);

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief deallocate 回收空间
        /// ------------------------------------------------------------------------------------------------------------

        static void deallocate(T *ptr);

        static void deallocate(T *ptr, size_type n);

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief construct 构造
        /// ------------------------------------------------------------------------------------------------------------

        static void construct(T *ptr);

        static void construct(T *ptr, const T &value);

        static void construct(T *ptr, T &&value);

        template<typename... Args>
        static void construct(T *ptr, Args &&...args);

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief destroy 析构
        /// ------------------------------------------------------------------------------------------------------------

        static void destroy(T *ptr);

        static void destroy(T *first, T *last);

    };

    /**
     * @brief allocate函数，分配相应空间并返回相应类型的指针
     * @param[in] n 分配的大小
     * @return T* 指向分配的空间
     * */

    template<typename T>
    T *allocator<T>::allocate()
    {
        // 全局函数 operator new，分配内存返回void指针，且不初始化
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    template<typename T>
    T *allocator<T>::allocate(size_type n)
    {
        if (n == 0) return nullptr;
        return static_cast<T *> (::operator new(n * sizeof(T)));
    }

    /**
     * @brief deallocate函数，释放指针指向的空间
     * @param[in] ptr 指向要释放的内存
     * @param[in] n 释放的大小
     * */

    template<typename T>
    void allocator<T>::deallocate(T *ptr)
    {
        // 全局函数 operator delete，释放内存，但不调用析构函数
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::deallocate(T *ptr, size_type n)
    {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    /**
     * @brief construct函数, 在已分配的内存上调用构造函数
     * @param[in] ptr 指向已分配内存
     * @param[in] value 需要初始化的值
     * @param[in] args 可变参数
     * */

    template<typename T>
    void allocator<T>::construct(T *ptr)
    {
        mystl::construct(ptr);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr, const T &value)
    {
        mystl::construct(ptr, value);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr, T &&value)
    {
        mystl::construct(ptr, mystl::move(value));
    }

    template<typename T>
    template<typename ...Args>
    void allocator<T>::construct(T *ptr, Args &&...args)
    {
        mystl::construct(ptr, mystl::forward<Args>(args)...);
    }

    template<typename T>
    void allocator<T>::destroy(T *ptr)
    {
        mystl::destroy(ptr);
    }

    template<typename T>
    void allocator<T>::destroy(T *first, T *last)
    {
        mystl::destroy(first, last);
    }

}

#endif //MYSTL_ALLOCATOR_H
