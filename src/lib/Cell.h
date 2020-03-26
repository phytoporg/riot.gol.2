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

