#include "CellStorage.h"

namespace gol
{
    CellStorage::iterator CellStorage::Find(const CellAddress& address)
    {
        return m_map.find(address);
    }

    void CellStorage::Insert(const Cell& cell)
    {
        m_map[cell.Address] = cell;
    }

    void CellStorage::Insert(
            const CellAddress& address,
            bool isAlive,
            uint8_t numNeighbors)
    {
        Insert(Cell(address, isAlive, numNeighbors));
    }

    bool CellStorage::Remove(const Cell& cell)
    {
        return Remove(cell.Address);
    }

    bool CellStorage::Remove(const CellAddress& address)
    {
        auto it = m_map.find(address);
        if (it != std::end(m_map))
        {
            m_map.erase(it);
            return true;
        }
        else
        {
            return false;
        }
    }
}

