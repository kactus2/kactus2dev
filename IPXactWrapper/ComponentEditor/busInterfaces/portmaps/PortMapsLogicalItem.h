//-----------------------------------------------------------------------------
// File: PortMapsLogicalItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// Port maps tree item representing a logical port.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSLOGICALITEM_H
#define PORTMAPSLOGICALITEM_H

#include "PortMapsTreeItem.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QSharedPointer>

#include <models/generaldeclarations.h>

class AbstractionDefinition;
class BusInterface;
class Component;

//-----------------------------------------------------------------------------
//! Port maps tree item representing a logical port.
//-----------------------------------------------------------------------------
class PortMapsLogicalItem : public PortMapsTreeItem
{
    Q_OBJECT

public:

    /*!
     *  Constructor for a logical port item.
     */
    PortMapsLogicalItem(PortMapsTreeItem* parent,
        QString const& logicalName,
        QSharedPointer<Component> component,
        BusInterface* busif,
        QSharedPointer<AbstractionDefinition> absDef,
        General::InterfaceMode interfaceMode
    );

    /*!
     *  Destructor.
     */
    virtual ~PortMapsLogicalItem();

    /*!
     *  Updates the item.
     *
     */
    virtual void refresh();

    /*!
     *  Gets the item data for a given section.
     *
     *      @param [in] section   The section whose data to get.
     *
     *      @return The data in section.
     */
    virtual QVariant data(int section) const;

    /*!
     *  Clears the mappings for all children.
     */
    void clearMappings();

    /*!
     *  Gets the width of the logical signal.
     *
     *
     *      @return width of the logical signal.
     */
    int getWidth() const;

private:
    // Disable copying.
    PortMapsLogicalItem(PortMapsLogicalItem const& rhs);
    PortMapsLogicalItem& operator=(PortMapsLogicalItem const& rhs);
    
    /*!
     *  Gets a string of the connected ports and their bits.
     *     
     *      @return Names and bits of connected physical ports.
     */
    QString getPhysPorts() const;


    /*!
     *  Gets the width of the logical signal.
     *
     *
     *      @return width of the logical signal.
     */
    int getConnectionCount() const;


    /*!
     *  Updates the port width to given value by adding/removing children.
     *
     *      @param [in] width   The new width.
     */
    void updateWidthTo(int width);    

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------  

    //! Pointer to the logical port bus interface.
    BusInterface* busif_;

    //! Pointer to the abstraction definition that is used.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! Specifies the interface mode of this bus interface
    General::InterfaceMode interfaceMode_;

    //! The port maps of the logical port item.
	QList<QSharedPointer<General::PortMap> >& portMaps_;

    //! The left bound of the logical port.
    int right_;

    //! The right bound of the logical port.
    int left_;
    
    //! Flag for indicating beginning of mapping when bounds are not yet set.
    bool beginMapping_;
    
};

#endif // PORTMAPSLOGICALITEM_H
