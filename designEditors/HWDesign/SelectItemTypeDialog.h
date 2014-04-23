//-----------------------------------------------------------------------------
// File: SelectItemTypeDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.5.2011
//
// Description:
// Item type selection dialog.
//-----------------------------------------------------------------------------

#ifndef SELECTITEMTYPEDIALOG_H
#define SELECTITEMTYPEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QGridLayout>
#include <QGroupBox>

#include <common/ColumnTypes.h>

//-----------------------------------------------------------------------------
//! SelectItemTypeDialog class.
//-----------------------------------------------------------------------------
class SelectItemTypeDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent        The parent widget.
     *      @param [in] allowedItems  The item types from which the user can select from.
     */
    SelectItemTypeDialog(QWidget* parent, unsigned int allowedItems);

    /*!
     *  Destructor.
     */
    ~SelectItemTypeDialog();

    /*!
     *  Returns the selected item type.
     */
    ColumnItemType getSelectedItemType() const;

private:
    // Disable copying.
    SelectItemTypeDialog(SelectItemTypeDialog const& rhs);
    SelectItemTypeDialog& operator=(SelectItemTypeDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The allowed items.
    unsigned int allowedItems_;

    //! The layout for the UI widgets.
    QGridLayout* layout_;

    //! The group box for the allowed column items selection.
    QGroupBox* allowedItemsGroup_;

    //! The item radio buttons.
    QRadioButton* itemRadioButtons_[CIT_COUNT];
};

//-----------------------------------------------------------------------------

#endif // SELECTITEMTYPEDIALOG_H
