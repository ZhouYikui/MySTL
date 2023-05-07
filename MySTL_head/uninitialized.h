//
// Created by HP on 2023/4/22.
//

/**
 * @file uninitialized.h
 * @brief 对未初始化空间构造元素
 *
 * @date 2023年4月22日
 * @author ZYK
 */

#ifndef MYSTL_UNINITIALIZED_H
#define MYSTL_UNINITIALIZED_H

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief uninitialized_fill
    /// ================================================================================================================

    template<typename ForwardIter, typename T>
    void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T &value, std::true_type)
    {

    }

    template<typename ForwardIter, typename T>
    void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T &value, std::false_type)
    {
        auto cur = first;
        try
        {
            for (; cur != last; ++cur)
            {
                mystl::construct(&*cur, value);
            }
        }
        catch (...)
        {
            for (; first != cur; ++first)
            {
                mystl::destroy(&*first);
            }
        }
    }

    template<typename ForwardIter, typename T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T &value)
    {
        unchecked_uninit_fill(first, last, value,
                              std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
    }

    /// ================================================================================================================
    /// @brief uninitialized_fill_n
    /// ================================================================================================================

    /// @brief 根据是否符合平凡copy选择不同的函数
    /// @param[in] std::true_type 平凡copy赋值时（不需要构造函数）

    template<typename ForwardIter, typename Size, typename T>
    ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value, std::true_type)
    {
        return mystl::fill_n(first, n, value);
    }

    template<typename ForwardIter, typename Size, typename T>
    ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value, std::false_type)
    {
        auto cur = first;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                mystl::construct(&*cur, value);
            }
        }
        catch (...)
        {
            for (; first != cur; ++first)
            {
                mystl::destroy(&*first);
            }
        }
        return cur;
    }

    template<typename ForwardIter, typename Size, typename T>
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T &value)
    {
        return mystl::unchecked_uninit_fill_n(first, n, value,
                                              std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
    }

    /// ================================================================================================================
    /// @brief uninitialized_copy
    /// ================================================================================================================

    template<typename InputIter, typename ForwardIter>
    ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
    {
        return mystl::copy(first, last, result);
    }

    template<typename InputIter, typename ForwardIter>
    ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; first != last; ++first, ++cur)
            {
                mystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; cur != result; --cur)
            {
                mystl::destroy(&*cur);
            }
        }
        return cur;
    }

    /**
     * @brief uninitialized_copy
     * @brief 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * */

    template<typename InputIter, typename ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy(first, last, result,
                                            std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
    }

    /// ================================================================================================================
    /// @brief uninitialized_copy_n
    /// ================================================================================================================

    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return mystl::copy_n(first, n, result).second;
    }

    template<class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++cur, ++first)
            {
                mystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; --cur)
            {
                mystl::destroy(&*cur);
            }
        }
        return cur;
    }

    /**
     * @brief uninitialized_copy_n
     * @brief 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
     * */

    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy_n(first, n, result,
                                              std::is_trivially_copy_assignable<typename iterator_traits<InputIter>::value_type>{});
    }

}

#endif //MYSTL_UNINITIALIZED_H
