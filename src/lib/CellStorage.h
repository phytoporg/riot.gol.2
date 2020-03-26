//
// Container for individual GOL cells.
//

#pragma once

#include "Cell.h"

#include <map>

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
        using ContainerType = std::map<CellAddress, Cell>;
        using iterator = ContainerType::iterator;

        //
        // Returns nullptr if no cell exists inside of this container which 
        // matches the query address.
        //
        // On success, returns retrieved cell address. 
        //
        // *USER BEWARE* the underlying container may not keep this object at 
        // its current address after modifying the container. Avoid inserting
        // and removing while relying on the returned value.
        //
        iterator Find(const CellAddress& address);

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
        iterator begin() noexcept { return m_map.begin(); }
        iterator end()   noexcept { return m_map.end(); }
        
    private:
        ContainerType m_map;
    };
}

