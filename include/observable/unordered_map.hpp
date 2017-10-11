
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/mapped_type_proxy.hpp"
#include "observable/is_observable.hpp"
#include "observable/proxy.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <unordered_map>

namespace observable {

template<typename Key,
         typename T,
         typename Hash = std::hash<Key>,
         typename KeyEqual = std::equal_to<Key>,
         typename Allocator = std::allocator<std::pair<const Key, T>>,
         template <typename, typename, typename,
                   typename, typename> class Model_ = std::unordered_map,
         typename Enable = void
         >
class unordered_map;
        
template<typename Observable>    
class unordered_map_iterator
    : public boost::iterator_adaptor<
        unordered_map_iterator<Observable>,
        typename Observable::Model::iterator,
        std::pair<
            typename Observable::Model::key_type,
            mapped_type_proxy<Observable>
        >,
        boost::forward_traversal_tag,
        std::pair<
            typename Observable::Model::key_type,
            mapped_type_proxy<Observable>
        >,
        typename Observable::Model::difference_type
    >
{
public:
    unordered_map_iterator() = default;

    explicit unordered_map_iterator
    (Observable& observable, typename Observable::Model::iterator it)
        : unordered_map_iterator::iterator_adaptor_(it)
        , _observable(&observable)
    {}
private:
    friend class boost::iterator_core_access;
    
    std::pair<
        typename Observable::Model::key_type,
        mapped_type_proxy<Observable>
    > dereference() const
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
         typename Hash,
         typename KeyEqual,
         typename Allocator,
         template <typename, typename, typename,
                   typename, typename> class Model_
         >
class unordered_map<
    Key,
    T,
    Hash,
    KeyEqual,
    Allocator,
    Model_,
    typename std::enable_if<!is_observable<T>::value>::type
    >
{
    using container_t = Model_<Key, T, Hash, KeyEqual, Allocator>;
    container_t _container;
    
    boost::signals2::signal<
        bool(const unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>&,
             typename container_t::const_iterator)>
    _before_erase;
    
    boost::signals2::signal<
        void(const unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>&,
             typename container_t::const_iterator)>
    _after_erase, _after_insert;
    
    boost::signals2::signal<
        void(const unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>&,
             const typename container_t::value_type&)>
    _on_value_change;
    
    boost::signals2::signal<
        void(const unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>&)>
    _on_change;

    friend class mapped_type_proxy<
        unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>
    >;
    
public:
    
    using Model = container_t;
    
    using key_type = typename container_t::key_type;
    using mapped_type = typename container_t::mapped_type;
    using value_type = typename container_t::value_type;
    using key_equal = typename container_t::key_equal;
    using hasher = typename container_t::hasher;
    using reference = std::pair<
        key_type,
        mapped_type_proxy<
            unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>>
        >;
    using const_reference = typename container_t::const_reference;
    using pointer = reference*;
    using const_pointer = typename container_t::const_pointer;
    using iterator = unordered_map_iterator<
        unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>>;
    using const_iterator = typename container_t::const_iterator;
    //TODO local_iterator
    using const_local_iterator = typename container_t::const_local_iterator;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using allocator_type = typename container_t::allocator_type;

    unordered_map() = default;
    
    explicit unordered_map(size_type n)
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
    { return iterator{*this, _container.begin()}; }

    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    iterator end() noexcept
    { return iterator{*this, _container.end()}; }
    
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
        return std::make_pair(iterator{*this, ret.first}, ret.second);;
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
        }
        return iterator{*this, it};
    }
    
    std::pair<iterator, bool> insert(const value_type& value)
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
    
    iterator insert(const_iterator hint,
                    value_type&& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, std::move(value));
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
            _after_insert(*this, const_iterator{}); //TODO
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
    
    void rehash(size_type n)
    { return _container.rehash(n); }
    
    void reserve(size_type n)
    { return _container.reserve(n); }
    
    mapped_type_proxy<
        unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>
    >
    at(const key_type& key)
    {
        auto it = _container.find(key);
        if(it == _container.end())
            throw std::out_of_range("vector::at");            
        return {*this, it};
    }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
    
    mapped_type_proxy<
        unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>
    >
    operator[](const key_type& key)
    {
        auto it = _container.find(key);
        if(it == _container.end())
            _container.emplace(key, mapped_type{});
        return {*this, it};
    }
    
    mapped_type_proxy<
        unordered_map<Key, T, Hash, KeyEqual, Allocator, Model_>
    >
    operator[](key_type&& key)
    {
        auto it = _container.find(key);
        if(it == _container.end())
            _container.emplace(std::move(key), mapped_type{});
        return {*this, it};
    }
        
    size_type count(const key_type& key) const
    { return _container.count(key); }
    
    iterator find(const key_type& key)
    { return iterator{*this, _container.find(key)}; }
    
    const_iterator find(const key_type& key) const
    { return _container.find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    { return _container.equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
                        
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
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }
    
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
    
    boost::signals2::signal<void(const unordered_map<Key, T>&,
                                 typename container_t::const_iterator)>
    _after_erase, _after_insert, _before_erase, _on_value_change;
    
    boost::signals2::signal<void(const unordered_map<Key, T>&)> _on_change;

    std::unordered_map<
        typename container_t::const_pointer,
        boost::signals2::scoped_connection            
    > _ptr2value;
    
    void insert_value_change_conn(typename container_t::value_type& e)
    {
        _ptr2value.emplace
            (&e,
             e.second.on_change
             ([this, &e](const typename container_t::mapped_type&)
              {
                  auto it = _container.find(e.first);
                  assert(it != _container.end());
                  _on_value_change(*this, it);
                  _on_change(*this);
              }));             
    }
    
public:
    
    using Model = container_t;
    
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
    
    unordered_map(unordered_map&& rhs)
        : _container(std::move(rhs._container))
        , _after_erase(std::move(rhs._after_erase))
        , _after_insert(std::move(rhs._after_insert))
        , _before_erase(std::move(rhs._before_erase))
        , _on_value_change(std::move(rhs._on_value_change))
        , _on_change(std::move(rhs._on_change))
    {
        rhs._ptr2value.clear();
        for(auto& e : _container)
            insert_value_change_conn(e);
    }

    unordered_map& operator=(unordered_map&& rhs)
    {
        _container = std::move(rhs._container);
        _after_erase = std::move(rhs._after_erase);
        _after_insert = std::move(rhs._after_insert);
        _before_erase = std::move(rhs._before_erase);
        _on_value_change = std::move(rhs._on_value_change);
        _on_change = std::move(rhs._on_change);

        rhs._ptr2value.clear();
        for(auto& e : _container)
            insert_value_change_conn(e);
        return *this;
    }
    
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
        _before_erase(*this, _container.cend());
        _container.clear();
        _after_erase(*this, _container.cend());
        _on_change(*this);
    }
    
    std::pair<iterator, bool> insert
    (const value_type& value)
    {
        auto ret = _container.insert(value);
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
            insert_value_change_conn(*ret.first);
        }
        return ret;
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
        {
            _after_insert(*this, ret.first);
            _on_change(*this);
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
            _after_insert(*this, it);
            _on_change(*this);
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
            _after_insert(*this, const_iterator{}); //TODO
            _on_change(*this);
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
            _after_insert(*this, ret.first);
            _on_change(*this);
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
            _after_insert(*this, it);
            _on_change(*this);
            insert_value_change_conn(*it);
        }
        return it;
    }
    
    iterator erase(const_iterator pos)        
    {
        _before_erase(*this, pos);
        _ptr2value.erase(&*pos);
        auto it = _container.erase(pos);
        _after_erase(*this, it);
        _on_change(*this);
        return it;
    }
    
    iterator erase(const_iterator first,
                   const_iterator last)        
    {
        //TODO range?
        _before_erase(*this, first);
        std::for_each(first, last,
                      [this](const value_type& e)
                      { _ptr2value.erase(&e); });
        auto it = _container.erase(first, last);
        _after_erase(*this, it);
        _on_change(*this);
        return it;
    }
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto before_size = _container.size();
        _ptr2value.erase(&*rng.first);
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
    
    void swap(container_t& other)
    {
        _before_erase(*this, _container.cend());
        _container.swap(other);
        //TODO: check?
        _after_insert(*this, const_iterator{});
        _after_erase(*this, const_iterator{});
        _on_change(*this);
    }

    void rehash(size_type n)
    { return _container.rehash(n); }
    
    void reserve(size_type n)
    { return _container.reserve(n); }
    
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
    
template<typename Key, typename T>
struct is_observable<unordered_map<Key, T>> : std::true_type {};
    
}
