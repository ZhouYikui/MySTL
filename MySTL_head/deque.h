//
// Created by HP on 2023/5/26.
//

/**
 * @file deque.h
 * @brief 实现模板类deque 双端队列
 *
 * @date 2023年5月26日
 * @author ZYK
 */

#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif
#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif

#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

    template<typename T>
    struct deque_buf_size
    {
        static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
    };

    /// ================================================================================================================
    /// @brief deque 迭代器
    /// ================================================================================================================
    template<typename T, typename Ref, typename Ptr>
    class deque_iterator : public iterator<random_access_iterator_tag, T>
    {
        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef deque_iterator self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *value_pointer;
        typedef T **map_pointer;
    };

}

#endif //MYSTL_DEQUE_H
