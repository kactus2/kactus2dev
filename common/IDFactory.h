//-----------------------------------------------------------------------------
// File: IDFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 7.3.2011
//
// Description:
// ID factory for generating unique id numbers.
//-----------------------------------------------------------------------------

#ifndef IDFACTORY_H
#define IDFACTORY_H

#include <deque>

//-----------------------------------------------------------------------------
//! IDFactory class.
//-----------------------------------------------------------------------------
class IDFactory
{
public: 
    /*!
     *  Constructor.
     */
    IDFactory();

    /*!
     *  Destructor.
     */
    ~IDFactory();

    /*!
     *  Returns an unused ID.
     */
    int getID();

    /*!
     *  Frees an ID to later use.
     */
    void freeID(int id);

    /*!
     *  Indicates that the given ID is already in use. This feature is meant for
     *  loading an existing project that has already attached some IDs somewhere.
     */
    void usedID(int id);

private:
    // Disable copying.
    IDFactory(IDFactory const& rhs);
    IDFactory& operator=(IDFactory const& rhs);

    typedef std::deque<int> IDList;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Free IDs.
    IDList m_freeIDs;

    //! The next ID number.
    int m_nextID;
};

//-----------------------------------------------------------------------------

#endif // IDFACTORY_H
