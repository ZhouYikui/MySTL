//
// Created by HP on 2023/5/12.
//

/**
 * @file list.h
 * @brief 实现模板类list
 *
 * @date 2023年5月12日
 * @author ZYK
 */

#ifndef MYSTL_LIST_H
#define MYSTL_LIST_H

#include "util.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief list 节点结构
    /// ================================================================================================================
    template<typename T>
    struct list_node_base;

    template<typename T>
    struct list_node;

    template<typename T>
    struct node_traits
    {
        typedef list_node_base<T> *base_ptr;
        typedef list_node<T> *node_ptr;
    };

    template<typename T>
    struct list_node_base
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        base_ptr prev;  // 前一节点
        base_ptr next;  // 下一节点

        list_node_base() = default;

        node_ptr as_node()
        {
            return static_cast<node_ptr>(self());
        }

        void unlink()
        {
            prev = next = self();
        }

        base_ptr self()
        {
            return static_cast<base_ptr>(&*this);
        }
    };

    template<typename T>
    struct list_node : public list_node_base<T>
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        T value;  // 数据域

        list_node() = default;

        list_node(const T &v) : value(v) {}

        list_node(T &&v) : value(mystl::move(v)) {}

        base_ptr as_base()
        {
            return static_cast<base_ptr>(&*this);
        }

        node_ptr self()
        {
            return static_cast<node_ptr>(&*this);
        }
    };

}

#endif //MYSTL_LIST_H
