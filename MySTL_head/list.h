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

#include <initializer_list>
#include "util.h"
#include "iterator.h"
#include "allocator.h"
#include "memory.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief list 节点结构
    /// ================================================================================================================
    template<typename T>
    struct list_node_base;

    template<typename T>
    struct list_node;

    /// @brief 定义了两个别名，分别是指向 list_node_bast<T> 和 list_node<T> 的指针
    template<typename T>
    struct node_traits
    {
        typedef list_node_base<T> *base_ptr;
        typedef list_node<T> *node_ptr;
    };

    /// @brief 链表节点的基类，提供了链表节点的常见操作
    template<typename T>
    struct list_node_base
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        base_ptr prev;  // 前一节点
        base_ptr next;  // 下一节点

        list_node_base() = default;

        /// @brief 返回派生类型节点指针
        node_ptr as_node()
        {
            return static_cast<node_ptr>(self());
        }

        /// @brief 用于删除节点
        void unlink()
        {
            prev = next = self();
        }

        /// @brief 为了保证as_node正常工作，需要将派生类指针转为基类指针
        base_ptr self()
        {
            return static_cast<base_ptr>(&*this);
        }
    };

    /// 链表节点的派生类，增加了value
    template<typename T>
    struct list_node : public list_node_base<T>
    {
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        T value;

        list_node() = default;

        list_node(const T &v) : value(v) {}

        list_node(T &&v) : value(mystl::move(v)) {}

        /// @brief 返回基类指针
        base_ptr as_base()
        {
            return static_cast<base_ptr>(&*this);
        }

        node_ptr self()
        {
            return static_cast<node_ptr>(&*this);
        }
    };

    /// ================================================================================================================
    /// @brief list 迭代器 继承自iterator
    /// ================================================================================================================

    template<typename T>
    struct list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T>
    {
        /// 首先定义了六个别名
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_iterator<T> self;

        /// node_ 指向当前节点的base指针
        base_ptr node_;

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 构造函数
        /// ------------------------------------------------------------------------------------------------------------
        list_iterator() = default;

        explicit list_iterator(base_ptr x) : node_(x) {}

        explicit list_iterator(node_ptr x) : node_(x->as_base()) {}

        list_iterator(const list_iterator &lhs) : node_(lhs.node_) {}

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 操作符重载
        /// ------------------------------------------------------------------------------------------------------------
        reference operator*() const
        {
            return node_->as_node()->value;
        }

        /// 返回指向value的指针
        pointer operator->() const
        {
            return &(operator*());
        }

        self &operator++()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        // 重载比较操作符
        bool operator==(const self &lhs) const
        {
            return node_ == lhs.node_;
        }

        bool operator!=(const self &lhs) const
        {
            return node_ != lhs.node_;
        }
    };

    /// ================================================================================================================
    /// @brief const list 迭代器 继承自iterator
    /// ================================================================================================================

    template<typename T>
    struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>
    {
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;
        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;
        typedef list_const_iterator<T> self;

        /// value
        base_ptr node_;

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 构造函数
        /// ------------------------------------------------------------------------------------------------------------

        list_const_iterator() = default;

        explicit list_const_iterator(base_ptr x) : node_(x) {}

        explicit list_const_iterator(node_ptr x) : node_(x->as_base()) {}

        explicit list_const_iterator(const list_iterator<T> &lhs) : node_(lhs.node_) {}

        list_const_iterator(const list_const_iterator &lhs) : node_(lhs.node_) {}

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 操作符重载
        /// ------------------------------------------------------------------------------------------------------------

        reference operator*() const
        {
            return node_->as_node()->value;
        }

        pointer operator->() const
        {
            return &(operator*());
        }

        self &operator++()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->next;
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--()
        {
            MYSTL_DEBUG(node_ != nullptr);
            node_ = node_->prev;
            return *this;
        }

        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const self &rhs) const
        {
            return node_ == rhs.node_;
        }

        bool operator!=(const self &rhs) const
        {
            return node_ != rhs.node_;
        }
    };

    /// ================================================================================================================
    /// @brief 模板类 list
    /// ================================================================================================================
    template<typename T>
    class list
    {
    public:
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;
        typedef mystl::allocator<list_node_base<T>> base_allocator;
        typedef mystl::allocator<list_node<T>> node_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef list_iterator<T> iterator;
        typedef list_const_iterator<T> const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

        typedef typename node_traits<T>::base_ptr base_ptr;
        typedef typename node_traits<T>::node_ptr node_ptr;

        allocator_type get_allocator() { return node_allocator(); }

    private:
        // 指向末尾节点
        base_ptr node_;
        size_type size_;

    public:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 构造函数
        /// ------------------------------------------------------------------------------------------------------------
        list()
        {
            fill_init(0, value_type());
        }

        explicit list(size_type n)
        {
            fill_init(n, value_type());
        }

        list(size_type n, const T &value)
        {
            fill_init(n, value);
        }

        template<class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
        list(Iter first, Iter last)
        {
            copy_init(first, last);
        }

        list(std::initializer_list<T> ilist)
        {
            copy_init(ilist.begin(), ilist.end());
        }

        list(const list &lhs)
        {
            copy_init(lhs.cbegin(), lhs.cend());
        }

        list(list &&rhs) noexcept: node_(rhs.node_), size_(rhs.size_)
        {
            rhs.node_ = nullptr;
            rhs.size_ = 0;
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 操作符重载
        /// ------------------------------------------------------------------------------------------------------------

        list &operator=(const list &lhs)
        {
            if (this != &lhs)
            {
                assign(lhs.begin(), lhs.end());
            }
            return *this;
        }

    public:

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 迭代器相关操作
        /// ------------------------------------------------------------------------------------------------------------

        iterator begin() noexcept
        {
            return node_->next;
        }

        const_iterator begin() const noexcept
        {
            return node_->next;
        }

        iterator end() noexcept
        {
            return node_;
        }

        const_iterator end() const noexcept
        {
            return node_;
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 容器相关操作
        /// ------------------------------------------------------------------------------------------------------------

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief assign
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void assign(size_type n, const value_type &value)
        {
            fill_assign(n, value);
        }

        template<class Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            copy_assign(first, last);
        }

        void assign(std::initializer_list<T> ilist)
        {
            copy_assign(ilist.begin(), ilist.end());
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief erase/clear
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void clear();


    private:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief helper function
        /// ------------------------------------------------------------------------------------------------------------

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief create/destroy node
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        template<typename ...Args>
        node_ptr create_node(Args &&...args);

        void destroy_node(node_ptr p);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief 初始化/回收函数
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void fill_init(size_type n, const value_type &value);

        template<typename Iter>
        void copy_init(Iter first, Iter last);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief link/unlink
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void link_nodes_at_back(base_ptr first, base_ptr last);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief assign
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void fill_assign(size_type n, const value_type &value);

        template<typename Iter>
        void copy_assign(Iter first, Iter last);

    };

    /// ================================================================================================================
    /// @brief 容器相关操作
    /// ================================================================================================================

    /// @brief clear, 用于清空链表

    template<typename T>
    void list<T>::clear()
    {
        if (size_ != 0)
        {
            auto cur = node_->next;
            for (base_ptr next = cur->next; cur != node_; cur = next, next = cur->next)
            {
                destroy_node(cur->as_node());
            }
            node_->unlink();
            size_ = 0;
        }
    }

    /// ================================================================================================================
    /// @brief helper function
    /// ================================================================================================================

    /// @brief creator_node

    template<typename T>
    template<typename ...Args>
    typename list<T>::node_ptr list<T>::create_node(Args &&...args)
    {
        node_ptr p = node_allocator::allocate(1);
        try
        {
            data_allocator::construct(mystl::address_of(p->value), mystl::forward<Args>(args)...);
            p->prev = nullptr;
            p->next = nullptr;
        }
        catch (...)
        {
            node_allocator::deallocate(p);
            throw;
        }
        return p;
    }

    /// @brief destroy_node

    template<typename T>
    void list<T>::destroy_node(node_ptr p)
    {
        data_allocator::destroy(mystl::address_of(p->value));
        node_allocator::deallocate(p);
    }

    /// @brief fill init

    template<typename T>
    void list<T>::fill_init(size_type n, const value_type &value)
    {
        node_ = base_allocator::allocate(1);
        node_->unlink();
        size_ = n;
        try
        {
            for (; n > 0; --n)
            {
                auto node = create_node(value);
                link_nodes_at_back(node->as_base(), node->as_base());
            }
        }
        catch (...)
        {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

    /// @brief copy init

    template<typename T>
    template<typename Iter>
    void list<T>::copy_init(Iter first, Iter last)
    {
        node_ = base_allocator::allocate(1);
        node_->unlink();
        size_type n = mystl::distance(first, last);
        size_ = n;
        try
        {
            for (; n > 0; --n, ++first)
            {
                auto node = create_node(*first);
                link_nodes_at_back(node->as_base(), node->as_base());
            }
        }
        catch (...)
        {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            throw;
        }
    }

    /// @brief link_node_at_back，将一段插入节点到末尾， node_prev是最后一个有效节点

    template<typename T>
    void list<T>::link_nodes_at_back(base_ptr first, base_ptr last)
    {
        last->next = node_;
        first->prev = node_->prev;
        first->prev->next = first;
        node_->prev = last;
    }

    /// @brief fill_assign，用n个value为容器赋值

    template<typename T>
    void list<T>::fill_assign(size_type n, const value_type &value)
    {
        auto b = begin();
        auto e = end();
    }

}

#endif //MYSTL_LIST_H
