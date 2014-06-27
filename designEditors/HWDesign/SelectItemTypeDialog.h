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
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

#include <designEditors/common/ColumnTypes.h>

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
     *      @param [in] allowedItems  The item types from which the user can select from.
     *      @param [in] parent        The parent widget.
     */
    SelectItemTypeDialog(unsigned int allowedItems, QWidget* parent);

    //! The destructor.
    ~SelectItemTypeDialog();

    //!  Returns the selected item type.
    ColumnItemType getSelectedItemType() const;

private:
    // Disable copying.
    SelectItemTypeDialog(SelectItemTypeDialog const& rhs);
    
    //! Disable assignment.
    SelectItemTypeDialog& operator=(SelectItemTypeDialog const& rhs);
    
    //! Sets the dialog layout.
    void setupLayout();

    //! Sets the label for selecting component/channel/bridge according to allowed item types.
    void setComponentButtonLabel();    

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The allowed items.
    unsigned int allowedItems_;

    //! The layout for the UI widgets.
    QVBoxLayout* layout_;

    //! The group box for the allowed column items selection.
    QGroupBox* allowedItemsGroup_;

    //! The radio button for selecting interface.
    QRadioButton* interfaceButton_;

    //! The radio button for selecting component/channel/bridge.
    QRadioButton* componentButton_;
    
    //! Dialog OK and Cancel buttons.
    QDialogButtonBox* dialogButtons_;
};

//-----------------------------------------------------------------------------

#endif // SELECTITEMTYPEDIALOG_H
