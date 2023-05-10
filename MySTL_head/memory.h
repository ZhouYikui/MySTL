//
// Created by HP on 2023/5/10.
//

/**
 * @file memory.h
 * @brief 负责更高级的动态内存管理
 *
 * @date 2023年5月10日
 * @author ZYK
 */

#ifndef MYSTL_MEMORY_H
#define MYSTL_MEMORY_H

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief 获取对象地址
    /// ================================================================================================================

    template<typename Tp>
    constexpr Tp *address_of(Tp &value) noexcept
    {
        return &value;
    }

}

#endif //MYSTL_MEMORY_H
