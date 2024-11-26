//
// Created by scros on 11/26/24.
//

#include "memento.hpp"

auto Memento::save() -> Snapshot
{
    Snapshot snapshot;

    return snapshot;
}

auto Memento::load(const Snapshot& state) -> void
{
    (void)state;
}
