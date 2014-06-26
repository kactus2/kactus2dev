//-----------------------------------------------------------------------------
// File: ColumnEditDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.4.2011
//
// Description:
// Dialog for editing diagram column properties.
//-----------------------------------------------------------------------------

#ifndef COLUMNEDITDIALOG_H
#define COLUMNEDITDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <common/graphicsItems/GraphicsColumn.h>

//-----------------------------------------------------------------------------
//! ColumnEditDialog class.
//-----------------------------------------------------------------------------
class ColumnEditDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent  The parent widget.
     *      @param [in] sw      If true, the dialog shows only options valid for SW designs.
     *      @param [in] column  The existing column to edit.
     */
    ColumnEditDialog(QWidget* parent, bool sw, GraphicsColumn const* column = 0);

    /*!
     *  Destructor.
     */
    virtual ~ColumnEditDialog();


    /*!
     *  Hides the column content settings.
     */
    void hideContentSettings();

    /*!
     *  Returns the column name from the dialog.
     */
    QString getName() const;

    /*!
     *  Returns the column content type from the dialog.
     */
    ColumnContentType getContentType() const;

    /*!
     *  Returns the allowed items from the dialog.
     */
    unsigned int getAllowedItems() const;

private slots:
    //! Called when the content type has changed.
    void onTypeChange(QString const& type);

    //! Called when OK button has been clicked.
    virtual void accept();

private:
    // Disable copying.
    ColumnEditDialog(ColumnEditDialog const& rhs);
    
    // Disable assignment.
    ColumnEditDialog& operator=(ColumnEditDialog const& rhs);

    /*!
     *  Initializes the selectable types.
     *
     *      @param [in] sw   True if editing sw columns.     
     */
    void initializeTypes(bool sw);

    /*!
     *  Sets the dialog layout.
     *
     *      @param [in] sw   True if editing sw columns.     
     */
    void setupLayout(bool sw);

    /*!
     *  Sets the allowed items in the dialog.
     *
     *      @param [in] allowedItems The allowed items. See ComponentItemType for possible values.
     */
    void setAllowedItems(unsigned int allowedItems);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The layout for the UI widgets.
    QVBoxLayout* layout_;

    //! The label for the name line edit.
    QLabel* nameLabel_;

    //! The line edit for the column name.
    QLineEdit* nameEdit_;

    //! The label for the type combo box.
    QLabel* typeLabel_;

    //! The combo box for the column type.
    QComboBox* typeCombo_;

    //! The group box for the allowed column items selection.
    QGroupBox* allowedItemsGroup_;

    //! The check boxes for the allowed column items.
    QCheckBox* itemCheckBoxes_[CIT_COUNT];

    //! Dialog buttons.
    QDialogButtonBox* buttons_;

    //! The column that is being edited. If null, we're creating a new column.
    GraphicsColumn const* column_;

    //! The names and types of possible columns.
    QMap<QString, ColumnContentType> types_;
};

//-----------------------------------------------------------------------------

#endif // COLUMNEDITDIALOG_H