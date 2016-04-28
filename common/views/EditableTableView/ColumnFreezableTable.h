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

//-----------------------------------------------------------------------------
//! Editable table view, with a possibility for freezing a column in place.
//-----------------------------------------------------------------------------
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
     *  Receives viewport events. Updates the frozen column width.
     *
     *      @param [in] event   The viewport event.
     */
    virtual bool viewportEvent(QEvent* event);

private slots:

    /*!
     *  Close the sorting section from the frozen columns.
     */
    void closeSortingSectionInFrozenColumn();

    /*!
     *  Close the sorting section from the main table.
     */
    void closeSortingSectionInMainEditor();

private:

    /*!
     *  Initialize the table.
     */
    void init();

    /*!
     *  Update the geometry of the frozen columns.
     */
    void updateColumnFreezableTableGeometry();

    /*!
     *  Resizes the view port margins of the table.
     */
    void resizeEditorMargins();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The frozen part of the editable table.
    QSharedPointer <EditableTableView> frozenColumns_;

    //! The number of columns that are frozen.
    int numberOfFrozenColumns_;
};

#endif // COLUMNFREEZABLETABLE_H