//-----------------------------------------------------------------------------
// File: PortItemMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two ports.
//-----------------------------------------------------------------------------

#ifndef PORTITEMMATCHER_H
#define PORTITEMMATCHER_H

#include <editors/common/ComponentItemAutoConnector/TableItemMatcher.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QSharedPointer>

class Component;

//-----------------------------------------------------------------------------
//! Automatically defines possible matches between two ports.
//-----------------------------------------------------------------------------
class PortItemMatcher : public TableItemMatcher
{
public:

    /*!
     *  The constructor.
     */
    PortItemMatcher() = default;

    /*!
     *  Destructor.
     */
    virtual ~PortItemMatcher() = default;

    // No copying. No assignments.
    PortItemMatcher(PortItemMatcher const& rhs) = delete;
    PortItemMatcher& operator=(PortItemMatcher const& rhs) = delete;

private:

    /*!
     *  Check if the selected items can be connected.
     *
     *      @param [in] firstItem           The selected item from the first component.
     *      @param [in] firstComponent      Component containing the first item.
     *      @param [in] secondItem          The selected item from the second component.
     *      @param [in] secondComponent     Component containing the second item.
     *
     *      @return True, if the items can be connected, false otherwise.
     */
    virtual bool itemsCanBeConnected(QString const& firstItem, QSharedPointer<Component> firstComponent,
        QString const& secondItem, QSharedPointer<Component> secondComponent) const;
};

//-----------------------------------------------------------------------------

#endif // PORTITEMMATCHER_H
