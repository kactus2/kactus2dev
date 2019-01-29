//-----------------------------------------------------------------------------
// File: ConnectedPortsTable.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.01.2019
//
// Description:
// Table for connected ports of two components.
//-----------------------------------------------------------------------------

#ifndef CONNECTEDPORTSTABLE_H
#define CONNECTEDPORTSTABLE_H

#include <QTableWidget>
#include <QSharedPointer>

#include <IPXACTmodels/common/DirectionTypes.h>

class ComponentItem;
class Port;

//-----------------------------------------------------------------------------
//! Table for connected ports of two components.
//-----------------------------------------------------------------------------
class ConnectedPortsTable : public QTableWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] firstItem   The first component item to be connected.
     *      @param [in] secondItem  The second component item to be connected.
     *      @param [in] parent      The parent item.
     */
    ConnectedPortsTable(ComponentItem* firstItem, ComponentItem* secondItem, QWidget* parent = 0);

    /*!
     *  The destructor.
     *
     */
    ~ConnectedPortsTable();

    //! Container for port and its containing component item.
    struct ConnectedPort
    {
        QSharedPointer<Port> port_;

        ComponentItem* containingItem_;
    };

    /*!
     *  Get the list of connected ports.
     *
     *      @return The list of port pairings.
     */
    QVector<QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> > getConnectedPorts()
        const;

public slots:

    /*!
     *  Clear the list of connected ports.
     */
    void clearConnectedPorts();

private:
    // No copying. No assignments.
    ConnectedPortsTable(ConnectedPortsTable const& rhs);
    ConnectedPortsTable& operator=(ConnectedPortsTable const& rhs);

    /*!
     *  Get the name of the selected component item.
     *
     *      @param [in] componentItem   The selected component item.
     *
     *      @return The name of the component item.
     */
    QString getItemName(ComponentItem* componentItem) const;

    /*!
     *  Create the automatic connections between two component items.
     *
     *      @param [in] firstItem   The first component item.
     *      @param [in] secondItem  The second component item.
     */
    void findInitialConnections(ComponentItem* firstItem, ComponentItem* secondItem);

    /*!
     *  Get a list of connectible ports for the selected port.
     *
     *      @param [in] portDirection       Direction of the selected port.
     *      @param [in] secondItemPorts     The ports of the connected item.
     */
    QVector<QSharedPointer<Port> > getConnectablePorts(DirectionTypes::Direction portDirection,
        QSharedPointer<QList<QSharedPointer<Port> > > secondItemPorts) const;

    /*!
     *  Create the table items for the connected ports.
     */
    void createPortTableItems();

    /*!
     *  Get the path of the icon for the selected port.
     *
     *      @param[in] portDirection    Direction of the selected port.
     *
     *      @return Path to the icon of the selected port.
     */
    QString getIconPath(DirectionTypes::Direction portDirection);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the port pairings.
    QVector<QPair<ConnectedPort, ConnectedPort> > portConnections_;
};

#endif // CONNECTEDPORTSTABLE_H
