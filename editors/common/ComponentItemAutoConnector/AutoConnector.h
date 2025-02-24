//-----------------------------------------------------------------------------
// File: AutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects items of two component items.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTOR_H
#define AUTOCONNECTOR_H

#include <editors/common/ComponentItemAutoConnector/ComponentItemAutoConnector.h>

#include <QObject>
#include <QWidget>
#include <QListView>

class Component;
class ComponentItem;
class AutoConnectorConnectionTable;
class ListFiller;
class TableAutoConnector;
class AutoConnectorListFilter;
class TableItemMatcher;
class Design;

//-----------------------------------------------------------------------------
//! Automatically connects items of two component items.
//-----------------------------------------------------------------------------
class AutoConnector : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] firstComponentContainer     The first component container.
     *    @param [in] secondComponentContainer    The second component container.
     *    @param [in] tableToolsContainer         The table tools container.
     *    @param [in] itemName                    Name of the items being connected.
     *    @param [in] parent                      The parent of this widget.
     */
    AutoConnector(ComponentItemAutoConnector::AutoContainer const& firstComponentContainer, 
        ComponentItemAutoConnector::AutoContainer const& secondComponentContainer, 
        ComponentItemAutoConnector::TableTools const& tableToolsContainer, QString const& itemName, 
        QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~AutoConnector();

    /*!
     *  Get the list of connected items.
     *
     *    @return A list of connected item pairs.
     */
    QVector<QPair<QString, QString> > getConnectedItems() const;

    /*!
     *  Connect the selected items from the item lists.
     */
    void connectSelectedItems();

    /*!
     *  Automatically connect the items.
     */
    void connectAutomatically();

    /*!
     *  Automatically connect the items.
     */
    void clearConnectedItems();

    /*!
     *	Fill the connection table with already connected items.
     *  
     *    @param [in] design     The containing design.
     */
    void connectAlreadyConnectedItems(QSharedPointer<Design> design);

    /*!
     *	Checks if the created connections are valid or not.
     *  
     * 	    @return True, if there are invalid connections, otherwise false.
     */
    bool connectionTableHasInvalidConnections() const;

    // No copying. No assignments.
    AutoConnector(AutoConnector const& rhs) = delete;
    AutoConnector& operator=(AutoConnector const& rhs) = delete;

public slots:

    /*!
     *  Invalidate the list filters.
     */
    void invalidateListFilters();

private:

    /*!
     *  Setup the layout.
     *
     *    @param [in] firstComponentName      Name of the first container.
     *    @param [in] secondComponentName     Name of the second container.
     *    @param [in] itemName                Name of the items being connected.
     */
    void setupLayout(QString const& firstComponentName, QString const& secondComponentName,
        QString const& itemName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component.
    ComponentItemAutoConnector::AutoContainer firstComponentContainer_;

    //! The second component.
    ComponentItemAutoConnector::AutoContainer secondComponentContainer_;

    //! The component connector table tools.
    ComponentItemAutoConnector::TableTools connectorTableTools_;

    //! List filter for the first list.
    AutoConnectorListFilter* firstListFilter_;

    //! List filter for the second list.
    AutoConnectorListFilter* secondListFilter_;

    //! View of the first item list.
    QListView* firstItemList_;

    //! View of the second item list.
    QListView* secondItemList_;

    //! Table containing the item connections.
    AutoConnectorConnectionTable* connectorTable_;

    //! Used to populate the connection table.
    QSharedPointer<TableAutoConnector> tableInitializer_;
};

//-----------------------------------------------------------------------------

#endif // PORTAUTOCONNECTOR_H
