
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>

namespace observable { 

template<typename Observed_>
struct unordered_set;
    
template<typename Observed>
struct observable_of<
    Observed,
    typename std::enable_if<is_unordered_set<Observed>::value>::type
>
{
    using type = unordered_set<Observed>;
};
    
template<typename Observed_>
struct unordered_set
{
    using Observed = Observed_;

    using hasher = typename Observed::hasher;
    using key_equal = typename Observed::key_equal;
    using key_type = typename Observed::key_type;
    using value_type = typename Observed::value_type;
    using reference = typename Observed::reference;
    using const_reference = typename Observed::const_reference;
    using pointer = typename Observed::pointer;
    using const_pointer = typename Observed::const_pointer;
    using iterator = typename Observed::iterator;
    using const_iterator = typename Observed::const_iterator;
    using size_type = typename Observed::size_type;
    using difference_type = typename Observed::difference_type;
    using allocator_type = typename Observed::allocator_type;
    
    unordered_set() = default;
    
    unordered_set(Observed& observed)
        : _observed(&observed)
    {}
    
    iterator begin() noexcept
    { return _observed->begin(); }

    const_iterator cbegin() const noexcept
    { return _observed->cbegin(); }

    iterator end() noexcept
    { return _observed->end(); }
    
    const_iterator cend() const noexcept
    { return _observed->cend(); }
    
    bool empty() const noexcept
    { return _observed->empty(); }
    
    size_type size() const noexcept
    { return _observed->size(); }
    
    size_type max_size() const noexcept
    { return _observed->max_size(); }
        
    void clear() noexcept
    {
        _observed->clear();
        _on_erase(*_observed, value_type{}, const_iterator{});
        _on_change(*_observed);
    }
    
    iterator erase(const_iterator pos)        
    {
        auto e = *pos;
        auto it = _observed->erase(pos);
        _on_erase(*_observed, std::move(e), it);
        _on_change(*_observed);
        return it;
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        auto it = _observed->erase(first, last);
        _on_erase(*_observed, value_type{}, it);
        _on_change(*_observed);
        return it;
    }
    
    size_type erase(const key_type& key)
    {
        auto n = _observed->erase(key);
        if (n > 0)
        {
            //TODO:on_erase
            _on_erase(*_observed, value_type{}, const_iterator{});
            _on_change(*_observed);
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _observed->emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _on_insert(*_observed, ret.first);
            _on_change(*_observed);
        }
        return ret;
    }
    
    template<typename... Args>
    iterator emplace_hint
    (const_iterator hint, Args&&... args)
    {
        auto before_size = _observed->size();
        auto it = _observed->emplace_hint(hint, std::forward<Args>(args)...);
        if (_observed->size() != before_size)
        {
            _on_insert(*_observed, it);
            _on_change(*_observed);
        }
        return it;
    }

    std::pair<iterator, bool> insert
    (const value_type& value)
    {
        auto ret = _observed->insert(value);
        if (ret.second)
        {
            _on_insert(*_observed, ret.first);
            _on_change(*_observed);
        }
        return ret;        
    }
    
    std::pair<iterator, bool> insert
    (value_type&& value)
    {
        auto ret = _observed->insert(std::move(value));
        if (ret.second)
        {
            _on_insert(*_observed, ret.first);
            _on_change(*_observed);
        }
        return ret;        
    }
    
    iterator insert_hint
    (const_iterator hint,
     const value_type& value)
    {
        auto before_size = _observed->size();
        auto it = _observed->insert(hint, value);
        if (_observed->size() != before_size)
        {
            _on_insert(*_observed, it);
            _on_change(*_observed);
        }
        return it;
    }
    
    iterator insert_hint
    (const_iterator hint,
     value_type&& value)
    {
        auto before_size = _observed->size();
        auto it = _observed->insert(hint, std::move(value));
        if (_observed->size() != before_size)
        {
            _on_insert(*_observed, it);
            _on_change(*_observed);
        }
        return it;
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _observed->size();
        _observed->insert(first, last);
        if (_observed->size() != before_size)
        {
            _on_insert(*_observed, const_iterator{}); //TODO
            _on_change(*_observed);
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }
    
    void swap(Observed& other)
    {
        _observed->swap(other);
        //TODO: check?
        _on_insert(*_observed, const_iterator{});
        _on_erase(*_observed, value_type{}, const_iterator{});
        _on_change(*_observed);
    }

    size_type count
    (const key_type& key) const noexcept
    { return _observed->count(key); }
    
    iterator find
    (const key_type& key)
    { return _observed->find(key); }
    
    const_iterator find
    (const key_type& key) const
    { return _observed->find(key); }
    
    std::pair<iterator,
              iterator> equal_range
    (const key_type& key)
    { return _observed->equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _observed->equal_range(key); }
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    { return _on_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    { return _on_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    const Observed& get() const noexcept
    { return *_observed; }
    
    Observed* _observed;
    
    boost::signals2::signal<void(const Observed&, const_iterator)>
    _on_insert, _on_value_change;
    
    boost::signals2::signal<void(const Observed&, value_type, const_iterator)>
    _on_erase;
    
    boost::signals2::signal<void(const Observed&)> _on_change;
};
    
}
