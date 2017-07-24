#pragma once

namespace observable {

template<typename Observable>
class scoped_notifying
{
    Observable _observable;
public:    
    scoped_notifying(Observable o) : _observable(o)
    {
        _observable->_under_transaction = true;
    }
    
    ~scoped_notifying()
    {
        _observable->any_change();
        _observable->_under_transaction = false;
    }
};

template<typename Observable>
inline scoped_notifying<Observable>
make_scoped_notifying(Observable o)
{
    return scoped_notifying<Observable>(o);
}

}
