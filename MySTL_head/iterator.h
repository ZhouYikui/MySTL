//
// Created by HP on 2023/4/16.
//

/**
 * @file iterator.h
 * @brief 迭代器iterator的相关实现
 *
 * @date 2023年4月16日
 * @author ZYK
 */

#ifndef MYSTL_ITERATOR_H
#define MYSTL_ITERATOR_H

#include <cstddef>
#include "type_traits.h"

namespace mystl
{
    /// ================================================================================================================
    /// @brief 五种迭代器类型标记
    /// ================================================================================================================
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    /// ================================================================================================================
    /// @brief 迭代器模板
    /// ================================================================================================================

    template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T *, typename Reference = T &>
    struct iterator
    {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Distance difference_type;
    };

    /// ================================================================================================================
    /// @brief 迭代器traits
    /// ================================================================================================================

    /// @brief 检测模板类型T是否具有iterator_category
    /// @brief 如果类型T有iterator_category类型成员，sizeof(test<T>(0)) == sizeof(char)为true
    /// @brief static char test(typename U::iterator_category * = 0)为特化版本
    /// @brief 通过函数重载和模板特化实现条件分支

    template<typename T>
    struct has_iterator_cat
    {
    private:
        struct two
        {
            char a;
            char b;
        };

        template<typename U>
        static two test(...) { return {}; };

        template<typename U>
        static char test(typename U::iterator_category * = 0) { return {}; };

    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    /**
     * @brief 辅助类型推导的结构体，当bool为true时从Iterator提取类型信息
     * @param iterator_category 迭代器标签类型
     * @param value_type 迭代器指向的值类型
     * @param pointer 迭代器指向的值类型的指针类型
     * @param reference 迭代器指向的值类型的引用类型
     * @param difference_type 迭代器之间的距离类型
     * */

    template<typename Iterator, bool>
    struct iterator_traits_impl {};

    template<typename Iterator>
    struct iterator_traits_impl<Iterator, true>
    {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::difference_type difference_type;
    };

    /**
     * @brief 确定迭代器的类型
     * @param bool 为true时，public继承iterator_traits_impl的属性
     * @param bool 为false时，public为空
     * @note 派生类向基类的隐式转换是可行的
     * */

    template<typename Iterator, bool>
    struct iterator_traits_helper {};

    template<typename Iterator>
    struct iterator_traits_helper<Iterator, true> : public iterator_traits_impl<Iterator,
            std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
            std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
    {
    };

    /**
     * @brief 迭代器的萃取器
     * @param Iterator 迭代器
     * @param has_iterator_cat<Iterator>::value 判断是否有iterator_category成员
     * @note if (has_iterator_cat::value == true) -> if (is_convertible(iterator_category to input || output)) -> 继承到五个特征
     * */

    template<typename Iterator>
    struct iterator_traits : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

    /**
     * @brief 针对指针和const指针的偏特化版本
     * @param random_access_iterator_tag 随机访问迭代器类型
     * */
    template<typename T>
    struct iterator_traits<T *>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef ptrdiff_t difference_type;
    };

    template<typename T>
    struct iterator_traits<const T *>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;
        typedef ptrdiff_t difference_type;
    };

    /// ================================================================================================================
    /// @brief 迭代器traits -> is_iterator && what iterator
    /// ================================================================================================================

    /// @brief 目的：判断迭代器T的iterator_category能否隐式转换为U
    /// @brief if false -> m_false_type

    template<typename T, typename U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
            : public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value>
    {
    };

    /// @brief 用于判断某个迭代器类型是否属于某种迭代器

    template<typename T, typename U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {};

    template<typename Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template<typename Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template<typename Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template<typename Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template<typename Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

    template<typename Iterator>
    struct is_iterator : public m_bool_constant<
            is_input_iterator<Iterator>::value || is_output_iterator<Iterator>::value>
    {
    };

    /// ================================================================================================================
    /// @brief 迭代器traits -> 萃取category distance_type value_type
    /// ================================================================================================================

    template<typename Iterator>
    typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator &)
    {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    template<typename Iterator>
    typename iterator_traits<Iterator>::difference_type *distance_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    template<typename Iterator>
    typename iterator_traits<Iterator>::value_type *value_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

    /**
     * @brief 计算迭代器间的距离
     * */

    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++first;
            ++n;
        }
        return n;
    }

    template<typename RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag)
    {
        return last - first;
    }

    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last)
    {
        return distance_dispatch(first, last, iterator_category(first));
    }

    /**
     * @brief 让迭代器前进n个distance
     * */
    template<typename InputIterator, typename Distance>
    void advance_dispatch(InputIterator &i, Distance n, input_iterator_tag)
    {
        while (n--) ++i;
    }

    template<typename BidirectionalIterator, typename Distance>
    void advance_dispatch(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag)
    {
        if (n >= 0) while (n--) ++i;
        else while (n++) --i;
    }

    template<typename RandomIter, typename Distance>
    void advance_dispatch(RandomIter &i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }

    /**
     * @brief 让迭代器增加n个distance，根据不同的iterator_category选择不同的模板函数
     * */

    template<typename InputIterator, typename Distance>
    void advance(InputIterator &i, Distance n)
    {
        advance_dispatch(i, n, iterator_category(i));
    }

    /**
     * @brief reverse_iterator模板类
     * @brief 定义了三个构造函数(默认构造函数，有参构造函数以及拷贝构造函数)
     * @brief 重载了运算符 * -> + - ++ -- += -=
     * */

    template<typename Iterator>
    class reverse_iterator
    {
    private:
        Iterator current;
    public:
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::pointer pointer;
        typedef typename iterator_traits<Iterator>::reference reference;

        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        reverse_iterator() = default;

        explicit reverse_iterator(iterator_type i) : current(i) {}

        reverse_iterator(const self &rhs) : current(rhs.current) {}

    public:
        iterator_type base() const
        {
            return current;
        }

        reference operator*() const
        {
            auto tmp = current;
            return *--tmp;
        }

        pointer operator->() const
        {
            return &(operator*());
        }

        self &operator++()
        {
            --current;
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            --current;
            return tmp;
        }

        self &operator--()
        {
            ++current;
            return *this;
        }

        self operator--(int)
        {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self &operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }

        self operator+(difference_type n) const
        {
            return self(current - n);
        }

        self &operator-=(difference_type n)
        {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const
        {
            return self(current + n);
        }

        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }
    };

    /**
     * @brief 运算符重载
     * */

    template<typename Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return rhs.base() - lhs.base();
    }

    template<typename Iterator>
    bool operator==(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return lhs.base() == rhs.base();
    }

    template<typename Iterator>
    bool operator<(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return rhs.base() < lhs.base();
    }

    template<typename Iterator>
    bool operator!=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Iterator>
    bool operator>(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return rhs < lhs;
    }

    template<typename Iterator>
    bool operator<=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return !(lhs > rhs);
    }

    template<typename Iterator>
    bool operator>=(const reverse_iterator<Iterator> &lhs, const reverse_iterator<Iterator> &rhs)
    {
        return !(lhs < rhs);
    }

}

#endif //MYSTL_ITERATOR_H
