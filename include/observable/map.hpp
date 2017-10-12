
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"
#include "observable/proxy.hpp"
#include "observable/mapped_parent_observable.hpp"
#include "observable/mapped_type_proxy.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>

namespace observable {
    
namespace serialization {
    struct map;
}
    
template<typename Key,
         typename T,
         typename Compare = std::less<Key>,
         typename Allocator = std::allocator<std::pair<const Key, T>>,
         template <typename, typename, typename, typename> class Model_ = std::map,
         typename Enable = void>
class map;
        
template<typename Observable>    
class map_iterator
    : public boost::iterator_adaptor<
        map_iterator<Observable>,
        typename Observable::Model::iterator,
        std::pair<
            typename Observable::Model::key_type,
            mapped_type_proxy<Observable>
        >,
        boost::bidirectional_traversal_tag,
        std::pair<
            typename Observable::Model::key_type,
            mapped_type_proxy<Observable>
        >,
        typename Observable::Model::difference_type
    >
{
public:
    map_iterator() = default;

    explicit map_iterator
    (Observable& observable, typename Observable::Model::iterator it)
        : map_iterator::iterator_adaptor_(it)
        , _observable(&observable)
    {}
private:
    friend class boost::iterator_core_access;
    
    std::pair<
        typename Observable::Model::key_type,
        mapped_type_proxy<Observable>
        >
    dereference() const
    {
        auto it = this->base_reference();
        return std::make_pair(it->first,
                              mapped_type_proxy<Observable>
                              {*_observable, this->base_reference()});
    }
    
    Observable* _observable{nullptr};    
};
    
template<typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model_>
class map<
    Key,
    T,
    Compare,
    Allocator,
    Model_,
    typename std::enable_if<!is_observable<T>::value>::type
>
{
    using container_t = Model_<Key, T, Compare, Allocator>;
    container_t _container;
    
    boost::signals2::signal<void(const map<Key, T, Compare, Allocator, Model_>&,
                                 typename container_t::const_iterator)>
    _after_erase, _after_insert;
    
    boost::signals2::signal<
        bool(const map<Key, T, Compare, Allocator, Model_>&,
             typename container_t::const_iterator)>
    _before_erase;
    
    boost::signals2::signal<
        void(const map<Key, T, Compare, Allocator, Model_>&,
             const typename container_t::value_type&)>
    _on_value_change;
    
    boost::signals2::signal<
        void(const map<Key, T, Compare, Allocator, Model_>&)> _on_change;

    friend class proxy<map<Key, T, Compare, Allocator, Model_>>;
    friend class mapped_type_proxy<map<Key, T, Compare, Allocator, Model_>>;
    friend struct serialization::map;
public:
    
    using Model = container_t;
    
    using key_type = typename container_t::key_type;
    using mapped_type = typename container_t::mapped_type;
    using value_type = typename container_t::value_type;
    using key_compare = typename container_t::key_compare;
    using reference = std::pair<
        key_type,
        mapped_type_proxy<
            map<Key, T, Compare, Allocator, Model_>>
        >;
    using const_reference = typename container_t::const_reference;
    using pointer = reference*;
    using const_pointer = typename container_t::const_pointer;
    using iterator = map_iterator<map<Key, T, Compare, Allocator, Model_>>;
    using reverse_iterator = boost::reverse_iterator<iterator>;
    using const_iterator = typename container_t::const_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    map() = default;

    template<typename InputIterator>
    map(InputIterator first, InputIterator last)
        : _container(first, last)
    {}
    
    map(std::initializer_list<value_type> l)
        : _container(std::move(l))
    {}
    
    map(map&& rhs)
        noexcept(std::is_nothrow_move_constructible<value_type>::value)
        : _container(std::move(rhs._container))
        , _after_erase(std::move(rhs._after_erase))
        , _after_insert(std::move(rhs._after_insert))
        , _before_erase(std::move(rhs._before_erase))
        , _on_value_change(std::move(rhs._on_value_change))
        , _on_change(std::move(rhs._on_change))
    {        
    }
    
    map& operator=(map&& rhs)
        noexcept(std::is_nothrow_move_assignable<value_type>::value)
    {
        _container = std::move(rhs._container);
        _after_erase = std::move(rhs._after_erase);
        _after_insert = std::move(rhs._after_insert);
        _before_erase = std::move(rhs._before_erase);
        _on_value_change = std::move(rhs._on_value_change);
        _on_change = std::move(rhs._on_change);
        return *this;
    }
    
    iterator begin() noexcept
    { return iterator{*this, _container.begin()}; }

    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    reverse_iterator rbegin() noexcept
    { return reverse_iterator{iterator{*this, _container.end()}}; }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    const_reverse_iterator crbegin() const noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return iterator{*this, _container.end()}; }
    
    const_iterator end() const noexcept
    { return _container.end(); }
    
    reverse_iterator rend() noexcept
    { return reverse_iterator{iterator{*this, _container.begin()}}; }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    const_reverse_iterator crend() const noexcept
    { return _container.crend(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }

    mapped_type_proxy<map<Key, T, Compare, Allocator, Model_>>
    at(const key_type& key)
    {
        auto it = _container.lower_bound(key);
        if (it == _container.end() || _container.key_comp()(key, (*it).first))
            throw std::out_of_range("map::at");
        return {*this, it};
    }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
    
    mapped_type_proxy<map<Key, T, Compare, Allocator, Model_>>
    operator[](const key_type& key)
    {
        auto it = _container.lower_bound(key);
        if (it == _container.end() || _container.key_comp()(key, (*it).first))
            it = _container.insert(it, value_type(key, mapped_type()));
        return {*this, it};
    }
    
    mapped_type_proxy<map<Key, T, Compare, Allocator, Model_>>
    operator[](key_type&& key)
    {
        auto it = _container.lower_bound(key);
        if (it == _container.end() || _container.key_comp()(key, (*it).first))
            it = _container.insert(it, value_type(std::move(key), mapped_type()));
        return {*this, it};
    }
    
    bool clear() noexcept
    {
        if(_before_erase(*this, _container.cend()) == false)
            return false;
        _container.clear();
        _after_erase(*this, _container.cend());
        _on_change(*this);
        return true;
    }
    
    std::pair<iterator, bool> erase(const_iterator pos)        
    {
        if(_before_erase(*this, pos) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos);
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(iterator{*this, it}, true);
    }
    
    std::pair<iterator, bool> erase(iterator pos)        
    {
        if(_before_erase(*this, pos.base()) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos.base());
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(iterator{*this, it}, true);
    }
    
    std::pair<iterator, bool> erase(const_iterator first,
                                    const_iterator last)        
    {
        //TODO range?
        if(_before_erase(*this, first) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(first, last);
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(iterator{*this, it}, true);
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto before_size = _container.size();
        _before_erase(*this, rng.first);
        _container.erase(rng.first, rng.second);
        auto n = before_size - _container.size();
        if (n > 0)
        {
            _after_erase(*this, rng.second);
            _on_change(*this);
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _container.emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
        }
        return std::make_pair(iterator{*this, ret.first}, ret.second);
    }
    
    template<typename... Args>
    iterator emplace_hint
    (const_iterator hint, Args&&... args)
    {
        auto before_size = _container.size();
        auto it = _container.emplace_hint(hint, std::forward<Args>(args)...);
        if (_container.size() != before_size)
        {
            _after_insert(*this, it);
            _on_change(*this);
        }
        return iterator{*this, it};
    }

    std::pair<iterator, bool> insert
    (const value_type& value)
    {
        auto ret = _container.insert(value);
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
        }
        return std::make_pair(iterator{*this, ret.first}, ret.second);
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
        }
        return std::make_pair(iterator{*this, ret.first}, ret.second);
    }
    
    iterator insert(const_iterator hint,
                    const value_type& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, value);
        if (_container.size() != before_size)
        {
            _after_insert(*this, it);
            _on_change(*this);
        }
        return iterator{*this, it};
    }
    
    iterator insert(iterator hint,
                    const value_type& value)
    { return insert(hint.base(), value); }
    
    iterator insert(iterator hint,
                    value_type&& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint.base(), std::move(value));
        if (_container.size() != before_size)
        {
            _after_insert(*this, it);
            _on_change(*this);
        }
        return iterator{*this, it};
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _container.size();
        _container.insert(first, last);
        if (_container.size() != before_size)
        {
            _after_insert(*this, _container.cend()); //TODO
            _on_change(*this);
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    void swap(container_t& other)
    {
        _before_erase(*this, _container.cend());
        _container.swap(other);
        //TODO: check?
        _after_insert(*this, const_iterator{});
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }

    size_type count
    (const key_type& key) const noexcept
    { return _container.count(key); }
    
    iterator find(const key_type& key)
    { return iterator{*this, _container.find(key)}; }
    
    const_iterator find
    (const key_type& key) const
    { return _container.find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    {
        auto ret = _container.equal_range(key);
        return std::make_pair(iterator{*this, ret.first},
                              iterator{*this, ret.second});
    }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
    
    iterator lower_bound(const key_type& key)
    { return iterator{*this, _container.lower_bound(key)}; }
    
    const_iterator lower_bound(const key_type& key) const
    { return _container.lower_bound(key); }
    
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
    
template<typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model_>
class map<
    Key,
    T,
    Compare,
    Allocator,
    Model_,
    typename std::enable_if<is_observable<T>::value>::type
>
{
    using container_t = Model_<Key, T, Compare, Allocator>;
    container_t _container;
    
    boost::signals2::signal<void(const map<Key, T, Compare, Allocator, Model_>&,
                                 typename container_t::const_iterator)>
    _after_erase, _after_insert;
    
    boost::signals2::signal<
        bool(const map<Key, T, Compare, Allocator, Model_>&,
             typename container_t::const_iterator)>
    _before_erase;
    
    boost::signals2::signal<
        void(const map<Key, T, Compare, Allocator, Model_>&,
             const typename container_t::value_type&)>
    _on_value_change;
    
    boost::signals2::signal<
        void(const map<Key, T, Compare, Allocator, Model_>&)> _on_change;

    friend class mapped_parent_observable;
    friend struct serialization::map;
    
public:

    using Model = container_t;
    
    using key_type = typename container_t::key_type;
    using mapped_type = typename container_t::mapped_type;
    using value_type = typename container_t::value_type;
    using key_compare = typename container_t::key_compare;
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

    map() = default;

    template<typename InputIterator>
    map(InputIterator first, InputIterator last)
        : _container(first, last)
    {}
    
    map(std::initializer_list<value_type> l)
        : _container(std::move(l))
    {}
    
    map(map&& rhs)
        noexcept(std::is_nothrow_move_constructible<value_type>::value)
        : _container(std::move(rhs._container))
        , _after_erase(std::move(rhs._after_erase))
        , _after_insert(std::move(rhs._after_insert))
        , _before_erase(std::move(rhs._before_erase))
        , _on_value_change(std::move(rhs._on_value_change))
        , _on_change(std::move(rhs._on_change))
    {
        mapped_parent_observable::unwatch_childs(*this);
        mapped_parent_observable::watch_childs(*this);
    }

    map& operator=(map&& rhs)
        noexcept(std::is_nothrow_move_constructible<value_type>::value)
    {
        _container = std::move(rhs._container);
        _after_erase = std::move(rhs._after_erase);
        _after_insert = std::move(rhs._after_insert);
        _before_erase = std::move(rhs._before_erase);
        _on_value_change = std::move(rhs._on_value_change);
        _on_change = std::move(rhs._on_change);

        mapped_parent_observable::unwatch_childs(*this);
        mapped_parent_observable::watch_childs(*this);
        
        return *this;
    }
    
    iterator begin() noexcept
    { return _container.begin(); }

    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    const_reverse_iterator crbegin() const noexcept
    { return _container.crbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    const_iterator end() const noexcept
    { return _container.end(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
        
    const_reverse_iterator crend() const noexcept
    { return _container.crend(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }

    mapped_type& at(const key_type& key)
    { return _container.at(key); }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
    
    mapped_type& operator[](const key_type& key)
    { return _container[key]; }
    
    mapped_type& operator[](key_type&& key)
    { return _container[std::move(key)]; }
    
    bool clear() noexcept
    {
        if(_before_erase(*this, _container.cend()) == false)
            return false;
        _container.clear();
        _after_erase(*this, _container.cend());
        _on_change(*this);
        return true;
    }
    
    std::pair<iterator, bool> erase(const_iterator pos)        
    {
        if(_before_erase(*this, pos) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos);
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(it, true);
    }
    
    std::pair<iterator, bool> erase(iterator pos)        
    {
        if(_before_erase(*this, pos) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(pos);
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(it, true);
    }
    
    std::pair<iterator, bool> erase(const_iterator first,
                                    const_iterator last)        
    {
        //TODO range?
        if(_before_erase(*this, first) == false)
            return std::make_pair(end(), false);
        auto it = _container.erase(first, last);
        _after_erase(*this, it);
        _on_change(*this);
        return std::make_pair(it, true);
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto before_size = _container.size();
        _before_erase(*this, rng.first);
        _container.erase(rng.first, rng.second);
        auto n = before_size - _container.size();
        if (n > 0)
        {
            _after_erase(*this, rng.second);
            _on_change(*this);
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _container.emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
            mapped_parent_observable::watch_child(*this, *ret.first);
        }
        return ret;
    }
    
    template<typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        auto before_size = _container.size();
        auto it = _container.emplace_hint(hint, std::forward<Args>(args)...);
        if (_container.size() != before_size)
        {
            _after_insert(*this, it);
            _on_change(*this);
            mapped_parent_observable::watch_child(*this, *it);
        }
        return it;
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
            mapped_parent_observable::watch_child(*this, *ret.first);
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
            _after_insert(*this, it);
            _on_change(*this);
            mapped_parent_observable::watch_child(*this, *it);
        }
        return it;
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _container.size();
        std::for_each(first, last,
                      [this](typename InputIt::value_type& e)
                      {
                          auto ret = _container.insert(std::move(e));
                          mapped_parent_observable::watch_child
                              (*this, *ret.first);
                      });
        if (_container.size() != before_size)
        {
            _after_insert(*this, _container.cend()); //TODO
            _on_change(*this);
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    void swap(container_t& other)
    {
        _before_erase(*this, _container.cend());
        _container.swap(other);
        //TODO: check?
        _after_insert(*this, const_iterator{});
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }

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

    iterator lower_bound(const key_type& key)
    { return _container.lower_bound(key); }
    
    const_iterator lower_bound(const key_type& key) const
    { return _container.lower_bound(key); }
    
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

template<typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model>
bool operator==(const map<Key, T, Compare, Allocator, Model>& lhs,
                const map<Key, T, Compare, Allocator, Model>& rhs)
{
    return lhs.model() == rhs.model();
}
    
template<typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model>
bool operator!=(const map<Key, T, Compare, Allocator, Model>& lhs,
                const map<Key, T, Compare, Allocator, Model>& rhs)
{
    return !(lhs == rhs);
}

template<typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model_>
struct is_observable<map<Key, T, Compare, Allocator, Model_>> : std::true_type {};
    
}
