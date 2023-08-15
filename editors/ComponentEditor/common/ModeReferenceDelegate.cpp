//-----------------------------------------------------------------------------
// File: ModeReferenceDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 10.8.2023
//
// Description:
// Delegate that provides editors for editing mode references.
//-----------------------------------------------------------------------------

#include "ModeReferenceDelegate.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::ModeReferenceDelegate()
//-----------------------------------------------------------------------------
ModeReferenceDelegate::ModeReferenceDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ModeReferenceDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto lineEdit = new QLineEdit(parent);
    return lineEdit;
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ModeReferenceDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (auto lineEdit = qobject_cast<QLineEdit*>(editor); lineEdit)
    {
        lineEdit->setText(index.data(Qt::DisplayRole).toString());
    }
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::setModelData()
//-----------------------------------------------------------------------------
void ModeReferenceDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto lineEdit = qobject_cast<QLineEdit*>(editor);

    if (!lineEdit)
    {
        return;
    }

    model->setData(index, lineEdit->text(), Qt::EditRole);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ModeReferenceDelegate::commitAndCloseEditor()
{
    auto editor = qobject_cast<QWidget*>(sender());

    if (editor)
    {
        emit commitData(editor);
        emit closeEditor(editor);
    }
}
