//-----------------------------------------------------------------------------
// File: TableAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects items of two components.
//-----------------------------------------------------------------------------

#ifndef TABLEAUTOCONNECTOR_H
#define TABLEAUTOCONNECTOR_H

#include <QTableWidget>
#include <QListView>

class Component;

//-----------------------------------------------------------------------------
//! Automatically connects items of two components.
//-----------------------------------------------------------------------------
class TableAutoConnector
{
public:

    /*!
     *  The constructor.
     */
    TableAutoConnector() = default;

    /*!
     *  Destructor.
     */
    virtual ~TableAutoConnector() = default;

    /*!
     *  Connect the items from the selected components.
     *
     *      @param [in] selectedTable       The selected connection table.
     *      @param [in] firstComponent      First component.
     *      @param [in] secondComponent     Second component.
     */
    void initializeTable(QTableWidget* selectedTable, QSharedPointer<Component> firstComponent,
        QSharedPointer<Component> secondComponent) const;

    /*!
     *  Remove the items from the selected table.
     */
    void clearTable(QTableWidget* selectedTable) const;

    /*!
     *  Connect the current items from the selected item lists.
     *
     *      @param [in] firstList       View of the first item list.
     *      @param [in] secondList      View of the second item list.
     *      @param [in] targetTable     The connection table.
     */
    void connectSelectedFromLists(QListView* firstList, QListView* secondList, QTableWidget* targetTable) const;

    // No copying. No assignments.
    TableAutoConnector(TableAutoConnector const& rhs) = delete;
    TableAutoConnector& operator=(TableAutoConnector const& rhs) = delete;

protected:

    /*!
     *  Create a table item.
     *
     *      @param [in] itemName                Name for the table item.
     *      @param [in] containingComponent     Component containing the selected item.
     *
     *      @return The new table item.
     */
    virtual QTableWidgetItem* createTableWidgetItem(QString const& itemName,
        QSharedPointer<Component> containingComponent) const;

private:

    /*!
     *  Get the item combinations for the selected items.
     *
     *      @param [in] firstComponent      The first selected item.
     *      @param [in] secondComponent     The second selected item.
     *
     *      @return List of item name pairs.
     */
    QVector<QPair<QString, QString> > getCombinations(QSharedPointer<Component> firstComponent,
        QSharedPointer<Component> secondComponent) const;

    /*!
     *  Find the possible item connections from the selected components.
     *
     *      @param [in] firstComponent      The first component.
     *      @param [in] secondComponent     The second component.
     *
     *      @return List of item / possible connected items pairs.
     */
    virtual QVector<QPair<QString, QVector<QString> > > findPossibleCombinations(
        QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const = 0;

    /*!
     *  Remove existing connections from the selected list of connections.
     *
     *      @param [in] combinations    The selected list of item connections.
     *      @param [in] targetTable     Connection table.
     *
     *      @return List of new connections.
     */
    QVector<QPair<QString, QString> > removeExistingCombinations(
        QVector<QPair<QString, QString> > const& combinations, QTableWidget* targetTable) const;

    /*!
     *  Get text from the selected table item.
     *
     *      @param [in] itemRow         Row of the item.
     *      @param [in] itemColumn      Column of the item.
     *      @param [in] targetTable     Table containing the item.
     *
     *      @return The selected item text.
     */
    QString getTextFromTableItem(int const& itemRow, int const& itemColumn, QTableWidget* targetTable) const;

    /*!
     *  Check if a connection exists in the connection table.
     *
     *      @param [in] combinationFirst        First item of the connection.
     *      @param [in] combinationSecond       Second item of the connection
     *      @param [in] existingCombinations    List of existing combinations.
     *
     *      @return True, if the connection exists in the connection table, false otherwise.
     */
    bool combinationExistsInTable(QString const& combinationFirst, QString const& combinationSecond,
        QVector<QPair<QString, QString> > const& existingCombinations) const;

};

//-----------------------------------------------------------------------------

#endif // TABLEAUTOCONNECTOR_H
