#include "GOLGrid.h"

#include <algorithm>
#include <cassert>

namespace
{
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
        : m_pCurrentStorage(&m_pingPongStorage[0]),
          m_pNextStorage(&m_pingPongStorage[1])
    {
        //
        // Insert live cells
        //
        for (const auto& Address : cellAddresses)
        {
            const bool AliveCell{true};
            m_pCurrentStorage->Insert(Address, AliveCell, 0);
        }

        //
        // Generate adjacent dead cells and properly set neighbor counts.
        //
        std::vector<Cell> deadCells;
        for (auto [liveAddress, IGNORE] : *m_pCurrentStorage)
        {
            //
            // Check for neighbors. Where there isn't one, create a new,
            // dead cell. Where there are neighbors, increment neighor count.
            //
            for (const auto& Offset : NeighborOffsets)
            {
                const auto NeighborAddress = 
                    std::make_pair(
                            liveAddress.first  + Offset.first,
                            liveAddress.second + Offset.second
                        );

                auto neighborCellIt = m_pCurrentStorage->Find(NeighborAddress);
                if (neighborCellIt != m_pCurrentStorage->end())
                {
                    neighborCellIt->second.State.NeighborCount++;
                }
                else
                {
                    //
                    // See if the dead cell exists in the dead cells list.
                    //
                    auto it = std::find_if(
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
            m_pCurrentStorage->Insert(DeadCell);
        }

        *m_pNextStorage = *m_pCurrentStorage;
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

        for (auto& [address, cell] : *m_pCurrentStorage)
        {
            if (cell.State.Alive && !AliveOrDeadLUT[cell.LookupKey()])
            {
                //
                // Mark this cell as dead in the next state.
                //
                auto nextIt = m_pNextStorage->Find(address);
                assert(nextIt != m_pNextStorage->end());
                nextIt->second.State.Alive = false;

                //
                // Decrement neighbor count for all adjacent cells in the next
                // generation.
                //
                for (const auto& Offset : NeighborOffsets)
                {
                    const auto NeighborAddress = 
                        std::make_pair(
                                address.first  + Offset.first,
                                address.second + Offset.second
                            );
                    auto neighborIt = m_pNextStorage->Find(NeighborAddress);
                    assert(neighborIt != m_pNextStorage->end());
                    assert(neighborIt->second.State.NeighborCount > 0);

                    neighborIt->second.State.NeighborCount--;
                }
            }

            if (!cell.State.Alive && AliveOrDeadLUT[cell.LookupKey()])
            {
                //
                // Mark this cell as alive in the next state.
                //
                auto nextIt = m_pNextStorage->Find(address);
                assert(nextIt != m_pNextStorage->end());
                nextIt->second.State.Alive = true;

                for (const auto& Offset : NeighborOffsets)
                {
                    const auto NeighborAddress = 
                        std::make_pair(
                                address.first  + Offset.first,
                                address.second + Offset.second
                            );
                    auto neighborIt = m_pNextStorage->Find(NeighborAddress);
                    if (neighborIt == m_pNextStorage->end())
                    {
                        //
                        // This address doesn't have a cell. Create a dead cell
                        // with a single neighbor.
                        //
                        const bool DeadCell{false};
                        m_pNextStorage->Insert(NeighborAddress, DeadCell, 1);
                    }
                    else
                    {
                        neighborIt->second.State.NeighborCount++;
                    }
                }
            }
            else if (!cell.State.Alive && cell.State.NeighborCount == 0) 
            { 
                m_pNextStorage->Remove(address);
            }
        }

        std::swap(m_pCurrentStorage, m_pNextStorage);
    }

    std::vector<Cell> GOLGrid::GetLiveCells() const
    {
        std::vector<Cell> liveCells;
        for (const auto& [Address, Cell] : *m_pCurrentStorage)
        {
            if (Cell.State.Alive) { liveCells.push_back(Cell); }
        }

        return liveCells;
    }
}

