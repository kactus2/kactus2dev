//-----------------------------------------------------------------------------
// File: PortListFiller.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Fills a list with ports.
//-----------------------------------------------------------------------------

#ifndef PORTLISTFILLER_H
#define PORTLISTFILLER_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <editors/common/ComponentItemAutoConnector/ListFiller.h>

class Component;

//-----------------------------------------------------------------------------
//! Fills a list with ports.
//-----------------------------------------------------------------------------
class PortListFiller : public ListFiller
{
public:

    /*!
     *  The constructor.
     */
    PortListFiller() = default;

    /*!
     *  Destructor.
     */
    ~PortListFiller() = default;

    /*!
     *  Populate the selected model with ports from the selected component.
     *
     *      @param [in] selectedList            The selected item model.
     *      @param [in] containingComponent     The selected component.
     */
    virtual void initializeList(QStandardItemModel* selectedList, QSharedPointer<Component> containingComponent)
        const;

    // No copying. No assignments.
    PortListFiller(PortListFiller const& rhs) = delete;
    PortListFiller& operator=(PortListFiller const& rhs) = delete;
};

//-----------------------------------------------------------------------------

#endif // AUTOCONNECTORLISTFILLER_H
