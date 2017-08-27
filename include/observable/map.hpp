
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>

namespace observable { 

template<typename Observed_>
struct map;
    
template<typename Observed>
struct observable_of<
    Observed,
    typename std::enable_if<is_map<Observed>::value>::type
>
{
    using type = map<Observed>;
};
    
template<typename Observed>    
class map_iterator
    : public boost::iterator_adaptor<
        map_iterator<Observed>,
        typename Observed::Observed::iterator,
        std::pair<
            typename Observed::Observed::key_type,
            std::shared_ptr<typename Observed::reference>
        >,
        boost::bidirectional_traversal_tag,
        std::pair<
            typename Observed::Observed::key_type,
            std::shared_ptr<typename Observed::reference>
            >,
        typename Observed::Observed::difference_type
    >
{
public:
    map_iterator() = default;

    explicit map_iterator
    (Observed& observed, const typename Observed::Observed::iterator& it)
        : map_iterator::iterator_adaptor_(it)
        , _observed(&observed)
    {}
private:
    friend class boost::iterator_core_access;
    using value = std::pair<
        typename Observed::Observed::key_type,
        std::shared_ptr<typename Observed::reference>
        >;
    
    value dereference() const
    {
        auto it = this->base_reference();
        //TODO:lvalue ref?
        auto& p = *it;        
        return std::make_pair(p.first, _observed->get_reference(it));
    }
    Observed* _observed{nullptr};    
};

template<typename Observed_>
struct map
{
    using Observed = Observed_;

    using key_type = typename Observed::key_type;
    using mapped_type = typename Observed::mapped_type;
    using value_type = typename Observed::value_type;
    using key_compare = typename Observed::key_compare;
    using reference = observable_of_t<typename Observed::mapped_type>;
    using const_reference = typename Observed::const_reference;
    using pointer = reference*;
    using const_pointer = typename Observed::const_pointer;
    using iterator = map_iterator<map<Observed>>;
    using reverse_iterator = boost::reverse_iterator<iterator>;
    using const_iterator = typename Observed::const_iterator;
    using const_reverse_iterator = typename Observed::const_reverse_iterator;
    using size_type = typename Observed::size_type;
    using difference_type = typename Observed::difference_type;
    using allocator_type = typename Observed::allocator_type;

    map() = default;
    
    map(Observed& value)
        : _observed(&value)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _observed->begin()); }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator
            (iterator(*this, _observed->end()));
    }
    
    const_iterator cbegin() const noexcept
    { return _observed->cbegin(); }

    const_reverse_iterator crbegin() noexcept
    { return _observed->crbegin(); }
    
    iterator end() noexcept
    { return iterator(*this, _observed->end()); }
    
    reverse_iterator rend() noexcept
    {
        return reverse_iterator
            (iterator(*this, _observed->begin()));
    }
    
    const_iterator cend() const noexcept
    { return _observed->cend(); }
    
    const_reverse_iterator crend() noexcept
    { return _observed->crend(); }
    
    bool empty() const noexcept
    { return _observed->empty(); }
    
    size_type size() const noexcept
    { return _observed->size(); }
    
    size_type max_size() const noexcept
    { return _observed->max_size(); }

    std::shared_ptr<reference> at(const key_type& key)
    {
        auto it = _observed->lower_bound(key);
        if (it == _observed->end() || _observed->key_comp()(key, (*it).first))
            throw std::out_of_range("map::at");
        return get_reference(it);
    }
    
    const mapped_type&
    at(const key_type& key) const
    { return _observed->at(key); }
    
    std::shared_ptr<reference> operator[](const key_type& key)
    {
        auto it = _observed->lower_bound(key);
        if (it == _observed->end() || _observed->key_comp()(key, (*it).first))
            it = _observed->insert(it, value_type
                               (key, mapped_type()));
        return get_reference(it);
    }
    
    std::shared_ptr<reference> operator[](key_type&& key)
    {
        auto it = _observed->lower_bound(key);
        if (it == _observed->end() || _observed->key_comp()(key, (*it).first))
            it = _observed->insert(it, value_type
                               (std::move(key), mapped_type()));
        return get_reference(it);
    }
    
    void clear() noexcept
    {
        _observed->clear();
        _on_erase(*_observed, const_iterator{});
        _on_change(*_observed);
    }
    
    iterator erase(const_iterator pos)        
    {
        auto it = _observed->erase(pos);
        _on_erase(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        auto it = _observed->erase(first, last);
        _on_erase(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _observed->equal_range(key);
        auto before_size = _observed->size();
        erase(rng.first, rng.second);
        auto n = before_size - _observed->size();
        if (n > 0)
        {
            _on_erase(*_observed, rng.second);
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
        return std::make_pair(iterator(*this, ret.first), ret.second);
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
        return iterator(*this, it);
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
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _observed->insert(std::move(value));
        if (ret.second)
        {
            _on_insert(*_observed, ret.first);
            _on_change(*_observed);
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    iterator insert(const_iterator hint,
                    const value_type& value)
    {
        auto before_size = _observed->size();
        auto it = _observed->insert(hint, value);
        if (_observed->size() != before_size)
        {
            _on_insert(*_observed, it);
            _on_change(*_observed);
        }
        return iterator(*this, it);
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
    { insert(ilist.begin(), ilist.end()); }
    
    void swap(Observed& other)
    {
        _observed->swap(other);
        //TODO: check?
        _on_insert(*_observed, const_iterator{});
        _on_erase(*_observed, const_iterator{});
        _on_change(*_observed);
    }

    size_type count
    (const key_type& key) const noexcept
    { return _observed->count(key); }
    
    iterator find(const key_type& key)
    { return iterator(*this, _observed->find(key)); }
    
    const_iterator find
    (const key_type& key) const
    { return _observed->find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    {
        auto p = _observed->equal_range(key);
        return std::make_pair(iterator(*this, p.first),
                              iterator(*this, p.second));
    }
    
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
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }
    
    const Observed& get() const noexcept
    { return *_observed; }
    
    Observed* _observed;
    
    boost::signals2::signal<void(const Observed&, const_iterator)>
    _on_erase, _on_insert, _on_value_change;
    
    boost::signals2::signal<void(const Observed&)> _on_change;
    
    std::unordered_map<const_pointer,
                       std::weak_ptr<reference>> _it2observable;
private:
    std::shared_ptr<reference> get_reference(typename Observed::iterator it)
    {
        auto observable = _it2observable[&*it].lock();
        if (!observable)
        {
            auto& it2observable = _it2observable;
            auto it_ptr = &*it;
            observable = std::shared_ptr<reference>
                (new reference(observable_factory(it->second)),
                 [&it2observable, it_ptr](reference *p)
                 {
                     it2observable.erase(it_ptr);
                     delete p;
                 });
            auto& container = *this;
            observable->_on_change.connect(
                [&container, it](const typename reference::Observed&)
                {
                    container._on_value_change(container.get(), it);
                    container._on_change(container.get());
                });
            it2observable[&*it] = observable;
        }
        return observable;
    }
    friend class map_iterator<map<Observed>>;
};
    
}
