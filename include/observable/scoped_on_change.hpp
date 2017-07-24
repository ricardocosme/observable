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
        _observable.any_change();
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