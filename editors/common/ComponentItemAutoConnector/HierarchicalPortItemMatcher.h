//-----------------------------------------------------------------------------
// File: HierarchicalPortItemMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically defines possible matches between an active port and a hierarchical one.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALPORTITEMMATCHER_H
#define HIERARCHICALPORTITEMMATCHER_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <editors/common/ComponentItemAutoConnector/PortItemMatcher.h>

//-----------------------------------------------------------------------------
//! Automatically defines possible matches between two ports.
//-----------------------------------------------------------------------------
class HierarchicalPortItemMatcher : public PortItemMatcher
{
public:

    /*!
     *  The constructor.
     */
    HierarchicalPortItemMatcher() = default;

    /*!
     *  Destructor.
     */
    virtual ~HierarchicalPortItemMatcher() = default;

    // No copying. No assignments.
    HierarchicalPortItemMatcher(HierarchicalPortItemMatcher const& rhs) = delete;
    HierarchicalPortItemMatcher& operator=(HierarchicalPortItemMatcher const& rhs) = delete;

private:

    /*!
     *  Get connectible port directions for the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return List of connectible port directions.
     */
    virtual QVector<DirectionTypes::Direction> getConnectableDirections(DirectionTypes::Direction portDirection)
        const;
};

//-----------------------------------------------------------------------------

#endif // HIERARCHICALPORTITEMMATCHER_H
