
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/exp/is_observable.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>

namespace observable { namespace exp {

template<typename Key, typename T, typename Enable = void>
class unordered_map;
        
template<typename Key, typename T>
class unordered_map<
    Key,
    T,
    typename std::enable_if<!is_observable<T>::value>::type
>
{
    using container_t = std::unordered_map<Key, T>;
    container_t _container;
    
    boost::signals2::signal<void(typename container_t::const_iterator)>
    _on_erase, _on_insert, _before_erase;
    
    boost::signals2::signal<void()> _on_change;

public:
    
    using key_type = typename container_t::key_type;
    using mapped_type = typename container_t::mapped_type;
    using value_type = typename container_t::value_type;
    using key_equal = typename container_t::key_equal;
    using hasher = typename container_t::hasher;
    using reference = typename container_t::reference;
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    //TODO local_iterator
    using const_local_iterator = typename container_t::const_local_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    explicit unordered_map(size_type n = 10)
        : _container(n)
    {}

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last,
                  size_type n = 0)
        : _container(first, last, n)
    {}
    
    unordered_map(std::initializer_list<value_type> l,
                  size_type n = 0)
        : _container(std::move(l), n)
    {}
    
    iterator begin() noexcept
    { return _container.begin(); }

    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    const_iterator end() const noexcept
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
        _before_erase(_container.cend());
        _container.clear();
        _on_erase(_container.cend());
        _on_change();
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
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
        }
        return ret;
    }
    
    iterator insert(const_iterator hint,
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
    { insert(ilist.begin(), ilist.end()); }
    
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
    
    iterator erase(const_iterator pos)        
    {
        _before_erase(pos);
        auto it = _container.erase(pos);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        //TODO range?
        _before_erase(first);
        auto it = _container.erase(first, last);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto e = *rng.first;
        auto before_size = _container.size();
        _before_erase(rng.first);
        _container.erase(rng.first, rng.second);
        auto n = before_size - _container.size();
        if (n > 0)
        {
            _on_erase(rng.second);
            _on_change();
        }
        return n;
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
    
    mapped_type& at(const key_type& key)
    { return _container.at(key); }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
    
    mapped_type& operator[](const key_type& key)
    { return _container[key]; }
    
    mapped_type& operator[](key_type&& key)
    { return _container[std::move(key)]; }
    
    size_type count
    (const key_type& key) const noexcept
    { return _container.count(key); }
    
    iterator find(const key_type& key)
    { return _container.find(key); }
    
    const_iterator find
    (const key_type& key) const
    { return _container.find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    { return _container.equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
                        
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
    
template<typename Key, typename T>
class unordered_map<
    Key,
    T,
    typename std::enable_if<is_observable<T>::value>::type
>
{
    using container_t = std::unordered_map<Key, T>;
    container_t _container;
    
    boost::signals2::signal<void(typename container_t::const_iterator)>
    _on_erase, _on_insert, _before_erase, _on_value_change;
    
    boost::signals2::signal<void()> _on_change;

    std::unordered_map<
        typename container_t::const_pointer,
        boost::signals2::scoped_connection            
    > _ptr2value;
    
    void insert_value_change_conn(typename container_t::value_type& e)
    {
        _ptr2value.emplace
            (&e,
             e.second.on_change
             ([this, &e]()
              {
                  auto it = _container.find(e.first);
                  assert(it != _container.end());
                  _on_value_change(it);
                  _on_change();
              }));             
    }
    
public:
    
    using key_type = typename container_t::key_type;
    using mapped_type = typename container_t::mapped_type;
    using value_type = typename container_t::value_type;
    using key_equal = typename container_t::key_equal;
    using hasher = typename container_t::hasher;
    using reference = typename container_t::reference;
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    //TODO local_iterator
    using const_local_iterator = typename container_t::const_local_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    explicit unordered_map(size_type n = 10)
        : _container(n)
    {}

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last,
                  size_type n = 0)
        : _container(first, last, n)
    {}
    
    unordered_map(std::initializer_list<value_type> l,
                  size_type n = 0)
        : _container(std::move(l), n)
    {}
    
    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    iterator begin() noexcept
    { return _container.begin(); }

    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    const_iterator end() const noexcept
    { return _container.end(); }
    
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
        _before_erase(_container.cend());
        _container.clear();
        _on_erase(_container.cend());
        _on_change();
    }
    
    std::pair<iterator, bool> insert
    (const value_type& value)
    {
        auto ret = _container.insert(value);
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
            insert_value_change_conn(*ret.first);
        }
        return ret;
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
            insert_value_change_conn(*ret.first);
        }
        return ret;
    }
    
    iterator insert(const_iterator hint,
                    value_type&& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, std::move(value));
        if (_container.size() != before_size)
        {
            _on_insert(it);
            _on_change();
            insert_value_change_conn(*it);
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
            //TODO
            // std::for_each(it, it + std::distance(first, last),
            //               [this](value_type& e)
            //               { insert_value_change_conn(e); });
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _container.emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _on_insert(ret.first);
            _on_change();
            insert_value_change_conn(*ret.first);
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
            insert_value_change_conn(*it);
        }
        return it;
    }
    
    iterator erase(const_iterator pos)        
    {
        _before_erase(pos);
        _ptr2value.erase(&*pos);
        auto it = _container.erase(pos);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        //TODO range?
        _before_erase(first);
        std::for_each(first, last,
                      [this](const value_type& e)
                      { _ptr2value.erase(&e); });
        auto it = _container.erase(first, last);
        _on_erase(it);
        _on_change();
        return it;
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto before_size = _container.size();
        _ptr2value.erase(&*rng.first);
        _before_erase(rng.first);
        _container.erase(rng.first, rng.second);
        auto n = before_size - _container.size();
        if (n > 0)
        {
            _on_erase(rng.second);
            _on_change();
        }
        return n;
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
    
    mapped_type& at(const key_type& key)
    { return _container.at(key); }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
    
    mapped_type& operator[](const key_type& key)
    { return _container[key]; }
    
    mapped_type& operator[](key_type&& key)
    { return _container[std::move(key)]; }
    
    size_type count
    (const key_type& key) const noexcept
    { return _container.count(key); }
    
    iterator find(const key_type& key)
    { return _container.find(key); }
    
    const_iterator find
    (const key_type& key) const
    { return _container.find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    { return _container.equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
                        
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
}}
