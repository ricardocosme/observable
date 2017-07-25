
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace observable {

template<typename Observable>
class scoped_on_change_t
{
    Observable& _observable;
public:    
    scoped_on_change_t(Observable& o) : _observable(o)
    {
        _observable._under_transaction = true;
    }
    
    ~scoped_on_change_t()
    {
        _observable.any_change(_observable.model());
        _observable._under_transaction = false;
    }
};

template<typename Observable>
inline scoped_on_change_t<Observable>
scoped_on_change(Observable& o)
{
    return scoped_on_change_t<Observable>(o);
}

}
