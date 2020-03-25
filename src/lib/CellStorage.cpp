#include "CellStorage.h"

namespace gol
{
    bool CellStorage::Find(const CellAddress& address, Cell* pCellOut) const
    {
        auto it = m_set.find(address);
        if (it != std::end(m_set))
        {
            *pCellOut = *it;
            return true;
        }
        else
        {
            return false;
        }
    }

    void CellStorage::Insert(const Cell& cell)
    {
        m_set.insert(cell);
    }

    bool CellStorage::Remove(const CellAddress& address)
    {
        auto it = m_set.find(address);
        if (it != std::end(m_set))
        {
            m_set.erase(it);
            return true;
        }
        else
        {
            return false;
        }
    }
}

