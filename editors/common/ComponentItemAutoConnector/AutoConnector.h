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
     *      @param [in] firstComponentName      Visible name of the first container.
     *      @param [in] secondComponentName     Visible name of the second container.
     *      @param [in] firstComponent          The first container component.
     *      @param [in] secondComponent         The second container component.
     *      @param [in] listFiller              Used to populate the item lists.
     *      @param [in] tableInitializer        Used to populate the connection table.
     *      @param [in] itemName                Name of the items being connected.
     *      @param [in] itemMatcher             Checks for possible matches between two items.
     *      @param [in] parent                  The parent of this widget.
     */
    AutoConnector(QString const& firstComponentName, QString const& secondComponentName,
        QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent,
        ListFiller* listFiller, TableAutoConnector* tableInitializer, QString const& itemName,
        TableItemMatcher* itemMatcher, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~AutoConnector();

    /*!
     *  Get the list of connected items.
     *
     *      @return A list of connected item pairs.
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
     *      @param [in] firstComponentName      Name of the first container.
     *      @param [in] secondComponentName     Name of the second container.
     *      @param [in] listFiller              Used to populate the item lists.
     *      @param [in] itemName                Name of the items being connected.
     *      @param [in] itemMatcher             Checks for possible matches between two items.
     */
    void setupLayout(QString const& firstComponentName, QString const& secondComponentName, ListFiller* listFiller,
        QString const& itemName, TableItemMatcher* itemMatcher);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The first component.
    QSharedPointer<Component> firstComponent_;

    //! The second component.
    QSharedPointer<Component> secondComponent_;

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
    TableAutoConnector* tableInitializer_;
};

//-----------------------------------------------------------------------------

#endif // PORTAUTOCONNECTOR_H
