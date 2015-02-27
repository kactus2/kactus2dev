//-----------------------------------------------------------------------------
// File: ColumnFreezableTable.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.02.2015
//
// Description:
// Editable table view, with a possibility for freezing a column in place.
//-----------------------------------------------------------------------------

#ifndef COLUMNFREEZABLETABLE_H
#define COLUMNFREEZABLETABLE_H

#include "editabletableview.h"

#include <QTableView>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSharedPointer>

class ColumnFreezableTable : public EditableTableView
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] numberOfFrozenColumns   The amount of columns that are frozen in the table.
     *      @param [in] frozenColums            Pointer to the table that is being frozen.
     *      @param [in] parent                  Parent of the table.
     */
    ColumnFreezableTable(int numberOfFrozenColumns, QSharedPointer<EditableTableView> frozenColumns, QWidget* parent);

    /*!
     *  The destructor.
     */
    ~ColumnFreezableTable();

    /*!
     *  Set the model class for this view.
     *
     *      @param [in] model   Pointer to the model class, which is displayed by this view.
     */
    virtual void setModel(QAbstractItemModel* model);

    /*!
     *  Enable/disable the import/export csv file functionality.
     *
     *      @param [in] allow   If true then import/export is allowed.
     */
    virtual void setAllowImportExport(bool allow);

    /*!
     *  Set the item delegate fro both the table and the frozen columns.
     *
     *      @param [in] delegateItem    The item delegate to be set.
     */
    void setDelegate(QAbstractItemDelegate *delegateItem);

public slots:

    /*!
     *  Set the default path to use for import / export csv.
     *
     *      @param [in] path    The path to use as default.
     */
    virtual void setDefaultImportExportPath(const QString& path);

protected:

    /*!
     *  Receives resize events. Calls table views resize event, and updates the table geometry.
     *
     *      @param [in] event   Resize events.
     */
    virtual void resizeEvent(QResizeEvent *event);

    /*!
     *  Moves the cursor in accordance with the given action.
     *
     *      @param [in] cursorAction    The given cursor action.
     *      @param [in] modifiers       Modifiers for the action.
     */
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifier modifiers);

    /*!
     *  Scrolls the view if necessary, ensuring that item at index is visible.
     *
     *      @param [in] index   Index of the item.
     *      @param [in] hint    The view will try to position the item according to the given hint.
     */
    void scrollTo (const QModelIndex& index, QAbstractItemView::ScrollHint hint = EnsureVisible);

private slots:

    /*!
     *  Update the width of the section.
     *
     *      @param [in] logicalIndex    The index of the column.
     *      @param [in]                 Needed to accept signal sectionResized.
     *      @param [in] newSize         The new size of the column.
     */
    void updateSectionWidth(int logicalIndex, int, int newSize);

    /*!
     *  Update the height of the section.
     *
     *      @param [in] logicalIndex    The index of the row.
     *      @param [in]                 Needed to accept signal sectionResized.
     *      @param [in] newSize         The new size of the column.
     */
    void updateSectionHeight(int logicalIndex, int, int newSize);

private:

    /*!
     *  Initialize the table.
     */
    void init();

    /*!
     *  Update the geometry of the frozen columns.
     */
    void updateColumnFreezableTableGeometry();

    //! The frozen part of the editable table.
    QSharedPointer <EditableTableView> frozenColumns_;

    //! The number of columns that are frozen.
    int numberOfFrozenColumns_;
};

#endif // COLUMNFREEZABLETABLE_H