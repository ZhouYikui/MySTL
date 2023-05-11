//
// Created by HP on 2023/5/11.
//

/**
 * @file algo.h
 * @brief 实现algo一系列算法
 *
 * @date 2023年5月11日
 * @author ZYK
 */

#ifndef MYSTL_ALGO_H
#define MYSTL_ALGO_H

#include "iterator.h"
#include "algobase.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief reverse
    /// ================================================================================================================

    /// @brief reverse_dispatch 的 bidirectional_iterator_tag 版本
    template<class BidirectionalIter>
    void reverse_dispatch(BidirectionalIter first, BidirectionalIter last, bidirectional_iterator_tag)
    {
        while (true)
        {
            if (first == last || first == --last) return;
            mystl::iter_swap(first++, last);
        }
    }

    /// @brief reverse_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter>
    void reverse_dispatch(RandomIter first, RandomIter last,
                          random_access_iterator_tag)
    {
        while (first < last)
        {
            mystl::iter_swap(first++, --last);
        }
    }

    template<typename BidirectionalIter>
    void reverse(BidirectionalIter first, BidirectionalIter last)
    {
        mystl::reverse_dispatch(first, last, iterator_category(first));
    }
}

#endif //MYSTL_ALGO_H
