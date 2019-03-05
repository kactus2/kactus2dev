//-----------------------------------------------------------------------------
// File: BusInterfaceListFiller.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.02.2019
//
// Description:
// Fills a list with bus interfaces.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACELISTFILLER_H
#define BUSINTERFACELISTFILLER_H

#include <editors/common/ComponentItemAutoConnector/ListFiller.h>

#include <IPXACTmodels/generaldeclarations.h>

class Component;

//-----------------------------------------------------------------------------
//! Fills a list with bus interfaces.
//-----------------------------------------------------------------------------
class BusInterfaceListFiller : public ListFiller
{
public:

    /*!
     *  The constructor.
     */
    BusInterfaceListFiller() = default;

    /*!
     *  Destructor.
     */
    ~BusInterfaceListFiller() = default;

    /*!
     *  Populate the selected model with bus interfaces from the selected component.
     *
     *      @param [in] selectedList            The selected item model.
     *      @param [in] containingComponent     The selected component.
     */
    virtual void initializeList(QStandardItemModel* selectedList, QSharedPointer<Component> containingComponent)
        const;

    // No copying. No assignments.
    BusInterfaceListFiller(BusInterfaceListFiller const& rhs) = delete;
    BusInterfaceListFiller& operator=(BusInterfaceListFiller const& rhs) = delete;
};

//-----------------------------------------------------------------------------

#endif // BUSINTERFACELISTFILLER_H
