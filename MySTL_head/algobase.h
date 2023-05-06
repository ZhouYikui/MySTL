//
// Created by HP on 2023/4/22.
//

/**
 * @file algobase.h
 * @brief 包含了mystl的基本算法
 *
 * @date 2023年4月14日
 * @author ZYK
 */

#ifndef MYSTL_ALGOBASE_H
#define MYSTL_ALGOBASE_H

#include <cstring>
#include "iterator.h"
#include "util.h"

namespace mystl
{
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

    /// ================================================================================================================
    /// @biref max函数
    /// ================================================================================================================

    template<typename T>
    const T &max(const T &lhs, const T &rhs)
    {
        return lhs >= rhs ? lhs : rhs;
    }

    template<typename T, typename Compare>
    const T &max(const T &lhs, const T &rhs)
    {
        return Compare(lhs, rhs) ? rhs : lhs;
    }

    /// ================================================================================================================
    /// @brief min函数
    /// ================================================================================================================

    template<typename T>
    const T &min(const T &lhs, const T &rhs)
    {
        return lhs <= rhs ? lhs : rhs;
    }

    template<typename T, typename Compare>
    const T &min(const T &lhs, const T &rhs)
    {
        return Compare(rhs, lhs) ? rhs : lhs;
    }

    /// ================================================================================================================
    /// @brief copy series
    /// ================================================================================================================

    /**
     * @brief copy辅助函数，根据不同的迭代器类型选择不同的处理方式
     * */

    template<typename InputIter, typename OutputIter>
    OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    template<typename RandomIter, typename OutputIter>
    OutputIter
    unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    /**
     * @brief unchecked_copy 不同情况下调用不同重载
     * @note void* memmove ( void* dest, const void* src, std::size_t count );
     * */

    template<typename InputIter, typename OutputIter>
    OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy_cat(first, last, result, iterator_category(first));
    }

    template<typename Tp, typename Up>
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_copy_assignable<Up>::value, Up *>::type
    unchecked_copy(Tp *first, Tp *last, Up *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) std::memmove(result, first, n * sizeof(Up));
        /// @return 下一个要写入的位置
        return result + n;
    }

    template<typename InputIter, typename OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy(first, last, result);
    }

    /// ================================================================================================================
    /// @brief copy_n series
    /// ================================================================================================================

    /**
     * @brief unchecked_copy_n 调用不同重载
     * */

    template<typename InputIter, typename Size, typename OutputIter>
    mystl::pair<InputIter, OutputIter>
    unchecked_copy_n(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag)
    {
        for (; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return mystl::pair<InputIter, OutputIter>(first, result);
    }

    template<typename RandomIter, typename Size, typename OutputIter>
    mystl::pair<RandomIter, OutputIter>
    unchecked_copy_n(RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag)
    {
        auto last = first + n;
        return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
    }

    template<typename InputIter, typename Size, typename OutputIter>
    mystl::pair<InputIter, OutputIter> copy_n(InputIter first, Size n, OutputIter result)
    {

    }

}

#endif //MYSTL_ALGOBASE_H
