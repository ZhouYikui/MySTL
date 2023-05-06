//
// Created by Zyk on 2023/5/1.
//

/**
 * @file exceptdef.h
 * @brief 用于提取类型信息
 *
 * @date 2023年5月1日
 * @author ZYK
 */

#ifndef MYSTL_EXCEPTDEF_H
#define MYSTL_EXCEPTDEF_H

#include <cassert>
#include <stdexcept>

namespace mystl
{
#define MYSTL_DEBUG(expr) assert(expr)
#define THROW_LENGTH_ERROR_IF(expr, what) if ((expr)) throw std::length_error(what)
#define THROW_OUT_OF_RANGE_IF(expr, what) if ((expr)) throw std::out_of_range(what)
#define THROW_RUNTIME_ERROE_IF(expr, what) if ((expr)) throw std::runtime_error(what)
}

#endif //MYSTL_EXCEPTDEF_H
