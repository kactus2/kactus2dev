//-----------------------------------------------------------------------------
// File: IDFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.3.2011
//
// Description:
// ID factory for generating unique id numbers.
//-----------------------------------------------------------------------------

#include "IDFactory.h"

#include <algorithm>
#include <cassert>

//-----------------------------------------------------------------------------
// Function: IDFactory()
//-----------------------------------------------------------------------------
IDFactory::IDFactory() : m_freeIDs(), m_nextID(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~IDFactory()
//-----------------------------------------------------------------------------
IDFactory::~IDFactory()
{
    m_freeIDs.clear();
}

//-----------------------------------------------------------------------------
// Function: getID()
//-----------------------------------------------------------------------------
int IDFactory::getID()
{
    // Retrieve an id from the list if there are already free IDs.
    if (!m_freeIDs.empty())
    {
        int id = m_freeIDs.front();
        m_freeIDs.pop_front();
        return id;
    }

    // Otherwise use the next available id.
    int id = m_nextID;
    ++m_nextID;

    return id;
}

//-----------------------------------------------------------------------------
// Function: freeID()
//-----------------------------------------------------------------------------
void IDFactory::freeID(int id)
{
    // Add the id to the list of free IDs.
    m_freeIDs.push_back(id);
}

//-----------------------------------------------------------------------------
// Function: usedID()
//-----------------------------------------------------------------------------
void IDFactory::usedID(int id)
{
    // There are three cases to handle.
    // 1. The given ID would be simply the next ID.
    if (id == m_nextID)
    {
        // Just increment the next ID counter.
        ++m_nextID;
    }
    // 2. The given ID is larger than any of the previous IDs.
    else if (id > m_nextID)
    {
        int oldNext = m_nextID;
        m_nextID = id;

        // Add the missing IDs to the list of free IDs.
        for (int i = oldNext; i < m_nextID; ++i)
        {
            m_freeIDs.push_back(i);
        }

        ++m_nextID;
    }
    // 3. The given ID is less than the next ID, and thus, should be in the list of free IDs.
    else
    {
        // Find the corresponding item and remove it from the list.
        IDList::iterator found = std::find(m_freeIDs.begin(), m_freeIDs.end(), id);

        if (found != m_freeIDs.end())
        {
            m_freeIDs.erase(found);
        }
    }
}
