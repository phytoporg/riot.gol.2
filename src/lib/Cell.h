//
// Individual cell state.
//

#pragma once

#include <utility>
#include <cstdint>

namespace gol
{
    //
    // For address + offset type logic.
    //
    using CellAddress = std::pair<int64_t, int64_t>;
    CellAddress operator+(const CellAddress& a, const CellAddress& b)
    {
        return std::make_pair<int64_t, int64_t>(
                a.first + b.first,
                a.second + b.second
            );
    }

    struct Cell
    {
        Cell(const CellAddress& address, bool alive, uint8_t neighborCount)
            : Address(address), State{alive, neighborCount} {}

        CellAddress Address;

        //
        // May be overkill. Packing these bits into place for an easy state
        // transition look-up when advancing the generation.
        //
        struct
        {
            uint8_t Alive;
            uint8_t NeighborCount;
        } State;

        uint8_t LookupKey() const
        {
            return ((State.Alive << 4) | (State.NeighborCount)) & 0x1F;
        };
    };
}

