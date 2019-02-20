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

class Component;
class ComponentItem;
class AutoConnectorConnectionTable;
class ListFiller;
class TableAutoConnector;
class AutoConnectorListFilter;

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
     *      @param [in] firstItem           The first component item to be connected.
     *      @param [in] secondItem          The second component item to be connected.
     *      @param [in] listFiller          Used to populate the item lists.
     *      @param [in] tableInitializer    Used to populate the connection table.
     *      @param [in] itemName            Name of the items being connected.
     *      @param [in] parent              The parent of this widget.
     */
    AutoConnector(ComponentItem* firstItem, ComponentItem* secondItem, ListFiller* listFiller,
        TableAutoConnector* tableInitializer, QString const& itemName, QWidget* parent = 0);

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
     *      @param [in] firstItem   The first component item to be connected.
     *      @param [in] secondItem  The second component item to be connected.
     *      @param [in] listFiller  Used to populate the item lists.
     *      @param [in] itemName    Name of the items being connected.
     */
    void setupLayout(ComponentItem* firstItem, ComponentItem* secondItem, ListFiller* listFiller,
        QString const& itemName);

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

    //! The first component.
    QSharedPointer<Component> firstComponent_;

    //! The second component.
    QSharedPointer<Component> secondComponent_;

    //! List filter for the first list.
    AutoConnectorListFilter* firstListFilter_;

    //! List filter for the second list.
    AutoConnectorListFilter* secondListFilter_;

    //! Table containing the item connections.
    AutoConnectorConnectionTable* connectorTable_;

    //! Used to populate the connection table.
    TableAutoConnector* tableInitializer_;
};

//-----------------------------------------------------------------------------

#endif // PORTAUTOCONNECTOR_H
