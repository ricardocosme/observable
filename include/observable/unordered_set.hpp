
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <unordered_set>

namespace observable {

template<typename T>
class unordered_set
{
    using container_t = std::unordered_set<T>;
    container_t _container;
    
    boost::signals2::signal<void(typename container_t::const_iterator)>
    _on_insert;    
    boost::signals2::signal<void(typename container_t::value_type,
                                 typename container_t::const_iterator)>
    _on_erase;    
    boost::signals2::signal<void()> _on_change;
    
public:
    
    using Model = container_t;
    
    using hasher = typename container_t::hasher;
    using key_equal = typename container_t::key_equal;
    using key_type = typename container_t::key_type;
    using value_type = typename container_t::value_type;
    using reference = typename container_t::reference;
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;
    
    explicit unordered_set(size_type n = 10)
        : _container(n)
    {}
    
    unordered_set(container_t observed)
    {
        for(auto& e : observed)
            _container.insert(value_type{std::move(e)});
    }

    template<typename InputIterator>
    unordered_set(InputIterator first, InputIterator last, size_type n = 0)
        : _container(first, last, n)
    {}

    unordered_set(std::initializer_list<value_type> l, size_type n = 0)
        : _container(std::move(l), n)
    {}
    
    iterator begin() noexcept
    { return _container.begin(); }

    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }

    iterator end() noexcept
    { return _container.end(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }
        
    void clear() noexcept
    {
        _container.clear();
        _on_erase(value_type{}, const_iterator{});
        _on_change();
    }
    
    iterator erase(const_iterator pos)        
    {
        auto e = *pos;
        auto it = _container.erase(pos);
        _on_erase(std::move(e), it);
        _on_change();
        return it;
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        auto it = _container.erase(first, last);
        _on_erase(value_type{}, it);
        _on_change();
        return it;
    }
    
    size_type erase(const key_type& key)
    {
        auto n = _container.erase(key);
        if (n > 0)
        {
            //TODO:on_erase
            _on_erase(value_type{}, const_iterator{});
            _on_change();
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _container.emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
        }
        return ret;
    }
    
    template<typename... Args>
    iterator emplace_hint
    (const_iterator hint, Args&&... args)
    {
        auto before_size = _container.size();
        auto it = _container.emplace_hint(hint, std::forward<Args>(args)...);
        if (_container.size() != before_size)
        {
            _on_insert(it);
            _on_change();
        }
        return it;
    }

    std::pair<iterator, bool> insert
    (const value_type& value)
    {
        auto ret = _container.insert(value);
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
        }
        return ret;        
    }
    
    std::pair<iterator, bool> insert
    (value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
        }
        return ret;        
    }
    
    iterator insert_hint
    (const_iterator hint,
     const value_type& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, value);
        if (_container.size() != before_size)
        {
            _on_insert(it);
            _on_change();
        }
        return it;
    }
    
    iterator insert_hint
    (const_iterator hint,
     value_type&& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, std::move(value));
        if (_container.size() != before_size)
        {
            _on_insert(it);
            _on_change();
        }
        return it;
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _container.size();
        _container.insert(first, last);
        if (_container.size() != before_size)
        {
            _on_insert(const_iterator{}); //TODO
            _on_change();
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }
    
    void swap(container_t& other)
    {
        _container.swap(other);
        //TODO: check?
        _on_insert(const_iterator{});
        _on_erase(value_type{}, const_iterator{});
        _on_change();
    }

    size_type count
    (const key_type& key) const noexcept
    { return _container.count(key); }
    
    iterator find
    (const key_type& key)
    { return _container.find(key); }
    
    const_iterator find
    (const key_type& key) const
    { return _container.find(key); }
    
    std::pair<iterator,
              iterator> equal_range
    (const key_type& key)
    { return _container.equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
    
    const container_t& model() const noexcept
    { return _container; }
    
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
    
}
