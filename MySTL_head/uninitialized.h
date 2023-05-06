//
// Created by HP on 2023/4/22.
//

#ifndef MYSTL_UNINITIALIZED_H
#define MYSTL_UNINITIALIZED_H

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl
{
    /**
     * @brief T 和 U 都是整数类型且大小为1字节，且T不是bool类型
     * @brief enable_if 在编译时确定函数模板是否有效
     * @brief memset
     * */
    template<typename T, typename Size, typename U>
    typename std::enable_if<std::is_integral<T>::value && sizeof(T) == 1 && !std::is_same<T, bool>::value &&
                            std::is_integral<U>::value && sizeof(U) == 1, T *>::type
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

    /**
     * @brief 根据是否符合平凡copy选择不同的函数
     * @param[in] std::true_type 平凡时
     * */

    template<typename ForwardIter, typename Size, typename T>
    ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value, std::true_type)
    {
        return mystl::fill_n(first, n, value);
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
                mystl::destroy(&*cur);
        }
        return cur;
    }

    template<typename InputIter, typename Size, typename ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
    {
        return mystl::unchecked_uninit_copy_n(first, n, result,
                                              std::is_trivially_copy_assignable<typename iterator_traits<InputIter>::value_type>{});
    }

}

#endif //MYSTL_UNINITIALIZED_H
