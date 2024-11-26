//
// Created by scros on 11/26/24.
//

#ifndef MEMENTO_HPP
#define MEMENTO_HPP
#include "data_buffer.hpp"

class Memento
{
public:
    using Snapshot = DataBuffer; // Maybe create a class and forward operator calls to DataBuffer

    [[nodiscard]] auto save() -> Snapshot;
    auto load(const Snapshot& state) -> void;

private:
    virtual auto _saveToSnapshot(Snapshot& snapshotToFill) -> void = 0;
    virtual auto _loadFromSnapshot(Snapshot& snapshot) -> void = 0;
};

#endif //MEMENTO_HPP
