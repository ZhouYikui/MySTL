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
    /// @brief iter_swap函数
    /// ================================================================================================================

    template<typename FIter1, typename FIter2>
    void iter_swap(FIter1 lhs, FIter2 rhs)
    {
        mystl::swap(*lhs, *rhs);
    }

    /// ================================================================================================================
    /// @brief fill series
    /// ================================================================================================================

    template<typename ForwardIter, typename T>
    void fill_cat(ForwardIter first, ForwardIter last, const T &value, mystl::forward_iterator_tag)
    {
        for (; first != last; ++first)
        {
            *first = value;
        }
    }

    template<typename ForwardIter, typename T>
    void fill_cat(ForwardIter first, ForwardIter last, const T &value, mystl::random_access_iterator_tag)
    {
        fill_n(first, last - first, value);
    }

    template<typename ForwardIter, typename T>
    void fill(ForwardIter first, ForwardIter last, const T &value)
    {
        fill_cat(first, last, value, iterator_category(first));
    }

    /// ================================================================================================================
    /// @brief fill_n series
    /// ================================================================================================================

    template<typename OutputIter, typename Size, typename T>
    OutputIter unchecked_fill_n(OutputIter first, Size n, const T &value)
    {
        for (; n > 0; --n, ++first)
        {
            *first = value;
        }
        return first;
    }

    /// @brief 当 T 和 U 都是整数类型且大小为1字节，且T不是bool类型时，调用特化版本（直接设置内存）
    /// @brief void *memset(void *s, int c, size_t n);

    template<typename T, typename Size, typename U>
    typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 1 &&
                            std::is_integral<U>::value && sizeof(U) == 1 &&
                            !std::is_same<T, bool>::value, T *>::type
    unchecked_fill_n(T *first, Size n, U value)
    {
        if (n > 0)
        {
            /// 转化成unsigned char 防止符号溢出
            std::memset(first, (unsigned char) value, (size_t) (n));
        }
        return first + n;
    }

    template<typename OutputIter, typename Size, typename T>
    OutputIter fill_n(OutputIter first, Size n, const T &value)
    {
        return unchecked_fill_n(first, n, value);
    }

    /// ================================================================================================================
    /// @brief copy series
    /// ================================================================================================================

    /// @brief copy辅助函数，根据不同的迭代器类型选择不同的处理方式

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

    /// @brief unchecked_copy 不同情况下调用不同重载
    /// @note void* memmove ( void* dest, const void* src, std::size_t count );

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

    /// @brief unchecked_copy_n 调用不同重载
    /// @param input_iterator_tag 逐个复制
    /// @param random_access_iterator_tag 一次性复制
    /// @return 返回一个pair 指向复制的尾部

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
        unchecked_copy_n(first, n, result, iterator_category(first));
    }

    /// ================================================================================================================
    /// @brief copy_backward series
    /// ================================================================================================================

    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2 unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                                   BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while (first != last)
        {
            *--result = *--last;
        }
        return result;
    }

    template<typename RandomIter1, typename BidirectionalIter2>
    BidirectionalIter2 unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last,
                                                   BidirectionalIter2 result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = *--last;
        return result;
    }

    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
    }

    template<typename Tp, typename Up>
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_copy_assignable<Up>::value, Up *>::type
    unchecked_copy_backward(Tp *first, Tp *last, Up *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward(first, last, result);
    }

    /// ================================================================================================================
    /// @brief move series
    /// ================================================================================================================

    template<typename InputIter, typename OutputIter>
    OutputIter unchecked_move_cat(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            *result = mystl::move(*first);
        }
        return result;
    }

    template<typename RandomIter, typename OutputIter>
    OutputIter
    unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = mystl::move(*first);
        }
        return result;
    }

    template<typename InputIter, typename OutputIter>
    OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }

    template<typename Tp, typename Up>
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_move_assignable<Up>::value, Up *>::type
    unchecked_move(Tp *first, Tp *last, Up *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0) std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template<typename InputIter, typename OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move(first, last, result);
    }

    /// ================================================================================================================
    /// @brief move_backward series
    /// ================================================================================================================

    /// @brief bidirectional_iterator_tag 版本
    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2 unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                                   BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while (first != last)
            *--result = mystl::move(*--last);
        return result;
    }

    /// @brief random_access_iterator_tag 版本
    template<typename RandomIter1, typename RandomIter2>
    RandomIter2
    unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
                                RandomIter2 result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = mystl::move(*--last);
        return result;
    }

    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2
    unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_move_backward_cat(first, last, result, iterator_category(first));
    }

    template<typename Tp, typename Up>
    typename std::enable_if<std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
                            std::is_trivially_move_assignable<Up>::value, Up *>::type
    unchecked_move_backward(Tp *first, Tp *last, Up *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template<typename BidirectionalIter1, typename BidirectionalIter2>
    BidirectionalIter2 move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_move_backward(first, last, result);
    }

    /// ================================================================================================================
    /// @brief equal
    /// ================================================================================================================
    template<typename InputIter1, typename InputIter2>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (*first1 != *first2)
                return false;
        }
        return true;
    }

    template<typename InputIter1, typename InputIter2, typename Compared>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!comp(*first1, *first2))
                return false;
        }
        return true;
    }

    /// ================================================================================================================
    /// @brief lexicographical_compare
    /// ================================================================================================================

    // 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
    // (1)如果第一序列的元素较小，返回 true ，否则返回 false
    // (2)如果到达 last1 而尚未到达 last2 返回 true
    // (3)如果到达 last2 而尚未到达 last1 返回 false
    // (4)如果同时到达 last1 和 last2 返回 false

    template<typename InputIter1, typename InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (*first1 < *first2) return true;
            if (*first2 < *first1) return false;
        }
        return first1 == last1 && first2 != last2;
    }

    template<typename InputIter1, typename InputIter2, typename Compred>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compred comp)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (comp(*first1, *first2)) return true;
            if (comp(*first2, *first1)) return false;
        }
        return first1 == last1 && first2 != last2;
    }

    bool lexicographical_compare(const unsigned char *first1, const unsigned char *last1,
                                 const unsigned char *first2, const unsigned char *last2)
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        // 先比较相同长度的部分
        const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));
        // 若相等，长度较长的比较大
        return result != 0 ? result < 0 : len1 < len2;
    }

}

#endif //MYSTL_ALGOBASE_H
