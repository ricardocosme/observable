
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"
#include "observable/proxy.hpp"

#include <boost/signals2/signal.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <vector>
#include <type_traits>

namespace observable { 

template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Model_ = std::vector,
         typename Enable = void>
class vector;    
    
template<typename Observable>    
class vec_iterator
    : public boost::iterator_adaptor<
        vec_iterator<Observable>,
        typename Observable::Model::iterator,
        proxy<Observable>,
        boost::random_access_traversal_tag,
        proxy<Observable>,
        typename Observable::Model::difference_type
    >
{
public:
    vec_iterator() = default;

    explicit vec_iterator
    (Observable& observable, typename Observable::Model::iterator it)
        : vec_iterator::iterator_adaptor_(it)
        , _observable(&observable)
    {}
private:
    friend class boost::iterator_core_access;
    
    proxy<Observable> dereference() const
    { return {*_observable, this->base_reference()}; }
    
    Observable* _observable{nullptr};    
};

template<typename T,
         typename Allocator,
         template <typename, typename> class Model_>
class vector<
    T,
    Allocator,
    Model_,
    typename std::enable_if<!is_observable<T>::value>::type
>
{
    using container_t = Model_<T, Allocator>;
    container_t _container;
    
    boost::signals2::signal<bool(const vector<T, Allocator>&,
                                 typename container_t::const_iterator)>
    _before_erase;
    
    boost::signals2::signal<void(const vector<T, Allocator>&,
                                 typename container_t::const_iterator)>
    _after_erase, _after_insert, _on_value_change;
    
    boost::signals2::signal<void(const vector<T, Allocator>&)> _on_change;

    friend class proxy<vector<T, Allocator>>;
    
public:
    
    using Model = container_t;
    
    using value_type = typename container_t::value_type;
    using reference = proxy<vector<T, Allocator>>;    
    using const_reference = typename container_t::const_reference;
    using pointer = reference*;
    using const_pointer = typename container_t::const_pointer;
    using iterator = vec_iterator<vector<T, Allocator>>;
    using reverse_iterator = boost::reverse_iterator<iterator>;
    using const_iterator = typename container_t::const_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    //TODO: allocator support
    
    vector() = default;
    
    explicit vector(size_type n)
        : _container(n)
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
    
    iterator begin() noexcept
    { return iterator{*this, _container.begin()}; }

    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    reverse_iterator rbegin() noexcept
    { return reverse_iterator{iterator{*this, _container.end()}}; }    

    const_reverse_iterator crbegin() noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return iterator{*this, _container.end()}; }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    reverse_iterator rend() noexcept
    { return reverse_iterator{iterator{*this, _container.begin()}}; }
        
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

    //TODO: notifications?
    void resize(size_type new_size)
    { _container.resize(new_size); }
    
    //TODO: notifications?
    void resize(size_type new_size, const value_type& v)
    { _container.resize(new_size, v); }
    
    reference at(size_type pos)
    {
        if (pos >= _container.size())
            throw std::out_of_range("vector::at");
        auto it = _container.begin() + pos;
        return {*this, it};
    }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }
    
    reference operator[](size_type pos) noexcept
    {
         auto it = _container.begin() + pos;
         return {*this, it};
    }
    
    const_reference operator[](size_type pos) const noexcept
    { return _container[pos]; }

    reference front() noexcept
    { return {*this, _container.begin()}; }
        
    const_reference front() const noexcept
    { return _container.front(); }
    
    reference back() noexcept
    {
        auto it = _container.end();
        --it;
        return {*this, it};
    }
        
    const_reference back() const noexcept
    { return _container.back(); }
    
    bool clear() noexcept
    {
        if(_before_erase(*this, _container.cend()) == false)
            return false;
        _container.clear();
        _after_erase(*this, const_iterator{});
        _on_change(*this);
        return true;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    std::pair<iterator, bool> erase(iterator pos)        
    {
        if(_before_erase(*this, pos.base()) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos.base());
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(iterator{*this, it}, true);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    std::pair<iterator, bool> erase(iterator first, iterator last)        
    {
        if(_before_erase(*this, first.base()) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(first.base(), last.base());
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(iterator{*this, it}, true);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        auto it = _container.emplace(pos.base(), std::forward<Args>(args)...);
        _after_insert(*this, it);
        _on_change(*this);
        return iterator{*this, it};
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, const value_type& value)
    {
        auto it = _container.insert(pos.base(), value);
        _after_insert(*this, it);
        _on_change(*this);
        return iterator{*this, it};
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, value_type&& value)
    {
        auto it = _container.insert(pos.base(), std::move(value));
        _after_insert(*this, it);
        _on_change(*this);
        return iterator{*this, it};
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(iterator pos,
                size_type count,
                const value_type& value)
    {
        auto offset = pos.base() - cbegin();
        _container.insert(pos.base(), count, value);
        _after_insert(*this, cbegin() + offset);
        _on_change(*this);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(iterator pos, InputIt first, InputIt last)
    {
        auto offset = pos.base() - cbegin();
        _container.insert(pos.base(), first, last);
        _after_insert(*this, cbegin() + offset);
        _on_change(*this);
    }
        
    void insert(iterator pos,
                std::initializer_list<value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    void push_back(const value_type& value)
    {
        _container.push_back(value);
        _after_insert(*this, std::prev(_container.end()));
        _on_change(*this);
    }
    
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        _after_insert(*this, std::prev(_container.end()));
        _on_change(*this);
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        _after_insert(*this, std::prev(_container.end()));
        _on_change(*this);
    }
    
    void pop_back()
    {
        if(_before_erase(*this, std::prev(_container.rbegin().base())) == false)
            return;
        _container.pop_back();
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }
    
    void swap(container_t& other) noexcept
    {
        if(_before_erase(*this, cbegin()) == false)
            return;
        _container.swap(other);
        //TODO: check?
        _after_erase(*this, const_iterator{});
        _after_insert(*this, const_iterator{});
        _on_change(*this);
    }
    
    const container_t& model() const noexcept
    { return _container; }
    
    template<typename F>
    boost::signals2::connection before_erase(F&& f)
    { return _before_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection after_erase(F&& f)
    { return _after_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection after_insert(F&& f)
    { return _after_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }
};

struct parent_observable
{
    template<typename Observable>
    static void watch_child(Observable& observable,
                            typename Observable::value_type& e)
    {
        e._on_change.connect
            (0, 
             [&observable](const typename Observable::value_type& changed)
             {
                 observable._on_value_change(observable, changed);
                 observable._on_change(observable);
             });             
    }
    
    template<typename Observable>
    static void watch_childs(Observable& observable)
    {
        for(auto& e : observable._container)
            watch_child(observable, e);
    }

    template<typename Observable>
    static void unwatch_childs(Observable& observable)
    {        
        for(auto& e : observable._container)
            e._on_change.disconnect(0);        
    }
};
    
template<typename T,
         typename Allocator,
         template <typename, typename> class Model_>
class vector<
    T,
    Allocator,
    Model_,
    typename std::enable_if<is_observable<T>::value>::type
>
{
    using container_t = Model_<T, Allocator>;
    container_t _container;
    
    boost::signals2::signal<bool(const vector<T, Allocator>&,
                                 typename container_t::const_iterator)>
    _before_erase;
    
    boost::signals2::signal<void(const vector<T, Allocator>&,
                                 typename container_t::const_iterator)>
    _after_erase, _after_insert;
        
    boost::signals2::signal<void(const vector<T, Allocator>&,
                                 const typename container_t::value_type&)>
    _on_value_change;
    
    boost::signals2::signal<void(const vector<T, Allocator>&)> _on_change;

    friend class parent_observable;
    
public:
    
    using Model = container_t;
    
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
    
    explicit vector(size_type n)
        : _container(n)
    {
        parent_observable::watch_childs(*this);
    }
    
    template<typename InputIterator>
    vector(InputIterator first, InputIterator last)
        : _container(std::distance(first, last))
    {
        std::move(first, last, _container.begin());
        parent_observable::watch_childs(*this);
    }
    
    vector(vector&& rhs)
        noexcept(std::is_nothrow_move_constructible<value_type>::value)
        : _container(std::move(rhs._container))
        , _before_erase(std::move(rhs._before_erase))
        , _after_erase(std::move(rhs._after_erase))
        , _after_insert(std::move(rhs._after_insert))
        , _on_value_change(std::move(rhs._on_value_change))
        , _on_change(std::move(rhs._on_change))
    {
        parent_observable::unwatch_childs(*this);
        parent_observable::watch_childs(*this);
    }

    vector& operator=(vector&& rhs)
        noexcept(std::is_nothrow_move_constructible<value_type>::value)
    {
        _container = std::move(rhs._container);
        _before_erase = std::move(rhs._before_erase);
        _after_erase = std::move(rhs._after_erase);
        _after_insert = std::move(rhs._after_insert);
        _on_value_change = std::move(rhs._on_value_change);
        _on_change = std::move(rhs._on_change);

        parent_observable::unwatch_childs(*this);
        parent_observable::watch_childs(*this);
        
        return *this;
    }

    iterator begin() noexcept
    { return _container.begin(); }

    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
    
    const_reverse_iterator crbegin() noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }    
    
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
    
    //TODO: notifications?
    void resize(size_type new_size)
    { _container.resize(new_size); }
    
    //TODO: notifications?
    void resize(size_type new_size, const value_type& v)
    { _container.resize(new_size, v); }
    
    reference at(size_type pos)
    { return _container.at(pos); }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }
    
    reference operator[](size_type pos) noexcept
    { return _container[pos]; }
    
    const_reference operator[](size_type pos) const noexcept
    { return _container[pos]; }

    reference front() noexcept
    { return _container.front(); }
        
    const_reference front() const noexcept
    { return _container.front(); }
    
    reference back() noexcept
    { return _container.back(); }
        
    const_reference back() const noexcept
    { return _container.back(); }
    
    bool clear() noexcept
    {
        if(_before_erase(*this, _container.cend()) == false)
            return false;
        _container.clear();
        _after_erase(*this,const_iterator{});
        _on_change(*this);
        return true;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    std::pair<iterator, bool> erase(iterator pos)        
    {
        if(_before_erase(*this, pos) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos);
        _after_erase(*this,it);
        _on_change(*this);
        return std::make_pair(it, true);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    std::pair<iterator, bool> erase(iterator first, iterator last)        
    {
        //TODO range?
        if(_before_erase(*this, first) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(first, last);
        _after_erase(*this,it);
        _on_change(*this);
        return std::make_pair(it, true);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args)
    {
        auto it = _container.emplace
            (pos, std::forward<Args>(args)...);
        parent_observable::watch_child(*this, *it);
        _after_insert(*this,it);
        _on_change(*this);
        return it;
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(iterator pos, value_type&& value)
    {
        auto it = _container.insert(pos, std::move(value));
        parent_observable::watch_child(*this, *it);
        _after_insert(*this,it);
        _on_change(*this);
        return it;
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(iterator pos, InputIt first, InputIt last)
    {
        difference_type offset{pos - begin()};
        auto beg_offset = offset;
        std::for_each(first, last,
                      [this, &offset](typename InputIt::value_type& e)
                      {
                          auto it = _container.insert
                              (begin() + offset, std::move(e));
                          ++offset;
                          parent_observable::watch_child(*this, *it);
                      });
        _after_insert(*this, begin() + beg_offset);
        _on_change(*this);
    }
        
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        parent_observable::watch_child(*this, _container.back());
        _after_insert(*this, std::prev(_container.end()));
        _on_change(*this);
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        parent_observable::watch_child(*this, _container.back());
        _after_insert(*this, std::prev(_container.end()));
        _on_change(*this);
    }
    
    void pop_back()
    {
        if(_before_erase(*this, std::prev(_container.rbegin().base())) == false)
            return;
        _container.pop_back();
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }
    
    void swap(container_t& other)
    {
        if(_before_erase(*this, _container.cend()) == false)
            return;
        _container.swap(other);
        //TODO: check?
        _after_insert(*this, const_iterator{});
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }

    const container_t& model() const noexcept
    { return _container; }

    template<typename F>
    boost::signals2::connection before_erase(F&& f)
    { return _before_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection after_erase(F&& f)
    { return _after_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection after_insert(F&& f)
    { return _after_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }    
};
    
template<typename T, typename Allocator>        
struct is_observable<vector<T, Allocator>> : std::true_type {};
    
}
