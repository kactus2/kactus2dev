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

#include <QListWidget>

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
    PortListFiller();

    /*!
     *  Destructor.
     */
    ~PortListFiller();

    /*!
     *  Populate the selected list widget with ports from the selected component.
     *
     *      @param [in] selectedList            The target list widget.
     *      @param [in] containingComponent     The selected component.
     */
    void initializeList(QListWidget* selectedList, QSharedPointer<Component> containingComponent) const;

    // No copying. No assignments.
    PortListFiller(PortListFiller const& rhs) = delete;
    PortListFiller& operator=(PortListFiller const& rhs) = delete;

private:

    /*!
     *  Get the path for the icon used by the port list item.
     *
     *      @param [in] portDirection   Direction of the selected port.
     */
    QString getIconPath(DirectionTypes::Direction portDirection) const;
};

//-----------------------------------------------------------------------------

#endif // AUTOCONNECTORLISTFILLER_H
