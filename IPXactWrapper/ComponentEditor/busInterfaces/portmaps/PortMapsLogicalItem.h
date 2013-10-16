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

private:
    // Disable copying.
    PortMapsLogicalItem(PortMapsLogicalItem const& rhs);
    PortMapsLogicalItem& operator=(PortMapsLogicalItem const& rhs);
    
    QString getPhysPorts() const;

    /*!
     *  Gets the width of the logical signal.
     *
     *
     *      @return width of the logical signal.
     */
    int getWidth() const;

    /*!
     *  Gets the width of the logical signal.
     *
     *
     *      @return width of the logical signal.
     */
    int getConnectionCount() const;


    void updateWidthTo(int width);    

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------  


BusInterface* busif_;

    //! \brief Pointer to the abstraction definition that is used.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! \brief Specifies the interface mode of this bus interface
    General::InterfaceMode interfaceMode_;

    //! The port maps of the logical port item.
	QList<QSharedPointer<General::PortMap> >& portMaps_;

    int right_;

    int left_;
    
    bool beginMapping_;
    
};

#endif // PORTMAPSLOGICALITEM_H
