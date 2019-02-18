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

#include <editors/common/ComponentItemAutoConnector/AutoConnectorItem.h>

#include <QDialog>
#include <QObject>
#include <QPushButton>
#include <QTabWidget>

class ComponentItem;
class AutoConnector;
class LibraryInterface;

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
     *      @param [in] library     The library access.
     *      @param [in] parent      The parent of this widget.
     */
    ComponentItemAutoConnector(ComponentItem* firstItem, ComponentItem* secondItem, LibraryInterface* library,
        QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~ComponentItemAutoConnector();

    /*!
     *  Get the list of connected items.
     *
     *      @return A list of item pairs.
     */
    QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > getConnectedItems() const;

    // No copying. No assignments.
    ComponentItemAutoConnector(ComponentItemAutoConnector const& rhs) = delete;
    ComponentItemAutoConnector& operator=(ComponentItemAutoConnector const& rhs) = delete;

private slots:

    /*!
     *  Clear the connected items from the currently selected table.
     */
    void clearCurrentConnections();

    /*!
     *  Connect the items of the currently selected table automatically.
     */
    void connectCurrentItems();

private:

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Create connected item pairs from the selected connector table.
     *
     *      @param [in] connector   The selected connector table.
     *      @param [in] itemType    Type for the items.
     *
     *      @return List of connected item pairs.
     */
    QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > createItemPairs(AutoConnector* connector,
        AutoConnectorItem::ItemType itemType) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the first component item.
    QString firstItemName_;

    //! Name of the second component item.
    QString secondItemName_;

    //! Button for connecting items automatically.
    QPushButton* autoConnectButton_;

    //! Button for clearing the connected items table.
    QPushButton* clearButton_;

    //! Connection editor for bus interfaces.
    AutoConnector* busInterfaceConnector_;

    //! Connection editor for ports.
    AutoConnector* portConnector_;

    //! Holds the different editors.
    QTabWidget tabs_;
};

//-----------------------------------------------------------------------------

#endif // COMPONENTITEMAUTOCONNECTOR_H
