//-----------------------------------------------------------------------------
// File: AutoConnectorConnectionTable.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.02.2019
//
// Description:
// Table for connected items of two components.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTORCONNECTIONTABLE_H
#define AUTOCONNECTORCONNECTIONTABLE_H

#include <QTableWidget>
#include <QListView>
#include <QSharedPointer>

class Component;
class TableItemMatcher;

//-----------------------------------------------------------------------------
//! Table for connected items of two components.
//-----------------------------------------------------------------------------
class AutoConnectorConnectionTable : public QTableWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] firstComponent      The first component containing the items to be connected.
     *      @param [in] secondComponent     The second component containing the items to be connected.
     *      @param [in] firstList           View of the first item list to be connected.
     *      @param [in] secondList          View of the second item list to be connected.
     *      @param [in] firstName           Name of the first item.
     *      @param [in] secondName          Name of the second item.
     *      @param [in] itemMatcher         Checks for possible matches between two items.
     *      @param [in] parent              The parent item.
     */
    AutoConnectorConnectionTable(QSharedPointer<Component> firstComponent,
        QSharedPointer<Component> secondComponent, QListView* firstList, QListView* secondList,
        QString const& firstItemName, QString const& secondItemName, TableItemMatcher* itemMatcher,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~AutoConnectorConnectionTable() = default;

    /*!
     *  Get the connected items.
     *
     *      @return A list of connected item pairs.
     */
    QVector<QPair<QString, QString> > getConnectedItems() const;

    // No copying. No assignments.
    AutoConnectorConnectionTable(AutoConnectorConnectionTable const& rhs) = delete;
    AutoConnectorConnectionTable& operator=(AutoConnectorConnectionTable const& rhs) = delete;

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
     *  Handles the events for drag entering this table
     *
     *      @param [in] event   The selected drag enter event.
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);

    /*!
     *  Handles the events for drag moving in this table
     *
     *      @param [in] event   The selected drag move event.
     */
    virtual void dragMoveEvent(QDragMoveEvent *event);

private slots:

    /*!
     *  Handles the row removal.
     */
    void onRemoveRow();

    /*!
     *  Handles the row addition.
     */
    void onAddRow();

    /*!
     *  Handles the cell clearing.
     */
    void onClearCells();

private:

    /*!
     *  Setup the available actions.
     */
    void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component containing the first list of items.
    QSharedPointer<Component> firstComponent_;

    //! Component containing the second list of items.
    QSharedPointer<Component> secondComponent_;

    //! View of the first item list.
    QListView* firstItemList_;

    //! View of the second item list.
    QListView* secondItemList_;

    //! Source view for the drag & drop event.
    QListView* dragSourceList_;

    //! Action for removing rows.
    QAction* removeRowAction_;

    //! Action for adding rows.
    QAction* addRowAction_;

    //! Action for clearing the cells.
    QAction* clearAction_;

    //! Checks for possible matches between two items.
    TableItemMatcher* itemMatcher_;
};

#endif // AUTOCONNECTORCONNECTIONTABLE_H
