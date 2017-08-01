
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/class_value.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>

namespace observable {

struct vector_member_tag {};
    
template<typename T, typename Tag>
struct vector
{
    using tag = Tag;
    using type = T;
    using member_type = vector_member_tag;
};

template<typename Model>    
class vector_iterator
    : public boost::iterator_adaptor<
        vector_iterator<Model>,
        typename Model::type::iterator,
        typename Model::reference,
        boost::bidirectional_traversal_tag,
        typename Model::reference,
        typename Model::type::difference_type
    >
{
public:
    vector_iterator() : vector_iterator::iterator_adaptor_()
    {}

    explicit vector_iterator
    (Model& model, const typename Model::type::iterator& it)
        : vector_iterator::iterator_adaptor_(it)
        , _model(model)
    {}
private:
    friend class boost::iterator_core_access;
    using value = typename Model::reference;
    
    value dereference() const
    {
        auto it = this->base_reference();
        //TODO:lvalue ref?
        return typename Model::reference(_model, *it, it);
    }
    Model& _model;    
};
    
template<typename Class, typename Model>
struct vector_impl
{
    using type = Model;
    using parent_t = Class;
    using reference = value_impl<vector_impl<Class, Model>, typename type::value_type, container_tag>;
    using iterator = vector_iterator<vector_impl<Class, Model>>;
    using difference_type = typename type::difference_type;
    
    vector_impl(parent_t& parent, type& value)
        : _model(value)
        , _parent(parent)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _model.begin()); }

    boost::reverse_iterator<iterator> rbegin() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model.end()));
    }
    
    typename type::const_iterator cbegin() const noexcept
    { return _model.cbegin(); }

    typename type::const_reverse_iterator crbegin() noexcept
    { return _model.crbegin(); }
    
    iterator end() noexcept
    { return iterator(*this, _model.end()); }
    
    boost::reverse_iterator<iterator> rend() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model.begin()));
    }
    
    typename type::const_iterator cend() const noexcept
    { return _model.cend(); }
    
    typename type::const_reverse_iterator crend() noexcept
    { return _model.crend(); }
    
    bool empty() const noexcept
    { return _model.empty(); }
    
    typename type::size_type size() const noexcept
    { return _model.size(); }
    
    typename type::size_type max_size() const noexcept
    { return _model.max_size(); }

    void reserve(typename type::size_type new_cap)
    { _model.reserve(new_cap); }
    
    typename type::size_type capacity() const noexcept
    { return _model.capacity(); }
    
    void shrink_to_fit() const noexcept
    { return _model.shrink_to_fit(); }
    
    reference at(typename type::size_type pos)
    {
        if (pos >= _model.size()) throw std::out_of_range("vector::at");
        auto it = _model.begin() + pos;
        return reference(*this, *it, it);
    }
    
    const typename type::const_reference& at(typename type::size_type pos) const
    { return _model.at(pos); }
    
    reference operator[](typename type::size_type pos)
    {
        auto it = _model.begin() + pos;
        return reference(*this, *it, it);
    }
    
    const typename type::const_reference& operator[]
    (typename type::size_type pos) const
    { return _model[pos]; }

    reference front()
    {
        auto it = _model.begin();
        return reference(*this, *it, it);
    }
        
    const typename type::const_reference front() const
    { return _model.front(); }
    
    reference back()
    {
        auto it = _model.end();
        --it;
        return reference(*this, *it, it);
    }
        
    const typename type::const_reference back() const
    { return _model.back(); }
    
    void clear() noexcept
    {
        _model.clear();
        if (!_parent._under_transaction)
        {
            _on_erase(_model, typename type::const_iterator{});
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator pos)        
    {
        auto it = _model.erase(pos.base());
        if (!_parent._under_transaction)
        {
            _on_erase(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator first, iterator last)        
    {
        auto it = _model.erase(first.base(), last.base());
        if (!_parent._under_transaction)
        {
            _on_erase(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return iterator(*this, it);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(typename type::iterator pos, Args&&... args)
    {
        auto it = _model.emplace(pos, std::forward<Args>(args)...);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return iterator(*this, it);
    }
    
    template <typename Container, typename ConstIterator>
    typename Container::iterator remove_constness(Container& c, ConstIterator it)
    {
        return c.erase(it, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename type::iterator pos,
                    const typename type::value_type& value)
    {
        auto it = _model.insert(pos, value);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename type::iterator pos,
                    typename type::value_type&& value)
    {
        auto it = _model.insert(pos, std::move(value));
        if (!_parent._under_transaction)
        {
            _on_insert(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return iterator(*this, it);
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(typename type::iterator pos,
                typename type::size_type count,
                const typename type::value_type& value)
    {
        _model.insert(pos, count, value);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, pos);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(typename type::iterator pos, InputIt first, InputIt last)
    {
        _model.insert(pos, first, last);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, pos);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
        
    void insert(typename type::iterator pos,
                std::initializer_list<typename type::value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    void push_back(const typename type::value_type& value)
    {
        _model.push_back(value);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, std::prev(_model.end()));
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
    
    void push_back(typename type::value_type&& value)
    {
        _model.push_back(std::move(value));
        if (!_parent._under_transaction)
        {
            _on_insert(_model, std::prev(_model.end()));
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _model.emplace_back(std::forward<Args>(args)...);
        if (!_parent._under_transaction)
        {
            _on_insert(_model, std::prev(_model.end()));
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
    
    void pop_back()
    {
        _model.pop_back();
        if (!_parent._under_transaction)
        {
            _on_erase(_model, typename type::const_iterator{});
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
    
    void swap(type& other)
    {
        _model.swap(other);
        //TODO: check?
        _on_insert(_model, typename type::const_iterator{});
        _on_erase(_model, typename type::const_iterator{});
        _on_change(_model);
        _parent._on_change(_parent._model);            
    }

    //TODO resize
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    {
        return _on_erase.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    {
        return _on_insert.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    {
        return _on_value_change.connect(std::forward<F>(f));
    }
    
    const Model& model() const noexcept
    { return _model; }
    
    type& _model;
    parent_t& _parent;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_erase;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_insert;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_value_change;
    boost::signals2::signal<void(const type&)> _on_change;
};
    
}
