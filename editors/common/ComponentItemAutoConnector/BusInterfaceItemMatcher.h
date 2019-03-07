//-----------------------------------------------------------------------------
// File: BusInterfaceItemMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEITEMMATCHER_H
#define BUSINTERFACEITEMMATCHER_H

#include <editors/common/ComponentItemAutoConnector/TableItemMatcher.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Automatically defines possible matches between two bus interfaces.
//-----------------------------------------------------------------------------
class BusInterfaceItemMatcher : public TableItemMatcher
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     Access to the document library.
     */
    BusInterfaceItemMatcher(LibraryInterface* library);

    /*!
     *  Destructor.
     */
    virtual ~BusInterfaceItemMatcher() = default;

    // No copying. No assignments.
    BusInterfaceItemMatcher(BusInterfaceItemMatcher const& rhs) = delete;
    BusInterfaceItemMatcher& operator=(BusInterfaceItemMatcher const& rhs) = delete;

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

    /*!
     *  Get connectible bus interface modes for the selected interface mode.
     *
     *      @param [in] busMode     The selected bus interface mode.
     *
     *      @return List of connectible interface modes.
     */
    virtual QVector<General::InterfaceMode> getCompatibleModes(General::InterfaceMode busMode) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Access to the document library.
    LibraryInterface* library_;
};

//-----------------------------------------------------------------------------

#endif // BUSINTERFACEITEMMATCHER_H
