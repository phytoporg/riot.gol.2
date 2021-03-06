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
        // Retrieve cells for testing, output and debugging. The returned data 
        // results from a deep copy of the internals.
        //
        std::vector<Cell> GetLiveCells() const;
        std::vector<Cell> GetAllCells() const;

    private:
        CellStorage  m_storage;
    };
}

