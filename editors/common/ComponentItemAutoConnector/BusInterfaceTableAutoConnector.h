//-----------------------------------------------------------------------------
// File: BusInterfaceTableAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.02.2019
//
// Description:
// Automatically connects bus interfaces of two components.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACETABLEAUTOCONNECTOR_H
#define BUSINTERFACETABLEAUTOCONNECTOR_H

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <editors/common/ComponentItemAutoConnector/TableAutoConnector.h>

class BusDefinition;

class Component;
class BusInterface;
class Design;

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Automatically connects bus interfaces of two components.
//-----------------------------------------------------------------------------
class BusInterfaceTableAutoConnector : public TableAutoConnector
{
public:

    /*!
     *  The constructor.
     */
    BusInterfaceTableAutoConnector(LibraryInterface* library);

    /*!
     *  Destructor.
     */
    virtual ~BusInterfaceTableAutoConnector() = default;

    // No copying. No assignments.
    BusInterfaceTableAutoConnector(BusInterfaceTableAutoConnector const& rhs) = delete;
    BusInterfaceTableAutoConnector& operator=(BusInterfaceTableAutoConnector const& rhs) = delete;

protected:

    /*!
     *  Create a table item.
     *
     *    @param [in] itemName                Name for the table item.
     *    @param [in] containingComponent     Component containing the selected item.
     *
     *    @return The new table item.
     */
    virtual QTableWidgetItem* createTableWidgetItem(QString const& itemName,
        QSharedPointer<Component> containingComponent) const;

private:

    /*!
     *  Find the possible item connections from the selected components.
     *
     *    @param [in] firstComponent      The first component.
     *    @param [in] secondComponent     The second component.
     *
     *    @return List of item / possible connected items pairs.
     */
    virtual QVector<QPair<QString, QVector<QString> > > findPossibleCombinations(
        QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const;

    /*!
     *	Find items that are already connected to populate the connection table with.
     *  
     *    @param [in] firstInstanceName   The first component.
     *    @param [in] secondInstanceName  The second component.
     *    @param [in] design              The containing design.
     *	    
     * 	    @return List of item connected item pairs.
     */
    virtual QList<QPair<QString, QString > > findAlreadyConnectedItems(
        QString const& firstInstanceName, QString const& secondInstanceName,
        QSharedPointer<Design> design) const override;

    /*!
     *  Get the names of the connectible bus interfaces for the selected bus interface.
     *
     *    @param [in] currentBus                  The selected bus interface.
     *    @param [in] secondItemBusInterfaces     List of the available bus interfaces.
     *
     *    @return A list of bus interface names connectible to the selected bus interface.
     */
    QVector<QString> getConnectableBusInterfaceNames(QSharedPointer<BusInterface> currentBus,
        QSharedPointer<QList<QSharedPointer<BusInterface> > > secondItemBusInterfaces) const;

    /*!
     *  Check if the selected bus interface can be connected to the selected mode.
     *
     *    @param [in] comparisonBus       The bus interface being compared to.
     *    @param [in] comparibleModes     List of connectible bus interface modes.
     *
     *    @return True, if the selected bus interface can be connected to the selected mode, false otherwise.
     */
    bool interfacesAreCompatible(QSharedPointer<BusInterface> comparisonBus,
        QVector<General::InterfaceMode> compatibleModes) const;

    /*!
     *  Get connectible bus interface modes for the selected interface mode.
     *
     *    @param [in] busMode     The selected bus interface mode.
     *
     *    @return List of connectible interface modes.
     */
    virtual QVector<General::InterfaceMode> getCompatibleInterfaceModes(General::InterfaceMode const& busMode)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Library access.
    LibraryInterface* library_;
};

//-----------------------------------------------------------------------------

#endif // BUSINTERFACETABLEAUTOCONNECTOR_H
