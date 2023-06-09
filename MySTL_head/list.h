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
#include "exceptdef.h"
#include "functional.h"

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

        list &operator=(list &&rhs) noexcept
        {
            clear();
            splice(end(), rhs);
            return *this;
        }

        list &operator=(std::initializer_list<T> ilist)
        {
            list tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }

        ~list()
        {
            if (node_)
            {
                clear();
                base_allocator::deallocate(node_);
                node_ = nullptr;
                size_ = 0;
            }
        }

    public:

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 迭代器相关操作
        /// ------------------------------------------------------------------------------------------------------------

        iterator begin() noexcept { return node_->next; }

        const_iterator begin() const noexcept { return node_->next; }

        iterator end() noexcept { return node_; }

        const_iterator end() const noexcept { return node_; }

        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept { return begin(); }

        const_iterator cend() const noexcept { return end(); }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }

        const_reverse_iterator crend() const noexcept { return rend(); }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 容量相关操作
        /// ------------------------------------------------------------------------------------------------------------

        bool empty() const noexcept
        {
            return node_->next == node_;
        }

        size_type size() const noexcept
        {
            return size_;
        }

        size_type max_size() const noexcept
        {
            return static_cast<size_type>(-1);
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 访问元素相关操作
        /// ------------------------------------------------------------------------------------------------------------

        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }

        const_reference front() const
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }

        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(--end());
        }

        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(--end());
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

        template<typename Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            copy_assign(first, last);
        }

        void assign(std::initializer_list<T> ilist)
        {
            copy_assign(ilist.begin(), ilist.end());
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief emplace_front / emplace_back / emplace
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        template<typename ...Args>
        void emplace_front(Args &&...args)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(mystl::forward<Args>(args)...);
            link_nodes_at_front(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        template<typename ...Args>
        void emplace_back(Args &&...args)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(mystl::forward<Args>(args)...);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        template<typename ...Args>
        iterator emplace(const_iterator pos, Args &&...args)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(mystl::forward<Args>(args)...);
            link_nodes(pos.node_, link_node->as_base(), link_node->as_base());
            ++size_;
            return iterator(link_node);
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief intsert 区分左值右值
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        iterator insert(const_iterator pos, const value_type &value)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(value);
            ++size_;
            return link_iter_node(pos, link_node->as_base());
        }

        iterator insert(const_iterator pos, value_type &&value)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(mystl::move(value));
            ++size_;
            return link_iter_node(pos, link_node->as_base());
        }

        iterator insert(const_iterator pos, size_type n, const value_type &value)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            return fill_insert(pos, n, value);
        }

        template<typename Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
        iterator insert(const_iterator pos, Iter first, Iter last)
        {
            size_type n = mystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            return copy_insert(pos, n, first);
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief push_front/push_back
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void push_front(const value_type &value)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(value);
            link_nodes_at_front(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        void push_front(value_type &&value)
        {
            emplace_front(mystl::move(value));
        }

        void push_back(const value_type &value)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
            auto link_node = create_node(value);
            link_nodes_at_back(link_node->as_base(), link_node->as_base());
            ++size_;
        }

        void push_back(value_type &&value)
        {
            emplace_back(mystl::move(value));
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief pop_front/pop_back
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void pop_front()
        {
            MYSTL_DEBUG(!empty());
            auto n = node_->next;
            unlink_nodes(n, n);
            destroy_node(n->as_node());
            --size_;
        }

        void pop_back()
        {
            MYSTL_DEBUG(!empty());
            auto n = node_->prev;
            unlink_nodes(n, n);
            destroy_node(n->as_node());
            --size_;
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief erase/clear
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator last);

        void clear();

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief resize
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void resize(size_type new_size)
        {
            resize(new_size, value_type());
        }

        void resize(size_type new_size, const value_type &value);

        void swap(list &rhs) noexcept
        {
            mystl::swap(node_, rhs.node_);
            mystl::swap(size_, rhs.size_);
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief list相关操作
        /// ------------------------------------------------------------------------------------------------------------

        void splice(const_iterator pos, list &other);

        void splice(const_iterator pos, list &other, const_iterator it);

        void splice(const_iterator pos, list &other, const_iterator first, const_iterator last);

        void remove(const value_type &value)
        {
            remove_if([&](const value_type &v)
                      {
                          return v == value;
                      });
        }

        template<class UnaryPredicate>
        void remove_if(UnaryPredicate pred);

        void unique() { unique(mystl::equal_to<T>()); }

        template<class BinaryPredicate>
        void unique(BinaryPredicate pred);

        void merge(list &x) { merge(x, mystl::less<T>()); }

        template<class Compare>
        void merge(list &x, Compare comp);

        void sort() { list_sort(begin(), end(), size(), mystl::less<T>()); }

        template<class Compared>
        void sort(Compared comp) { list_sort(begin(), end(), size(), comp); }

        void reverse();

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

        iterator link_iter_node(const_iterator pos, base_ptr link_node);

        void link_nodes(base_ptr pos, base_ptr first, base_ptr last);

        void link_nodes_at_front(base_ptr first, base_ptr last);

        void link_nodes_at_back(base_ptr first, base_ptr last);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief assign
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void fill_assign(size_type n, const value_type &value);

        template<typename Iter>
        void copy_assign(Iter first, Iter last);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief insert
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        iterator fill_insert(const_iterator pos, size_type n, const value_type &value);

        template<class Iter>
        iterator copy_insert(const_iterator pos, size_type n, Iter first);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief sort
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        template<class Compared>
        iterator list_sort(iterator first, iterator last, size_type n, Compared comp);

    };

    /// ================================================================================================================
    /// @brief 容器相关操作
    /// ================================================================================================================

    /// @brief erase

    template<typename T>
    typename list<T>::iterator list<T>::erase(const_iterator pos)
    {
        MYSTL_DEBUG(pos != cend());
        auto n = pos.node_;
        auto next = n->next;
        unlink_nodes(n, n);
        destroy_node(n->as_node());
        --size_;
        return iterator(next);
    }

    template<typename T>
    typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last)
    {
        if (first != last)
        {
            unlink_nodes(first.node_, last.node_->prev);
            while (first != last)
            {
                auto cur = first.node_;
                ++first;
                destroy_node(cur->as_node());
                --size_;
            }
        }
        return iterator(last.node_);
    }


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

    /// @brief resize

    template<typename T>
    void list<T>::resize(size_type new_size, const value_type &value)
    {
        auto i = begin();
        size_type len = 0;
        while (i != end() && len < new_size)
        {
            ++i;
            ++len;
        }
        if (len == new_size)
        {
            erase(i, node_);
        }
        else
        {
            insert(node_, new_size - len, value);
        }
    }

    /// ================================================================================================================
    /// @brief list相关操作
    /// ================================================================================================================

    template<typename T>
    void list<T>::splice(const_iterator pos, list &x)
    {
        MYSTL_DEBUG(this != &x);
        if (!x.empty())
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

            auto f = x.node_->next;
            auto l = x.node_->prev;

            x.unlink_nodes(f, l);
            link_nodes(pos.node_, f, l);

            size_ += x.size_;
            x.size_ = 0;
        }
    }

    template<typename T>
    void list<T>::splice(const_iterator pos, list &x, const_iterator it)
    {
        if (pos.node_ != it.node_ && pos.node_ != it.node_->next)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");

            auto f = it.node_;

            x.unlink_nodes(f, f);
            link_nodes(pos.node_, f, f);

            ++size_;
            --x.size_;
        }
    }

    template<typename T>
    void list<T>::splice(const_iterator pos, list &x, const_iterator first, const_iterator last)
    {
        if (first != last && this != &x)
        {
            size_type n = mystl::distance(first, last);
            THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
            auto f = first.node_;
            auto l = last.node_->prev;

            x.unlink_nodes(f, l);
            link_nodes(pos.node_, f, l);

            size_ += n;
            x.size_ -= n;
        }
    }

    template<typename T>
    template<typename UnaryPredicate>
    void list<T>::remove_if(UnaryPredicate pred)
    {
        auto f = begin();
        auto l = end();
        for (auto next = f; f != l; f = next)
        {
            ++next;
            if (pred(*f))
            {
                erase(f);
            }
        }
    }

    template<typename T>
    template<typename BinaryPredicate>
    void list<T>::unique(BinaryPredicate pred)
    {
        auto i = begin();
        auto e = end();
        auto j = i;
        ++j;
        while (j != e)
        {
            if (pred(*i, *j))
            {
                erase(j);
            }
            else
            {
                i = j;
            }
            j = i;
            ++j;
        }
    }

    template<typename T>
    template<typename Compare>
    void list<T>::merge(list &x, Compare comp)
    {
        if (this != &x)
        {
            THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "list<T>'s size too big");

            auto f1 = begin();
            auto l1 = end();
            auto f2 = x.begin();
            auto l2 = x.end();

            while (f1 != l1 && f2 != l2)
            {
                if (comp(*f2, *f1))
                {
                    // 使 comp 为 true 的一段区间
                    auto next = f2;
                    ++next;
                    for (; next != l2 && comp(*next, *f1); ++next);
                    auto f = f2.node_;
                    auto l = next.node_->prev;
                    f2 = next;

                    // link node
                    x.unlink_nodes(f, l);
                    link_nodes(f1.node_, f, l);
                    ++f1;
                }
                else
                {
                    ++f1;
                }
            }
            // 连接剩余部分
            if (f2 != l2)
            {
                auto f = f2.node_;
                auto l = l2.node_->prev;
                x.unlink_nodes(f, l);
                link_nodes(l1.node_, f, l);
            }

            size_ += x.size_;
            x.size_ = 0;
        }
    }

    template<typename T>
    void list<T>::reverse()
    {
        if (size_ <= 1)
        {
            return;
        }
        auto i = begin();
        auto e = end();
        while (i.node_ != e.node_)
        {
            mystl::swap(i.node_->prev, i.node_->next);
            i.node_ = i.node_->prev;
        }
        mystl::swap(e.node_->prev, e.node_->next);
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

    /// @brief link_iter_node

    template<typename T>
    typename list<T>::iterator list<T>::link_iter_node(const_iterator pos, base_ptr link_node)
    {
        if (pos == node_->next)
        {
            link_nodes_at_front(link_node, link_node);
        }
        else if (pos == node_)
        {
            link_nodes_at_back(link_node, link_node);
        }
        else
        {
            link_nodes(pos.node_, link_node, link_node);
        }
        return iterator(link_node);
    }

    /// @brief link_nodes

    template<typename T>
    void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last)
    {
        pos->prev->next = first;
        first->prev = pos->pre;
        pos->prev = last;
        last->next = pos;
    }

    /// @brief link_node_at_front

    template<typename T>
    void list<T>::link_nodes_at_front(base_ptr first, base_ptr last)
    {
        first->prev = node_;
        last->next = node_->next;
        node_->next->prev = last;
        node_->next = first;
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
        for (; n > 0 && b != e; --n, ++b)
        {
            *b = value;
        }
        if (n > 0)
        {
            insert(e, n, value);
        }
        else
        {
            erase(b, e);
        }
    }

    /// @brief copy_assign

    template<typename T>
    template<typename Iter>
    void list<T>::copy_assign(Iter f2, Iter l2)
    {
        auto f1 = begin();
        auto l1 = end();
        for (; f1 != l1 && f2 != l2; ++f1, ++f2)
        {
            *f1 = *f2;
        }
        if (f2 == l2)
        {
            erase(f1, l1);
        }
        else
        {
            insert(l1, f2, l2);
        }
    }

    template<typename T>
    typename list<T>::iterator list<T>::fill_insert(const_iterator pos, size_type n, const value_type &value)
    {
        iterator r(pos.node_);
        if (n != 0)
        {
            const auto add_size = n;
            auto node = create_node(value);
            node->prev = nullptr;
            r = iterator(node);
            iterator end = r;
            try
            {
                // 前面已经创建了一个节点，还需 n - 1 个
                for (--n; n > 0; --n, ++end)
                {
                    auto next = create_node(value);
                    end.node_->next = next->as_base();  // link node
                    next->prev = end.node_;
                }
                size_ += add_size;
            }
            catch (...)
            {
                auto enode = end.node_;
                while (true)
                {
                    auto prev = enode->prev;
                    destroy_node(enode->as_node());
                    if (prev == nullptr)
                        break;
                    enode = prev;
                }
                throw;
            }
            link_nodes(pos.node_, r.node_, end.node_);
        }
        return r;
    }

    template<typename T>
    template<typename Iter>
    typename list<T>::iterator list<T>::copy_insert(const_iterator pos, size_type n, Iter first)
    {
        iterator r(pos.node_);
        if (n != 0)
        {
            const auto add_size = n;
            auto node = create_node(*first);
            node->prev = nullptr;
            r = iterator(node);
            iterator end = r;
            try
            {
                for (--n, ++first; n > 0; --n, ++first, ++end)
                {
                    auto next = create_node(*first);
                    end.node_->next = next->as_base();  // link node
                    next->prev = end.node_;
                }
                size_ += add_size;
            }
            catch (...)
            {
                auto enode = end.node_;
                while (true)
                {
                    auto prev = enode->prev;
                    destroy_node(enode->as_node());
                    if (prev == nullptr)
                        break;
                    enode = prev;
                }
                throw;
            }
            link_nodes(pos.node_, r.node_, end.node_);
        }
        return r;
    }

    template<typename T>
    template<typename Compared>
    typename list<T>::iterator list<T>::list_sort(iterator f1, iterator l2, size_type n, Compared comp)
    {
        if (n < 2)
            return f1;

        if (n == 2)
        {
            if (comp(*--l2, *f1))
            {
                auto ln = l2.node_;
                unlink_nodes(ln, ln);
                link_nodes(f1.node_, ln, ln);
                return l2;
            }
            return f1;
        }

        auto n2 = n / 2;
        auto l1 = f1;
        mystl::advance(l1, n2);
        auto result = f1 = list_sort(f1, l1, n2, comp);  // 前半段的最小位置
        auto f2 = l1 = list_sort(l1, l2, n - n2, comp);  // 后半段的最小位置

        // 把较小的一段区间移到前面
        if (comp(*f2, *f1))
        {
            auto m = f2;
            ++m;
            for (; m != l2 && comp(*m, *f1); ++m);
            auto f = f2.node_;
            auto l = m.node_->prev;
            result = f2;
            l1 = f2 = m;
            unlink_nodes(f, l);
            m = f1;
            ++m;
            link_nodes(f1.node_, f, l);
            f1 = m;
        }
        else
        {
            ++f1;
        }

        // 合并两段有序区间
        while (f1 != l1 && f2 != l2)
        {
            if (comp(*f2, *f1))
            {
                auto m = f2;
                ++m;
                for (; m != l2 && comp(*m, *f1); ++m);
                auto f = f2.node_;
                auto l = m.node_->prev;
                if (l1 == f2)
                    l1 = m;
                f2 = m;
                unlink_nodes(f, l);
                m = f1;
                ++m;
                link_nodes(f1.node_, f, l);
                f1 = m;
            }
            else
            {
                ++f1;
            }
        }
        return result;
    }

    /// ================================================================================================================
    /// @brief 重载操作符
    /// ================================================================================================================

    template<typename T>
    bool operator==(const list<T> &lhs, const list<T> &rhs)
    {
        auto f1 = lhs.cbegin();
        auto f2 = rhs.cbegin();
        auto l1 = lhs.cend();
        auto l2 = rhs.cend();
        for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2);
        return f1 == l1 && f2 == l2;
    }

    template<typename T>
    bool operator<(const list<T> &lhs, const list<T> &rhs)
    {
        return mystl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T>
    bool operator!=(const list<T> &lhs, const list<T> &rhs)
    {
        return !(lhs == rhs);
    }

    template<typename T>
    bool operator>(const list<T> &lhs, const list<T> &rhs)
    {
        return rhs < lhs;
    }

    template<typename T>
    bool operator<=(const list<T> &lhs, const list<T> &rhs)
    {
        return !(rhs < lhs);
    }

    template<typename T>
    bool operator>=(const list<T> &lhs, const list<T> &rhs)
    {
        return !(lhs < rhs);
    }

    template<typename T>
    void swap(list<T> &lhs, list<T> &rhs) noexcept
    {
        lhs.swap(rhs);
    }
}

#endif //MYSTL_LIST_H
