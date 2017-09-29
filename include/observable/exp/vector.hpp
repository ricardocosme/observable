
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/exp/is_observable.hpp"
#include "observable/exp/types.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace observable { namespace exp {

template<typename T, typename Enable = void>
class vector;
    
template<typename T>
class vector<
    T,
    typename std::enable_if<!is_observable<T>::value>::type
>
{
    using container_t = std::vector<T>;
    container_t _container;
    
    boost::signals2::signal<void(typename container_t::const_iterator)>
    _on_erase, _on_insert, _before_erase;
    
    boost::signals2::signal<void()> _on_change;
public:    
    using value_type = typename container_t::value_type;
    using reference = typename container_t::reference;    
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_iterator = typename container_t::const_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    vector() = default;
    
    vector(container_t observed)
    {
        _container.reserve(observed.size());
        for(auto& e : observed)
            _container.push_back(value_type{std::move(e)});
    }
    
    iterator begin() noexcept
    { return _container.begin(); }

    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }

    const_reverse_iterator crbegin() noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    const_reverse_iterator crend() noexcept
    { return _container.crend(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }

    void reserve(size_type new_cap)
    { _container.reserve(new_cap); }
    
    size_type capacity() const noexcept
    { return _container.capacity(); }
    
    void shrink_to_fit() noexcept
    { return _container.shrink_to_fit(); }
    
    reference at(size_type pos)
    { return _container.at(pos); }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }
    
    reference operator[](size_type pos)
    { return _container[pos]; }
    
    const_reference operator[](size_type pos) const
    { return _container[pos]; }

    reference front()
    { return _container.front(); }
        
    const_reference front() const
    { return _container.front(); }
    
    reference back()
    { return _container.back(); }
        
    const_reference back() const
    { return _container.back(); }
    
    void clear() noexcept
    {
        _before_erase(_container.cend());
        _container.clear();
        _on_erase(const_iterator{});
        _on_change();
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator pos)        
    {
        _before_erase(pos);
        auto it = _container.erase(pos);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator first, iterator last)        
    {
        //TODO range?
        _before_erase(first);
        auto it = _container.erase(first, last);
        _on_erase(it);
        _on_change();
        return it;
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        auto it = _container.emplace(pos, std::forward<Args>(args)...);
        _on_insert(it);
        _on_change();
        return it;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, const value_type& value)
    {
        auto it = _container.insert(pos, value);
        _on_insert(it);
        _on_change();
        return it;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, value_type&& value)
    {
        auto it = _container.insert(pos, std::move(value));
        _on_insert(it);
        _on_change();
        return it;
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(iterator pos,
                size_type count,
                const value_type& value)
    {
        _container.insert(pos, count, value);
        _on_insert(pos);
        _on_change();
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(iterator pos, InputIt first, InputIt last)
    {
        _container.insert(pos, first, last);
        _on_insert(pos);
        _on_change();
    }
        
    void insert(iterator pos,
                std::initializer_list<value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    void push_back(const value_type& value)
    {
        _container.push_back(value);
        _on_insert(std::prev(_container.end()));
        _on_change();
    }
    
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        _on_insert(std::prev(_container.end()));
        _on_change();
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        _on_insert(std::prev(_container.end()));
        _on_change();
    }
    
    void pop_back()
    {
        _before_erase(std::prev(_container.rbegin().base()));
        _container.pop_back();
        _on_erase(const_iterator{});
        _on_change();
    }
    
    void swap(container_t& other)
    {
        _before_erase(_container.cend());
        _container.swap(other);
        //TODO: check?
        _on_insert(const_iterator{});
        _on_erase(const_iterator{});
        _on_change();
    }

    //TODO resize
    
    template<typename F>
    boost::signals2::connection before_erase(F&& f)
    { return _before_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    { return _on_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    { return _on_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
};
    
template<typename T>
class vector<
    T,
    typename std::enable_if<is_observable<T>::value>::type
>
{
    using container_t = std::vector<T>;
    container_t _container;
    
    boost::signals2::signal<void(typename container_t::const_iterator)>
    _on_erase, _on_insert, _on_value_change, _before_erase;
    
    boost::signals2::signal<void()> _on_change;

    std::unordered_map<
        std::string,
        boost::signals2::scoped_connection            
    > _uuid2value;
    
    void insert_value_change_conn(typename container_t::value_type& e)
    {
        auto& uuid = e.tag;
        _uuid2value.emplace
            (uuid,
             e.on_change
             ([this, uuid]()
              {
                  auto it = std::find_if
                      (_container.begin(),
                       _container.end(),
                       [uuid](const typename container_t::value_type& e)
                       { return e.tag == uuid; });
                  assert(it != _container.end());
                  _on_value_change(it);
                  _on_change();
              }));             
    }
    
public:
    
    using value_type = typename container_t::value_type;
    using reference = typename container_t::reference;    
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_iterator = typename container_t::const_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    vector() = default;
    
    explicit vector(size_type n) : _container(n)
    {}
    
    vector(size_type n, const value_type& value)
        : _container(n, value)
    {}
    
    vector(std::initializer_list<value_type> l)
        : _container(std::move(l))
    {}

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last)
        : _container(first, last)
    {}
    
    vector(vector&& rhs)
        : _container(std::move(rhs._container))
        , _on_erase(std::move(rhs._on_erase))
        , _on_insert(std::move(rhs._on_insert))
        , _on_value_change(std::move(rhs._on_value_change))
        , _before_erase(std::move(rhs._before_erase))
        , _on_change(std::move(rhs._on_change))
    {
        rhs._uuid2value.clear();
        for(auto& e : _container)
            insert_value_change_conn(e);
    }

    vector& operator=(vector&& rhs)
    {
        _container = std::move(rhs._container);
        _on_erase = std::move(rhs._on_erase);
        _on_insert = std::move(rhs._on_insert);
        _on_value_change = std::move(rhs._on_value_change);
        _before_erase = std::move(rhs._before_erase);
        _on_change = std::move(rhs.on_change);

        rhs._uuid2value.clear();
        for(auto& e : _container)
            insert_value_change_conn(e);
    }
    
    iterator begin() noexcept
    { return _container.begin(); }

    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }

    const_reverse_iterator crbegin() noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    const_reverse_iterator crend() noexcept
    { return _container.crend(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }

    void reserve(size_type new_cap)
    { _container.reserve(new_cap); }
    
    size_type capacity() const noexcept
    { return _container.capacity(); }
    
    void shrink_to_fit() noexcept
    { return _container.shrink_to_fit(); }
    
    reference at(size_type pos)
    { return _container.at(pos); }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }
    
    reference operator[](size_type pos)
    { return _container[pos]; }
    
    const_reference operator[](size_type pos) const
    { return _container[pos]; }

    reference front()
    { return _container.front(); }
        
    const_reference front() const
    { return _container.front(); }
    
    reference back()
    { return _container.back(); }
        
    const_reference back() const
    { return _container.back(); }
    
    void clear() noexcept
    {
        _before_erase(_container.cend());
        _container.clear();
        _uuid2value.clear();
        _on_erase(const_iterator{});
        _on_change();
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator pos)        
    {
        _before_erase(pos);
        auto uuid = pos->tag;
        auto it = _container.erase(pos);
        _uuid2value.erase(uuid);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator first, iterator last)        
    {
        //TODO range?
        _before_erase(first);
        std::for_each(first, last,
                      [this](const value_type& e)
                      { _uuid2value.erase(e.tag); });
        auto it = _container.erase(first, last);
        _on_erase(it);
        _on_change();
        return it;
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        auto it = _container.emplace
            (pos, std::forward<Args>(args)...);
        insert_value_change_conn(*it);
        _on_insert(it);
        _on_change();
        return it;
    }
    
    // //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    // iterator insert(iterator pos, const value_type& value)
    // {
    //     auto it = _container.insert(pos, observable_factory(value));
    //     insert_value_change_conn(*it);
    //     _on_insert(it);
    //     _on_change();
    //     return it;
    // }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, value_type&& value)
    {
        auto it = _container.insert(pos, std::move(value));
        insert_value_change_conn(*it);
        _on_insert(it);
        _on_change();
        return it;
    }
            
    // //GCC 4.8.2 uses iterator and not const_iterator for the first
    // //argument and `void` as return type instead of iterator
    // void insert(iterator pos,
    //             size_type count,
    //             const typename value_type::Observed& value)
    // {
    //     _container.insert(pos, count, value);
    //     _on_insert(pos);
    //     _on_change();
    // }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(iterator pos, InputIt first, InputIt last)
    {
        auto it = _container.insert(pos, first, last);
        std::for_each(it, it + std::distance(first, last),
                      [this](value_type& e)
                      { insert_value_change_conn(e); });
        _on_insert(pos);
        _on_change();
    }
        
    void insert(iterator pos,
                std::initializer_list<value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    // void push_back(const value_type& value)
    // {
    //     _container.push_back(observable_factory(value));
    //     insert_value_change_conn(_container.back());
    //     _on_insert(std::prev(_container.end()));
    //     _on_change();
    // }
    
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        insert_value_change_conn(_container.back());
        _on_insert(std::prev(_container.end()));
        _on_change();
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        insert_value_change_conn(_container.back());
        _on_insert(std::prev(_container.end()));
        _on_change();
    }
    
    void pop_back()
    {
        _before_erase(std::prev(_container.rbegin().base()));
        _uuid2value.erase(_container.back().tag);
        _container.pop_back();
        _on_erase(const_iterator{});
        _on_change();
    }
    
    void swap(container_t& other)
    {
        _before_erase(_container.cend());
        _uuid2value.clear();
        _container.swap(other);
        //TODO: check?
        _on_insert(const_iterator{});
        _on_erase(const_iterator{});
        _on_change();
    }

    //TODO resize
    
    template<typename F>
    boost::signals2::connection before_erase(F&& f)
    { return _before_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    { return _on_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    { return _on_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }
    
};
    
}}
