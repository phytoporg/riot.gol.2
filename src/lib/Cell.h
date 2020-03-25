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

        struct
        {
            uint8_t NeighborCount : 4;
            uint8_t Alive         : 1;
        } State;
    };

    struct CellCompare
    {
        using is_transparent = std::true_type;
        bool operator()(const Cell& a, const Cell& b) const
        {
            return a.Address < b.Address;
        }

        bool operator()(const CellAddress& a, const CellAddress& b) const
        {
            return a < b;
        }

        bool operator()(const CellAddress& a, const Cell& b) const
        {
            return a < b.Address;
        }

        bool operator()(const Cell& a, const CellAddress& b) const
        {
            return a.Address < b;
        }
    };
}

