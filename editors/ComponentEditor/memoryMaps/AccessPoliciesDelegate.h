//-----------------------------------------------------------------------------
// File: AccessPoliciesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Provides editors for access policies.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICIESDELEGATE_H
#define ACCESSPOLICIESDELEGATE_H

#include <QStyledItemDelegate>

class AccessPoliciesDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    AccessPoliciesDelegate(QWidget* parent);

    AccessPoliciesDelegate(AccessPoliciesDelegate const& other) = delete;
    AccessPoliciesDelegate& operator=(AccessPoliciesDelegate const& other) = delete;

    /*!
     *  Create a new editor for the given item
     *
     *      @param [in] parent  Owner for the editor.
     *      @param [in] option  Contains options for the editor.
     *      @param [in] index   Model index identifying the item.
     *
     *      @return The editor to be used to edit the item.
     */
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /*!
     *  Set the data for the editor.
     *
     *      @param [in] editor  The editor where the data is to be set.
     *      @param [in] index   Model index identifying the item that's data is to be set.
     */
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    /*!
     *  Save the data from the editor to the model.
     *
     *      @param [in] editor The editor that contains the data to store.
     *      @param [in] model  Model that contains the data structure where data is to be saved to.
     *      @param [in] index  Model index identifying the item that's data is to be saved.
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    /*!
     *  Updates the editor geometry for a given index.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:

    /*!
     *  Commit the data from the sending editor and close the editor.
     */
    void commitAndCloseEditor();

    /*!
     *  Handle canceling editing.
     */
    void onEditingCanceled();
};

#endif // ACCESSPOLICIESDELEGATE_H



