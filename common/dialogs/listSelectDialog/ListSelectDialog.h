//-----------------------------------------------------------------------------
// File: ListSelectDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.12.2011
//
// Description:
// Dialog for making a selection from a list of strings.
//-----------------------------------------------------------------------------

#ifndef LISTSELECTDIALOG_H
#define LISTSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>

//-----------------------------------------------------------------------------
//! ListSelectDialog class.
//-----------------------------------------------------------------------------
class ListSelectDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     */
    ListSelectDialog(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ListSelectDialog();

    /*!
     *  Sets the description text for the list.
     *
     *      @param [in] desc The description text.
     */
    void setDescription(QString const& desc);

    /*!
     *  Adds a new item to the list.
     *
     *      @param [in] item The item to add.
     */
    void addItem(QListWidgetItem* item);

    /*!
     *  Returns the selected item.
     */
    QListWidgetItem* getSelectedItem() const;

private:
    // Disable copying.
    ListSelectDialog(ListSelectDialog const& rhs);
    ListSelectDialog& operator=(ListSelectDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The description label.
    QLabel descLabel_;

    //! The list of possible items to choose from.
    QListWidget itemList_;
};

//-----------------------------------------------------------------------------

#endif // LISTSELECTDIALOG_H
