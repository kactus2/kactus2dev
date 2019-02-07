//-----------------------------------------------------------------------------
// File: ComponentItemAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.01.2019
//
// Description:
// Automatically connects the ports and bus interfaces of two component items.
//-----------------------------------------------------------------------------

#ifndef COMPONENTITEMAUTOCONNECTOR_H
#define COMPONENTITEMAUTOCONNECTOR_H

#include <editors/common/ComponentItemAutoConnector/ConnectedPortsTable.h>

#include <QDialog>
#include <QObject>
#include <QPushButton>

class ComponentItem;

//-----------------------------------------------------------------------------
//! Automatically connects the ports and bus interfaces of two component items.
//-----------------------------------------------------------------------------
class ComponentItemAutoConnector : public QDialog
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] firstItem   The first component item to be connected.
     *      @param [in] secondItem  The second component item to be connected.
     *      @param [in] parent      The parent of this widget.
     */
    ComponentItemAutoConnector(ComponentItem* firstItem, ComponentItem* secondItem, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentItemAutoConnector();

    /*!
     *  Get the list of connected ports.
     *
     *      @return A list of port pairs.
     */
    QVector<QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> > getConnectedPorts()
        const;

private slots:

private:
    // No copying. No assignments.
    ComponentItemAutoConnector(ComponentItemAutoConnector const& rhs);
    ComponentItemAutoConnector& operator=(ComponentItemAutoConnector const& rhs);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Get the name of the selected component item.
     *
     *      @param [in] componentItem   The selected component item.
     *
     *      @return The name of the selected component item.
     */
    QString getComponentItemName(ComponentItem* componentItem) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component item.
    ComponentItem* firstItem_;

    //! The second component item.
    ComponentItem* secondItem_;

    //! Button for connecting the ports automatically.
    QPushButton* autoConnectButton_;

    //! The button for clearing the connected ports table.
    QPushButton* clearButton_;

    //! Table for the connected ports.
    ConnectedPortsTable* portsTable_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTITEMAUTOCONNECTOR_H
