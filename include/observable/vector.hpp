
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
struct vector;
    
template<typename Observed>
struct observable_of<
    Observed,
    typename std::enable_if<is_vector<Observed>::value>::type
>
{
    using type = vector<Observed>;
};
    
template<typename Observable>    
class vector_iterator
    : public boost::iterator_adaptor<
        vector_iterator<Observable>,
        typename Observable::Observed::iterator,
        std::shared_ptr<typename Observable::reference>,
        boost::bidirectional_traversal_tag,
        std::shared_ptr<typename Observable::reference>,
        typename Observable::Observed::difference_type
    >
{
public:
    vector_iterator() = default;

    explicit vector_iterator
    (Observable& observable, const typename Observable::Observed::iterator& it)
        : vector_iterator::iterator_adaptor_(it)
        , _observable(&observable)
    {}
private:
    friend class boost::iterator_core_access;
    
    std::shared_ptr<typename Observable::reference> dereference() const
    { return _observable->get_reference(this->base_reference()); }
    
    Observable* _observable{nullptr};    
};
            
template<typename Observed_>
struct vector
{
    using Observed = Observed_;

    using value_type = typename Observed::value_type;
    using reference = observable_of_t<typename Observed::value_type>;    
    using const_reference = typename Observed::const_reference;
    using pointer = reference*;
    using const_pointer = typename Observed::const_pointer;
    using iterator = vector_iterator<vector<Observed>>;
    using reverse_iterator = boost::reverse_iterator<iterator>;
    using const_iterator = typename Observed::const_iterator;
    using const_reverse_iterator = typename Observed::const_reverse_iterator;
    using size_type = typename Observed::size_type;
    using difference_type = typename Observed::difference_type;
    using allocator_type = typename Observed::allocator_type;

    vector() = default;
    
    vector(Observed& observed)
        : _observed(&observed)
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

    void reserve(size_type new_cap)
    { _observed->reserve(new_cap); }
    
    size_type capacity() const noexcept
    { return _observed->capacity(); }
    
    void shrink_to_fit() const noexcept
    { return _observed->shrink_to_fit(); }
    
    std::shared_ptr<reference> at(size_type pos)
    {
        if (pos >= _observed->size()) throw std::out_of_range("vector::at");
        auto it = _observed->begin() + pos;
        return get_reference(it);
    }
    
    const_reference at(size_type pos) const
    { return _observed->at(pos); }
    
    std::shared_ptr<reference> operator[](size_type pos)
    {
        auto it = _observed->begin() + pos;
        return get_reference(it);
    }
    
    const_reference operator[]
    (size_type pos) const
    { return (*_observed)[pos]; }

    std::shared_ptr<reference> front()
    {
        auto it = _observed->begin();
        return get_reference(it);
    }
        
    const_reference front() const
    { return _observed->front(); }
    
    std::shared_ptr<reference> back()
    {
        auto it = _observed->end();
        --it;
        return get_reference(it);
    }
        
    const_reference back() const
    { return _observed->back(); }
    
    void clear() noexcept
    {
        _before_erase(*_observed, _observed->cend());
        _observed->clear();
        _on_erase(*_observed, const_iterator{});
        _on_change(*_observed);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator pos)        
    {
        _before_erase(*_observed, pos.base());
        auto it = _observed->erase(pos.base());
        _on_erase(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator first, iterator last)        
    {
        //TODO range?
        _before_erase(*_observed, first.base());
        auto it = _observed->erase(first.base(), last.base());
        _on_erase(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(typename Observed::iterator pos, Args&&... args)
    {
        auto it = _observed->emplace(pos, std::forward<Args>(args)...);
        _on_insert(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
    
    template <typename Container, typename ConstIterator>
    typename Container::iterator remove_constness(Container& c,
                                                  ConstIterator it)
    {
        return c.erase(it, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename Observed::iterator pos,
                    const value_type& value)
    {
        auto it = _observed->insert(pos, value);
        _on_insert(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename Observed::iterator pos,
                    value_type&& value)
    {
        auto it = _observed->insert(pos, std::move(value));
        _on_insert(*_observed, it);
        _on_change(*_observed);
        return iterator(*this, it);
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(typename Observed::iterator pos,
                size_type count,
                const value_type& value)
    {
        _observed->insert(pos, count, value);
        _on_insert(*_observed, pos);
        _on_change(*_observed);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(typename Observed::iterator pos, InputIt first, InputIt last)
    {
        _observed->insert(pos, first, last);
        _on_insert(*_observed, pos);
        _on_change(*_observed);
    }
        
    void insert(typename Observed::iterator pos,
                std::initializer_list<value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    void push_back(const value_type& value)
    {
        _observed->push_back(value);
        _on_insert(*_observed, std::prev(_observed->end()));
        _on_change(*_observed);
    }
    
    void push_back(value_type&& value)
    {
        _observed->push_back(std::move(value));
        _on_insert(*_observed, std::prev(_observed->end()));
        _on_change(*_observed);
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _observed->emplace_back(std::forward<Args>(args)...);
        _on_insert(*_observed, std::prev(_observed->end()));
        _on_change(*_observed);
    }
    
    void pop_back()
    {
        _before_erase(*_observed, _observed->rbegin().base());
        _observed->pop_back();
        _on_erase(*_observed, const_iterator{});
        _on_change(*_observed);
    }
    
    void swap(Observed& other)
    {
        _before_erase(*_observed, _observed->cend());
        _observed->swap(other);
        //TODO: check?
        _on_insert(*_observed, const_iterator{});
        _on_erase(*_observed, const_iterator{});
        _on_change(*_observed);
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
    
    const Observed& get() const noexcept
    { return *_observed; }
    
    Observed* _observed;
    
    boost::signals2::signal<void(const Observed&, const_iterator)>
    _on_erase, _on_insert, _on_value_change, _before_erase;
    
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
                (new reference(observable_factory(*it)),
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
    friend class vector_iterator<vector<Observed>>;
};
    
}
