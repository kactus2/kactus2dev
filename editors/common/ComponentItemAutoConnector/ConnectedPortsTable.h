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

    /*!
     *  Get the available mime types.
     *
     *      @return List of the available mime types.
     */
    virtual QStringList mimeTypes() const;

protected:

    /*!
     *  Handles the context menu.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent *event);

    /*!
     *  Handles the mime data drop.
     *
     *      @param [in] row     Row to drop the mime data to.
     *      @param [in] column  Column to drop the mime data to.
     *      @param [in] data    The mime data.
     *      @param [in] action  The drop action.
     */
    virtual bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);

    /*!
     *  Handles the drop events.
     *
     *      @param [in] event   The drop event.
     */
    virtual void dropEvent(QDropEvent *event);

public slots:

    /*!
     *  Clear the list of connected ports.
     */
    void clearConnectedPorts();

    /*!
     *  Connect the items of the selected instances automatically.
     */
    void connectAutomatically();

private slots:

    /*!
     *  Handles the row removal.
     */
    void onRemoveRow();

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
     */
    void findInitialConnections();

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

    /*!
     *  Get the port of the containing item using the port name.
     *
     *      @param [in] portName        Name of the selected port.
     *      @param [in] containingItem  Component item containing the selected port.
     *
     *      @return The port with the selected port name.
     */
    QSharedPointer<Port> getPortByName(QString const& portName, ComponentItem* containingItem) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component item.
    ComponentItem* firstItem_;

    //! The second component item.
    ComponentItem* secondItem_;

    //! Source component item for the drag & drop event.
    ComponentItem* dragSourceComponentItem_;

    //! List of the port pairings.
    QVector<QPair<ConnectedPort, ConnectedPort> > portConnections_;

    //! Action for removing rows.
    QAction* removeRowAction_;
};

#endif // CONNECTEDPORTSTABLE_H
