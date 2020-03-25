//
// Container for individual GOL cells.
//

#pragma once

#include "Cell.h"

#include <set>

namespace gol
{
    class CellStorage
    {
    public:
        CellStorage() = default;
        ~CellStorage() = default;

        //
        // Would be nice to templateize the underlying representation or just
        // make this opaque;
        //
        using ContainerType = std::set<Cell, CellCompare>;

        //
        // Returns false if no cell exists inside of this container which 
        // matches the query address. *pCellOut is not modified in this
        // case.
        //
        // On success, *pCellOut contains the retrieved data.
        //
        bool Find(const CellAddress& address, Cell* pCellOut) const;

        //
        // Inserts a new cell into the container if it doesn't already exist,
        // and otherwise clobbers the existing state at the cell address.
        //
        void Insert(const Cell& cell);

        //
        // Fails if no cell exists at the provided address.
        //
        bool Remove(const CellAddress& address);

        //
        // Iterator convenience.
        //
        using iterator = ContainerType::iterator;
        iterator begin() noexcept { return m_set.begin(); }
        iterator end()   noexcept { return m_set.end(); }
        
    private:
        ContainerType m_set;
    };
}

