
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/member/value.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>

namespace observable { namespace member {

struct vector_tag {};
    
template<typename Model, typename Tag,
         typename ObservableValueType = value<Model, Tag>>
struct vector
{
    using member_type = vector_tag;
    using model = Model;
    using tag = Tag;
    using observable_value_type = ObservableValueType;
};

template<typename Observable>    
class vector_iterator
    : public boost::iterator_adaptor<
        vector_iterator<Observable>,
        typename Observable::Model::iterator,
        std::shared_ptr<typename Observable::reference>,
        boost::bidirectional_traversal_tag,
        std::shared_ptr<typename Observable::reference>,
        typename Observable::Model::difference_type
    >
{
public:
    vector_iterator() : vector_iterator::iterator_adaptor_()
    {}

    explicit vector_iterator
    (Observable& observable, const typename Observable::Model::iterator& it)
        : vector_iterator::iterator_adaptor_(it)
        , _observable(observable)
    {}
private:
    friend class boost::iterator_core_access;
    
    std::shared_ptr<typename Observable::reference> dereference() const
    { return _observable.get_reference(this->base_reference()); }
    
    Observable& _observable;    
};

template<typename VectorImpl, typename ObservableValueType, typename Enable = void>
struct observable_value_type_impl;
        
template<typename VectorImpl, typename ObservableValueType>
struct observable_value_type_impl<
    VectorImpl,
    ObservableValueType,
    typename std::enable_if<
        std::is_same<
            typename ObservableValueType::member_type,
            value_tag
        >::value
    >::type
>    
{
    using type = value_impl<VectorImpl,
                            typename VectorImpl::Model::value_type, container_tag>;
};
        
template<typename VectorImpl, typename ObservableValueType>
struct observable_value_type_impl<
    VectorImpl,
    ObservableValueType,
    typename std::enable_if<
        !std::is_same<
            typename ObservableValueType::Tag2Member,
            void
        >::value
    >::type
>
{    
    using type = class_impl<VectorImpl,
                            typename ObservableValueType::Model,
                            ObservableValueType, container_tag>;
};
        
template<typename Class, typename Model_, typename ObservableValueType>
struct vector_impl
{
    using Model = Model_;
    using parent_t = Class;
    using reference = typename observable_value_type_impl
        <vector_impl<Class, Model, ObservableValueType>, ObservableValueType>::type;
    using iterator = vector_iterator<vector_impl<Class, Model, ObservableValueType>>;
    using difference_type = typename Model::difference_type;

    vector_impl() = default;
    
    vector_impl(parent_t& parent, Model& value)
        : _model(&value)
        , _parent(&parent)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _model->begin()); }

    boost::reverse_iterator<iterator> rbegin() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model->end()));
    }
    
    typename Model::const_iterator cbegin() const noexcept
    { return _model->cbegin(); }

    typename Model::const_reverse_iterator crbegin() noexcept
    { return _model->crbegin(); }
    
    iterator end() noexcept
    { return iterator(*this, _model->end()); }
    
    boost::reverse_iterator<iterator> rend() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model->begin()));
    }
    
    typename Model::const_iterator cend() const noexcept
    { return _model->cend(); }
    
    typename Model::const_reverse_iterator crend() noexcept
    { return _model->crend(); }
    
    bool empty() const noexcept
    { return _model->empty(); }
    
    typename Model::size_type size() const noexcept
    { return _model->size(); }
    
    typename Model::size_type max_size() const noexcept
    { return _model->max_size(); }

    void reserve(typename Model::size_type new_cap)
    { _model->reserve(new_cap); }
    
    typename Model::size_type capacity() const noexcept
    { return _model->capacity(); }
    
    void shrink_to_fit() const noexcept
    { return _model->shrink_to_fit(); }
    
    std::shared_ptr<reference> at(typename Model::size_type pos)
    {
        if (pos >= _model->size()) throw std::out_of_range("vector::at");
        auto it = _model->begin() + pos;
        return get_reference(it);
    }
    
    typename Model::const_reference at(typename Model::size_type pos) const
    { return _model->at(pos); }
    
    std::shared_ptr<reference> operator[](typename Model::size_type pos)
    {
        auto it = _model->begin() + pos;
        return get_reference(it);
    }
    
    typename Model::const_reference operator[]
    (typename Model::size_type pos) const
    { return (*_model)[pos]; }

    std::shared_ptr<reference> front()
    {
        auto it = _model->begin();
        return get_reference(it);
    }
        
    typename Model::const_reference front() const
    { return _model->front(); }
    
    std::shared_ptr<reference> back()
    {
        auto it = _model->end();
        --it;
        return get_reference(it);
    }
        
    typename Model::const_reference back() const
    { return _model->back(); }
    
    void clear() noexcept
    {
        _model->clear();
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, typename Model::const_iterator{});
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator pos)        
    {
        auto it = _model->erase(pos.base());
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator erase(iterator first, iterator last)        
    {
        auto it = _model->erase(first.base(), last.base());
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return iterator(*this, it);
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    template<typename... Args>
    iterator emplace(typename Model::iterator pos, Args&&... args)
    {
        auto it = _model->emplace(pos, std::forward<Args>(args)...);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return iterator(*this, it);
    }
    
    template <typename Container, typename ConstIterator>
    typename Container::iterator remove_constness(Container& c, ConstIterator it)
    {
        return c.erase(it, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename Model::iterator pos,
                    const typename Model::value_type& value)
    {
        auto it = _model->insert(pos, value);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return iterator(*this, it);
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first argument
    iterator insert(typename Model::iterator pos,
                    typename Model::value_type&& value)
    {
        auto it = _model->insert(pos, std::move(value));
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return iterator(*this, it);
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(typename Model::iterator pos,
                typename Model::size_type count,
                const typename Model::value_type& value)
    {
        _model->insert(pos, count, value);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, pos);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
        
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(typename Model::iterator pos, InputIt first, InputIt last)
    {
        _model->insert(pos, first, last);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, pos);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
        
    void insert(typename Model::iterator pos,
                std::initializer_list<typename Model::value_type> ilist)
    { insert(pos, ilist.begin(), ilist.end()); }

    void push_back(const typename Model::value_type& value)
    {
        _model->push_back(value);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, std::prev(_model->end()));
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
    
    void push_back(typename Model::value_type&& value)
    {
        _model->push_back(std::move(value));
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, std::prev(_model->end()));
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }

    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _model->emplace_back(std::forward<Args>(args)...);
        if (!_parent->_under_transaction)
        {
            _on_insert(*_model, std::prev(_model->end()));
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
    
    void pop_back()
    {
        _model->pop_back();
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, typename Model::const_iterator{});
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
    
    void swap(Model& other)
    {
        _model->swap(other);
        //TODO: check?
        _on_insert(*_model, typename Model::const_iterator{});
        _on_erase(*_model, typename Model::const_iterator{});
        _on_change(*_model);
        _parent->_on_change(*(_parent->_model));            
    }

    //TODO resize
    
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
    
    const Model& model() const noexcept
    { return *_model; }
    
    Model* _model;
    parent_t* _parent;
    
    boost::signals2::signal<void(const Model&, typename Model::const_iterator)>
    _on_erase, _on_insert, _on_value_change;
    
    boost::signals2::signal<void(const Model&)> _on_change;
    
    std::unordered_map<typename Model::const_iterator::pointer,
                       std::weak_ptr<reference>> _it2observable;
private:
    std::shared_ptr<reference> get_reference(typename Model::iterator it)
    {
        auto observable = _it2observable[&*it].lock();
        if (!observable)
        {
            auto& it2observable = _it2observable;
            auto it_ptr = &*it;
            observable = std::shared_ptr<reference>
                (new reference(*this, *it, it),
                 [&it2observable, it_ptr](reference *p)
                 {
                     it2observable.erase(it_ptr);
                     delete p;
                 });
            it2observable[&*it] = observable;
        }
        return observable;
    }
    friend class vector_iterator<vector_impl<Class, Model, ObservableValueType>>;
};
    
}}
