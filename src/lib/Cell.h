//
// Individual cell state.
//

#pragma once

#include <utility>
#include <cstdint>

namespace gol
{
    using CellAddress = std::pair<int64_t, int64_t>;

    struct Cell
    {
        Cell() = default;
        Cell(const CellAddress& address, bool alive, uint8_t neighborCount)
            : Address(address), State{alive, neighborCount} {}
        Cell(const Cell& other) = default;

        CellAddress Address;

        struct
        {
            uint8_t Alive;
            uint8_t NeighborCount;
        } State;

        //
        // May be overkill? Packing state bits into place for an easy state
        // transition look-up when advancing the generation.
        //
        uint8_t LookupKey() const
        {
            return ((State.Alive << 4) | (State.NeighborCount)) & 0x1F;
        };
    };
}

