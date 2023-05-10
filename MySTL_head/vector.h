//
// Created by ZYK on 2023/4/13.
//

/**
 * @file vector.h
 * @brief 实现模板类vector
 *
 * @date 2023年4月13日
 * @author ZYK
 */

#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include "allocator.h"
#include "algobase.h"
#include "uninitialized.h"
#include "exceptdef.h"
#include "memory.h"

namespace mystl
{
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif
#ifdef min
#pragma message("#undefing marco max")
#undef min
#endif

    template<typename T>
    class vector
    {
    public:
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;
        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator(); }

    private:
        iterator begin_;
        iterator end_;
        iterator cap_;
    public:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 构造函数
        /// ------------------------------------------------------------------------------------------------------------
        ///@brief 默认构造函数
        vector() noexcept
        {
            try_init();
        }

        ///@brief 有参构造函数
        explicit vector(size_type n)
        {
            fill_init(n, value_type());
        }

        vector(size_type n, const value_type &value)
        {
            fill_init(n, value);
        }

        template<typename Iter, typename std::enable_if<mystl::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last)
        {
            MYSTL_DEBUG(first >= last);
            range_init();
        }

        vector(const vector &lhs)
        {
            range_init(lhs.begin_, lhs.end_);
        }

        vector(vector &&rhs) noexcept: begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_)
        {
            rhs.begin_ = nullptr;
            rhs.end_ = nullptr;
            rhs.cap_ = nullptr;
        }

        vector(std::initializer_list<value_type> ilist)
        {
            range_init(ilist.begin(), ilist.end());
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 赋值运算符重载
        /// ------------------------------------------------------------------------------------------------------------

        vector &operator=(const vector<T> &lhs);

        vector &operator=(vector<T> &&rhs) noexcept;

        vector &operator=(std::initializer_list<value_type> ilist);

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 析构函数
        /// ------------------------------------------------------------------------------------------------------------
        ~vector()
        {
            destroy_and_recover(begin_, end_, cap_ - begin_);
        }

    public:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 迭代器相关操作
        /// ------------------------------------------------------------------------------------------------------------
        iterator begin() noexcept
        {
            return begin_;
        }

        const_iterator begin() const noexcept
        {
            return begin_;
        }

        iterator end() noexcept
        {
            return end_;
        }

        const_iterator end() const noexcept
        {
            return end_;
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return rbegin();
        }

        const_reverse_iterator crend() const noexcept
        {
            return rend();
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 容量相关函数
        /// ------------------------------------------------------------------------------------------------------------

        bool empty() const noexcept
        {
            return begin_ == end_;
        }

        size_type size() const noexcept
        {
            return static_cast<size_type>(end_ - begin_);
        }

        /// @brief  max_size函数，用于获取vector可容纳元素的最大数量
        size_type max_size() const noexcept
        {
            return static_cast<size_type>(-1) / sizeof(T);
        }

        size_type capacity() const noexcept
        {
            return static_cast<size_type>(cap_ - begin_);
        }

        void reserve(size_type n);

        void shrink_to_fit();

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 访问元素相关函数
        /// ------------------------------------------------------------------------------------------------------------

        reference operator[](size_type n)
        {
            MYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }

        const_reference operator[](size_type n) const
        {
            MYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }

        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *begin_;
        }

        const_reference front() const
        {
            MYSTL_DEBUG(!empty());
            return *begin_;
        }

        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        pointer data() noexcept
        {
            return begin_;
        }

        const_pointer data() const noexcept
        {
            return begin_;
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 容器相关函数
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
            MYSTL_DEBUG(!(last < first));
            copy_assign(first, last, iterator_category(first));
        }

        void assign(std::initializer_list<value_type> ilist)
        {
            copy_assign(ilist.begin(), ilist.end(), mystl::forward_iterator_tag{});
        }

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief emplace/emplace_back
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        template<typename... Args>
        iterator emplace(const_iterator pos, Args &&...args);

        void swap(vector<T> &lhs) noexcept;

    private:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief helper function
        /// ------------------------------------------------------------------------------------------------------------

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief 初始化/回收函数
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void try_init();

        void init_space(size_type size, size_type cap);

        void fill_init(size_type n, const value_type &value);

        template<typename Iter>
        void range_init(Iter first, Iter last);

        void destroy_and_recover(iterator first, iterator last, size_type n);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief get new capacity
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        size_type get_new_cap(size_type add_size);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief assign
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void fill_assign(size_type n, const value_type &value);

        template<typename IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);

        template<typename FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief reallocate
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        template<class... Args>
        void reallocate_emplace(iterator pos, Args &&...args);

        void reallocate_insert(iterator pos, const value_type &value);

        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        /// @brief shrink_to_fit函数
        /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        void reinsert(size_type size);

    };

    /// ================================================================================================================
    /// @brief 容量相关函数定义
    /// ================================================================================================================

    template<typename T>
    void vector<T>::reserve(size_type n)
    {
        if (capacity() < n)
        {
            // size n 不能超过max_size()
            THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size() in vector<T>::reserve(n)");
            const auto old_size = size();
            auto tmp = data_allocator::allocate(n);
            mystl::uninitialized_move(begin_, end_, tmp);
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = tmp;
            end_ = begin_ + old_size;
            cap_ = begin_ + n;
        }
    }

    /// @brief 放弃多余的容量

    template<typename T>
    void vector<T>::shrink_to_fit()
    {
        if (end_ < cap_)
        {
            reinsert(size());
        }
    }

    /// ================================================================================================================
    /// @brief 容器相关函数定义
    /// ================================================================================================================

    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /// @brief emplace/emplace_back
    /// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    template<typename T>
    template<typename ...Args>
    typename vector<T>::iterator vector<T>::emplace(const_iterator pos, Args &&...args)
    {
        MYSTL_DEBUG(pos >= begin() && pos <= end());
        // xpos表示待插入位置
        auto xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if (end_ != cap_ && xpos == end_)
        {
            // 当空间未满且插入元素在尾部时, 直接在尾部调用construct
            data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
            ++end_;
        }
        else if (end_ != cap_)
        {
            // 当空间未满且插入元素不在尾部时
            auto new_end = end_;
            // 首先在尾部构造一个副本，然后逆序依次拷贝
            data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
            ++new_end;
            mystl::copy_backward(xpos, end_ - 1, end_);
            // 最后在xpos处赋新值
            *xpos = value_type(mystl::forward<Args>(args)...);
            end_ = new_end;
        }
        else
        {
            // 如果空间已满，重新分配空间
            reallocate_emplace(xpos, mystl::forward<Args>(args)...);
        }
        return begin() + n;
    }

    template<typename T>
    void vector<T>::swap(vector<T> &lhs) noexcept
    {
        if (this != &lhs)
        {
            mystl::swap(begin_, lhs.begin_);
            mystl::swap(end_, lhs.end_);
            mystl::swap(cap_, lhs.cap_);
        }
    }

    /// ================================================================================================================
    /// @brief 初始化/回收辅助函数定义
    /// ================================================================================================================

    /**
     * @brief 初始化 分配16个大小的空间
     * */

    template<typename T>
    void vector<T>::try_init()
    {
        try
        {
            begin_ = data_allocator::allocate(16);
            end_ = begin_;
            cap_ = begin_ + 16;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
        }
    }

    /**
     * @brief 调整初始化空间大小
     * @param[in] size 已初始化
     * @param[in] cap 已分配
     * */

    template<typename T>
    void vector<T>::init_space(size_type size, size_type cap)
    {
        try
        {
            begin_ = data_allocator::allocate(cap);
            end_ = begin_ + size;
            cap_ = begin_ + cap;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
            throw;
        }
    }

    template<typename T>
    void vector<T>::fill_init(size_type n, const value_type &value)
    {
        const size_type init_size = mystl::max(static_cast<size_type>(16), n);
        // 空间分配，且保证cap至少为16
        init_space(n, init_size);
        // 填充value
        mystl::uninitialized_fill_n(begin_, n, value);
    }

    template<typename T>
    template<typename Iter>
    void vector<T>::range_init(Iter first, Iter last)
    {
        const size_type len = mystl::distance(first, last);
        const size_type init_size = mystl::max(len, static_cast<size_type>(16));
        init_space(len, init_size);
        mystl::uninitialized_copy(first, last, begin_);
    }

    template<typename T>
    void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n)
    {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }

    /// ================================================================================================================
    /// @brief get new capacity
    /// ================================================================================================================

    template<typename T>
    typename vector<T>::size_type vector<T>::get_new_cap(size_type add_size)
    {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, "vector<T>'s size too big");
        if (old_size > max_size() - old_size / 2)
        {
            return old_size + add_size > max_size() - 16 ? old_size + add_size : old_size + add_size + 16;
        }
        const size_type new_size = old_size == 0 ? mystl::max(add_size, static_cast<size_type>(16))
                                                 : mystl::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
    }

    /// ================================================================================================================
    /// @brief assign辅助函数
    /// ================================================================================================================

    template<typename T>
    void vector<T>::fill_assign(size_type n, const value_type &value)
    {
        if (n > capacity())
        {
            vector tmp(n, value);
            swap(tmp);
        }
        else if (n > size())
        {
            mystl::fill(begin(), end(), value);
            end_ = mystl::uninitialized_fill_n(end_, n - size(), value);
        }
        else
        {
            erase(mystl::fill_n(begin_, n, value), end_);
        }
    }

    template<typename T>
    template<typename IIter>
    void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag)
    {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first, ++cur)
        {
            *cur = *first;
        }
        if (first == last)
        {
            erase(cur, end_);
        }
        else
        {
            insert(end_, first, last);
        }
    }

    template<typename T>
    template<typename FIter>
    void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag)
    {
        const size_type len = mystl::distance(first, last);
        if (len > capacity())
        {
            vector tmp(first, last);
            swap(tmp);
        }
        else if (len > size())
        {
            auto mid = first;
            mystl::advance(mid, size());
            mystl::copy(first, mid, begin_);
            auto new_end = mystl::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        }
        else
        {
            auto new_end = mystl::copy(first, last, begin_);
            data_allocator::destroy(new_end, end_);
            end_ = new_end;
        }
    }

    /// ================================================================================================================
    /// @brief reallocate函数，重新分配空间
    /// ================================================================================================================

    template<class T>
    template<class ...Args>
    void vector<T>::reallocate_emplace(iterator pos, Args &&...args)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try
        {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(mystl::address_of(*new_end), mystl::forward<Args>(args)...);
            ++new_end;
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    /// ================================================================================================================
    /// @brief shrink_to_fit辅助函数定义
    /// ================================================================================================================

    template<typename T>
    void vector<T>::reinsert(size_type size)
    {
        auto new_begin = data_allocator::allocate(size);
        try
        {
            mystl::uninitialized_move(begin_, end_, new_begin);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = begin_ + size;
        cap_ = begin_ + size;
    }

    /// ================================================================================================================
    /// @brief 重载赋值运算符
    /// ================================================================================================================

    ///@brief 拷贝赋值运算符
    template<typename T>
    vector<T> &vector<T>::operator=(const vector<T> &lhs)
    {
        if (this != &lhs)
        {
            const auto len = lhs.size();
            if (len > capacity())
            {
                vector<T> tmp(lhs.begin_, lhs.end_);
                swap(tmp);
            }
            else if (len <= size())
            {
                auto i = mystl::copy(lhs.begin(), lhs.end(), begin());
                data_allocator::destroy(i, end_);
                end_ = begin_ + len;
            }
            else
            {
                // 该条件包括当len 大于 size() 且 小于capcity() 时
                mystl::copy(lhs.begin(), lhs.begin() + size(), begin_);
                mystl::uninitialized_copy(lhs.begin() + size(), lhs.end(), end_);
                // 讲cap_ 和 end_ 调整至扩展后的空间
                cap_ = end_ = begin_ + len;
            }
        }
        return *this;
    }

    ///@brief 移动复制运算符
    template<typename T>
    vector<T> &vector<T>::operator=(vector &&rhs) noexcept
    {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        cap_ = rhs.cap_;
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
        return *this;
    }

    template<typename T>
    vector<T> &vector<T>::operator=(std::initializer_list<value_type> ilist)
    {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
    }

}

#endif //MYSTL_VECTOR_H
