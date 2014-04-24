//-----------------------------------------------------------------------------
// File: PortMapsBitMapItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.10.2013
//
// Description:
// Port maps tree item representing a mapping for one bit in logical port.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSBITITEM_H
#define PORTMAPSBITITEM_H

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
//! Port maps tree item representing a mapping for one bit in logical port.
//-----------------------------------------------------------------------------
class PortMapsBitMapItem : public PortMapsTreeItem
{
    Q_OBJECT

public:

    /*!
     *  Constructor for a physical port item.
     */
    PortMapsBitMapItem(PortMapsTreeItem* parent, QSharedPointer<Component> component, 
        BusInterface* busIf, QString const& physicalName = QString());

    /*!
     *  Destructor.
     */
    virtual ~PortMapsBitMapItem();


    /*!
     *  Updates the item.
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
     *  Checks the validity of the item.
     *
     *      @return True, if item is valid, otherwise false.
     */
    virtual bool isValid() const;

    /*!
     *  Checks the validity of the item. 
     *
     *      @param [inout] errorList   The list to add the possible error messages to.
     *
     *      @return True, if the item is valid, otherwise false.
     */
    virtual bool isValid(QStringList& errorList) const;

    /*!
     *  Inserts an item.
     *
     *      @param [in] item The item to insert.
     */
    virtual void addChild(PortMapsBitMapItem* item);


    /*!
     *  Adds a new mapping.
     *
     *      @param [in] portName   The name of the port to map.
     *      @param [in] index      The index of the port to map.
     *
     *      @return 
     */
    void addMapping(QString const& portName, int index = 0);

    /*!
     *  Removes all mappings from the item.
     */
    void clearMappings();

    /*!
     *  Checks if the bit map has at least one connection.
     *
     *      @return True, if map has at least one connection, otherwise false.
     */
    bool isConnected() const;

private:
    // Disable copying.
    PortMapsBitMapItem(PortMapsBitMapItem const& rhs);
    PortMapsBitMapItem& operator=(PortMapsBitMapItem const& rhs);
    
    /*!
     *  Gets the names and indexes of the connected ports.
     *
     *      @return The list of ports and indexes.
     */
    virtual QString getPortConnections() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    // Data structure for one bit mapping.
    struct BitMapping
    {
        int physIndex;            //!< Index of the physical port.
        QString physName;         //!< Name of the physical port.

    /*!
     *  Constructor.
     */
        BitMapping(int index, QString const& name) : physIndex(index), physName(name){};
    
    /*!
     *  Equivalence operator required by QList.
     */
        bool operator==(const BitMapping& other){return physIndex == other.physIndex && 
            QString::compare(physName,other.physName) == 0;};
    };

    //! Pointer to the bus interface of the port map.
    BusInterface* busIf_;

    //! The bit mappings of the logical port bit.
    QList<BitMapping> mappings_;
};

#endif // PORTMAPSBITITEM_H
