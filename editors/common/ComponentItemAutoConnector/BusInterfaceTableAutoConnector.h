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
    ~BusInterfaceTableAutoConnector();

    // No copying. No assignments.
    BusInterfaceTableAutoConnector(BusInterfaceTableAutoConnector const& rhs) = delete;
    BusInterfaceTableAutoConnector& operator=(BusInterfaceTableAutoConnector const& rhs) = delete;

private:

    /*!
     *  Find the possible item connections from the selected components.
     *
     *      @param [in] firstComponent      The first component.
     *      @param [in] secondComponent     The second component.
     *
     *      @return List of item / possible connected items pairs.
     */
    virtual QVector<QPair<QString, QVector<QString> > > findPossibleCombinations(
        QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const;

    /*!
     *  Get the names of the connectible bus interfaces for the selected bus interface.
     *
     *      @param [in] currentBus                  The selected bus interface.
     *      @param [in] secondItemBusInterfaces     List of the available bus interfaces.
     *
     *      @return A list of bus interface names connectible to the selected bus interface.
     */
    QVector<QString> getConnectableBusInterfaceNames(QSharedPointer<BusInterface> currentBus,
        QSharedPointer<QList<QSharedPointer<BusInterface> > > secondItemBusInterfaces) const;

    /*!
     *  Check if the selected bus interface can be connected to the selected mode.
     *
     *      @param [in] comparisonBus       The bus interface being compared to.
     *      @param [in] comparibleModes     List of connectible bus interface modes.
     *
     *      @return True, if the selected bus interface can be connected to the selected mode, false otherwise.
     */
    bool interfacesAreCompatible(QSharedPointer<BusInterface> comparisonBus,
        QVector<General::InterfaceMode> compatibleModes) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Library access.
    LibraryInterface* library_;
};

//-----------------------------------------------------------------------------

#endif // BUSINTERFACETABLEAUTOCONNECTOR_H
