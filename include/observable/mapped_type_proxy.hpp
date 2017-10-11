
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

namespace observable {

template<typename Observable>    
class mapped_type_proxy
{
    Observable* _observable;
    typename Observable::Model::iterator _it;
public:
    mapped_type_proxy(Observable& observable, typename Observable::Model::iterator it)
        : _observable(&observable)
        , _it(it)
    {}
    
    template<typename U>
    mapped_type_proxy<Observable>& operator=(U&& rhs)
    {
        _it->second = std::forward<U>(rhs);
        _observable->_on_value_change(*_observable, *_it);
        _observable->_on_change(*_observable);
        return *this;
    }
    
    const typename Observable::Model::mapped_type& get() const noexcept
    { return _it->second; }

    const typename Observable::Model::mapped_type& operator*() const noexcept
    { return get(); }
    
    typename Observable::mapped_type extract()
    {
        auto o = std::move(_it->second); 
        _observable->_on_value_change(*_observable, *_it);
        _observable->_on_change(*_observable);        
        return o;
    }
};

template<typename Observable>
inline bool operator==(const mapped_type_proxy<Observable>& lhs,
                       const mapped_type_proxy<Observable>& rhs)
{ return lhs.get() == rhs.get(); }
    
template<typename Observable>
inline bool operator!=(const mapped_type_proxy<Observable>& lhs,
                       const mapped_type_proxy<Observable>& rhs)
{ return !(lhs == rhs); }
    
template<typename Observable>
inline bool operator==(const mapped_type_proxy<Observable>& lhs,
                       const typename Observable::Model::mapped_type& rhs)
{ return lhs.get() == rhs; }
    
template<typename Observable>
inline bool operator!=(const mapped_type_proxy<Observable>& lhs,
                       const typename Observable::Model::mapped_type& rhs)
{ return !(lhs == rhs); }
    
template<typename Observable>
inline bool operator==(const typename Observable::Mode::mapped_type& lhs,
                       const mapped_type_proxy<Observable>& rhs)
{ return lhs == rhs.get(); }
    
template<typename Observable>
inline bool operator!=(const typename Observable::Model::mapped_type& lhs,
                       const mapped_type_proxy<Observable>& rhs)
{ return !(lhs == rhs); }
    
}
