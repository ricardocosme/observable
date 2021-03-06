
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "person.hpp"

#include <observable/class.hpp>
#include <observable/unordered_set.hpp>
#include <observable/map.hpp>
#include <observable/observable_class_gen.hpp>

#include <cstddef>
#include <string>

/// This source provides a non intrusive observable proxy to a model
/// of person. The `observable_person` is a proxy to an object of
/// `person_t` that permits the programmer to manipulate the object
/// with a straightforward support to notifications linked with
/// actions that changes the object.  The observable::class_ has
/// signals to actions like on_change of an object or on_insert,
/// on_erase or on_value_change of a container. The signals can be
/// emitted under the hood through the observable`s API. Slots may be
/// connected to these signals to react with some specific action,
/// like an observer that registers your interest in an observable
/// object.
///
/// This approach is non intrusive because the model lives without any
/// notion of signals or whatever. The observable object manipulates
/// the model through a reference and the there is no restriction to
/// bypass the observable and change the model without the
/// observable. 

/// Generate an observable class of person. This class must be used to
/// manipulate the model(person_t) with support to notifications to
/// changes.
/// 
/// Note: The macro also generates tag types to identify the class
/// members.
OBSERVABLE_CLASS_GEN(
    observable_person, //The observable class to be generated
    person_t, //The model's type
    ((std::string, name))
    ((std::size_t, age))
    
    /// Observable of containers don't store anything. It's only a
    /// proxy that behaves like a container to manipulate the
    /// container(model) with support to notifications. When an
    /// element is fetched through the observable, an observable of
    /// the element is returned. Note: The ownership of the observable
    /// of the element is shared. The container observable only
    /// store a weak pointer.
    ((skills_t, skills))
    ((kids_t, kids))
);

/// The factory function is responsible to say how the
/// `observable_person` can be constructed. 
inline observable_person observable_factory(person_t& model)
{
    return observable_person(model, model.name, model.age,
                             model.skills, model.kids);
}
