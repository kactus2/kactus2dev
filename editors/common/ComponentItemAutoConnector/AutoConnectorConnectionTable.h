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
     *      @param [in] firstList   View of the first item list to be connected.
     *      @param [in] secondList  View of the second item list to be connected.
     *      @param [in] firstName   Name of the first item.
     *      @param [in] secondName  Name of the second item.
     *      @param [in] parent      The parent item.
     */
    AutoConnectorConnectionTable(QListView* firstList, QListView* secondList, QString const& firstItemName,
        QString const& secondItemName, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~AutoConnectorConnectionTable();

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
     *  Handles the drop events.
     *
     *      @param [in] event   The drop event.
     */
    virtual void dropEvent(QDropEvent *event);

private slots:

    /*!
     *  Handles the row removal.
     */
    void onRemoveRow();

    /*!
     *  Handles the row addition.
     */
    void onAddRow();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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
};

#endif // AUTOCONNECTORCONNECTIONTABLE_H
