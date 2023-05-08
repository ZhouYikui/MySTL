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
            return reverse_iterator(begin());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(end());
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
        /// @brief 容量相关操作
        /// ------------------------------------------------------------------------------------------------------------
        size_type size() const noexcept
        {
            return static_cast<size_type>(end_ - begin_);
        }

        size_type capacity() const noexcept
        {
            return static_cast<size_type>(cap_ - begin_);
        }

        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 容器相关操作
        /// ------------------------------------------------------------------------------------------------------------

        void swap(vector<T> &lhs) noexcept;

    private:
        /// ------------------------------------------------------------------------------------------------------------
        /// @brief 初始化/回收辅助函数
        /// ------------------------------------------------------------------------------------------------------------
        void try_init();

        void init_space(size_type size, size_type cap);

        void fill_init(size_type n, const value_type &value);

        template<typename Iter>
        void range_init(Iter first, Iter last);

        void destroy_and_recover(iterator first, iterator last, size_type n);
    };

    /// ================================================================================================================
    /// @brief 容器修改相关函数定义
    /// ================================================================================================================

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
    /// @brief helper function
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
