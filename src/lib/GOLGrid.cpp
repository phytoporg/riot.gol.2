#include "GOLGrid.h"

#include <algorithm>
#include <cassert>

namespace
{
    //
    // For address + offset type logic.
    //
    gol::CellAddress 
    operator+(
        const gol::CellAddress& a,
        const gol::CellAddress& b
    )
    {
        return std::make_pair<int64_t, int64_t>(
                a.first + b.first,
                a.second + b.second
            );
    }

    static const std::vector<gol::CellAddress> NeighborOffsets = {
        //
        // Neighbor offsets in clockwise order from the upper-left corner.
        //
        std::make_pair<int64_t, int64_t>(-1, -1),
        std::make_pair<int64_t, int64_t>( 0, -1),
        std::make_pair<int64_t, int64_t>( 1, -1),
        std::make_pair<int64_t, int64_t>( 1,  0),
        std::make_pair<int64_t, int64_t>( 1,  1),
        std::make_pair<int64_t, int64_t>( 0,  1),
        std::make_pair<int64_t, int64_t>(-1,  1),
        std::make_pair<int64_t, int64_t>(-1,  0)
    };
}

namespace gol
{
    GOLGrid::GOLGrid(const std::vector<CellAddress>& cellAddresses)
    {
        //
        // Insert live cells
        //
        for (const auto& Address : cellAddresses)
        {
            const bool AliveCell{true};
            m_storage.Insert(Address, AliveCell, 0);
        }

        //
        // Generate adjacent dead cells and properly set neighbor counts.
        //
        std::vector<Cell> deadCells;
        for (auto [liveAddress, IGNORE] : m_storage)
        {
            //
            // Check for neighbors. Where there isn't one, create a new,
            // dead cell. Where there are neighbors, increment neighor count.
            //
            for (const auto& Offset : NeighborOffsets)
            {
                const auto NeighborAddress = liveAddress + Offset;
                auto neighborCellIt = m_storage.Find(NeighborAddress);
                if (neighborCellIt != m_storage.end())
                {
                    neighborCellIt->second.State.NeighborCount++;
                }
                else
                {
                    //
                    // See if the dead cell exists in the dead cells list.
                    //
                    auto it = 
                        std::find_if(
                            std::begin(deadCells),
                            std::end(deadCells),
                            [&NeighborAddress](const Cell& cell)
                            {
                                return NeighborAddress == cell.Address;
                            });
                    if (it != std::end(deadCells))
                    {
                        it->State.NeighborCount++;
                    }
                    else
                    {
                        //
                        // Don't mess with the container while iterating through
                        // it. Defer adding dead cells to storage.
                        //
                        const bool DeadCell{false};
                        deadCells.emplace_back(NeighborAddress, DeadCell, 1);
                    }
                }
            }
        }

        for (const auto& DeadCell : deadCells)
        {
            m_storage.Insert(DeadCell);
        }
    }

    void GOLGrid::AdvanceGeneration()
    {
        //
        // A look-up table which encodes life-to-death states based on the 
        // bit field contained in the State variable in a given cell.
        //
        // High order bit represents current alive state and the remaining
        // bits are the neighbor count.
        //
        static const std::vector<bool> AliveOrDeadLUT = 
        {
            false, // 0b00000
            false, // 0b00001
            false, // 0b00010
            true , // 0b00011
            false, // 0b00100
            false, // 0b00101
            false, // 0b00110
            false, // 0b00111
            false, // 0b01000

            //
            // This chunk represents invalid states, since any given cell may
            // only have up to eight neighbors.
            //
            false, // 0b01001
            false, // 0b01010
            false, // 0b01011
            false, // 0b01100
            false, // 0b01101
            false, // 0b01110
            false, // 0b01111

            false, // 0b10000
            false, // 0b10001
            true , // 0b10010
            true , // 0b10011
            false, // 0b10100
            false, // 0b10101
            false, // 0b10110
            false, // 0b10111
            false, // 0b11000
        };

        //
        // To update storage in-place, conduct the update in two passes:
        // 1) Note state transitions for each cell (alive<->dead)
        // 2) Update transitioning cells' living state and the neighbor count
        //    of their neighbors.
        //
        // Along the way, kill off cells with no neighbors.
        //
        std::vector<Cell> changedCells;
        std::vector<Cell> retiredCells;
        for (auto& [address, cell] : m_storage)
        {
            const bool NewState{ AliveOrDeadLUT[cell.LookupKey()] };
            const bool Transitioned{ cell.State.Alive != NewState };
            if (Transitioned) 
            { 
                changedCells.push_back(cell); 
            }
            else if (!cell.State.Alive && cell.State.NeighborCount == 0)
            {
                retiredCells.push_back(cell);
            }
        }

        for (auto& retiredCell : retiredCells)
        {
            m_storage.Remove(retiredCell);
        }
        
        for (auto& changedCell : changedCells)
        {
            changedCell.State.Alive = !changedCell.State.Alive;

            // 
            // Update the alive state here, neighbor count below.
            //
            auto changedCellIt = m_storage.Find(changedCell.Address);
            changedCellIt->second.State.Alive = changedCell.State.Alive;

            if (!changedCell.State.Alive)
            {
                //
                // Decrement neighbor cell NeighborCounts
                //
                for (const auto& Offset : NeighborOffsets)
                {
                    const auto NeighborAddress = changedCell.Address + Offset;
                    auto neighborIt = m_storage.Find(NeighborAddress);

                    //
                    // A formerly living cell should have eight neighboring
                    // cells in storage (not necessarily alive).
                    //
                    assert(neighborIt != m_storage.end());
                    assert(neighborIt->second.State.NeighborCount > 0);
                    neighborIt->second.State.NeighborCount--;
                }
            }
            else if(changedCell.State.Alive)
            {
                //
                // Increment neighbor cell NeighborCounts
                //
                for (const auto& Offset : NeighborOffsets)
                {
                    const auto NeighborAddress = changedCell.Address + Offset;
                    auto neighborIt = m_storage.Find(NeighborAddress);
                    if (neighborIt == m_storage.end())
                    {
                        //
                        // Live cells must be surrounded.
                        //
                        const bool DeadCell{false};
                        m_storage.Insert(NeighborAddress, DeadCell, 1);
                    }
                    else
                    {
                        assert(neighborIt->second.State.NeighborCount < 9);
                        neighborIt->second.State.NeighborCount++;
                    }
                }
            } 
        }
    }

    std::vector<Cell> GOLGrid::GetLiveCells() const
    {
        std::vector<Cell> liveCells;
        for (const auto& [IGNORE, Cell] : m_storage)
        {
            if (Cell.State.Alive) { liveCells.push_back(Cell); }
        }

        return liveCells;
    }

    std::vector<Cell> GOLGrid::GetAllCells() const
    {
        std::vector<Cell> cells;
        for (const auto& [IGNORE, Cell] : m_storage) { cells.push_back(Cell); }

        return cells;
    }
}

