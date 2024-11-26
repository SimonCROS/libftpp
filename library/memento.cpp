//
// Created by scros on 11/26/24.
//

#include "memento.hpp"

auto Memento::save() -> Snapshot
{
    Snapshot snapshot;
    _saveToSnapshot(snapshot);
    return snapshot;
}

auto Memento::load(const Snapshot& state) -> void
{
    Snapshot tmp = state; // _loadFromSnapshot should take a non-const snapshot so wee need to copy
    _loadFromSnapshot(tmp);
}
