//
// The main board state abstraction for the Game of Life.
//

#pragma once

#include "Cell.h"
#include "CellStorage.h"

#include <vector>

namespace gol
{
    class GOLGrid
    {
    public:
        //
        // cellAddresses lists all live cells which describes the initial state.
        //
        GOLGrid(const std::vector<CellAddress>& cellAddresses);

        //
        // Advance the generation by one single iteration.
        //
        void AdvanceGeneration();

        //
        // Retrieve the live cells for testing, output and debugging. The
        // returned data results from a deep copy of the internals.
        //
        std::vector<Cell> GetLiveCells() const;

    private:
        //
        // The current state bounces back and forth between these two.
        //
        CellStorage  m_pingPongStorage[2];

        //
        // Swapped on every generation.
        //
        CellStorage* m_pCurrentStorage;
        CellStorage* m_pNextStorage;
    };
}

