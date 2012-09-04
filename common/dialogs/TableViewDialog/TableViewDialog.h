//-----------------------------------------------------------------------------
// File: TableViewDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 04.09.2012
//
// Description:
// Generic table view dialog with OK button.
//-----------------------------------------------------------------------------

#ifndef TABLEVIEWDIALOG_H
#define TABLEVIEWDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QLabel>
#include <QAbstractTableModel>

//-----------------------------------------------------------------------------
//! Generic table view dialog with OK button.
//-----------------------------------------------------------------------------
class TableViewDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     */
    TableViewDialog(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~TableViewDialog();

    /*!
     *  Sets the description text for the table view.
     *
     *      @param [in] desc The description text.
     */
    void setDescription(QString const& desc);

    /*!
     *  Sets the model used for the table view.
     *
     *      @param [in] model The model to set.
     */
    void setModel(QAbstractTableModel* model);

private:
    // Disable copying.
    TableViewDialog(TableViewDialog const& rhs);
    TableViewDialog& operator=(TableViewDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Description label.
    QLabel* label_;

    //! The table view.
    QTableView* view_;
};

#endif // TABLEVIEWDIALOG_H
