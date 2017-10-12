
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace observable {
    
struct mapped_parent_observable
{
    template<typename Observable>
    static void watch_child(Observable& observable,
                            typename Observable::value_type& e)
    {
        e.second._on_change.connect
            (0, 
             [&observable, &e](const typename Observable::mapped_type& changed)
             {
                 observable._on_value_change(observable, e);
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
            e.second._on_change.disconnect(0);        
    }
};
    
}
