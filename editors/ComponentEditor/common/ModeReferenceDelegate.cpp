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

#include <QComboBox>
#include <QLineEdit>

#include <QRegularExpressionValidator>

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
    if (index.column() == 0)
    {
        auto lineEdit = new QLineEdit(parent);

        lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\d*"), parent));
        return lineEdit;
    }
    else if (index.column() == 1)
    {
        auto comboBox = new QComboBox(parent);
        return comboBox;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ModeReferenceDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        if (auto lineEdit = qobject_cast<QLineEdit*>(editor))
        {
            lineEdit->setText(index.data(Qt::DisplayRole).toString());
        }
    }
    else if (index.column() == 1)
    {
        if (auto comboBox = qobject_cast<QComboBox*>(editor))
        {
            comboBox->setCurrentText(index.data(Qt::DisplayRole).toString());
            comboBox->addItems(index.data(Qt::UserRole).toStringList());
            comboBox->addItem(QString());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ModeReferenceDelegate::setModelData()
//-----------------------------------------------------------------------------
void ModeReferenceDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 0)
    {
        if (auto lineEdit = qobject_cast<QLineEdit*>(editor))
        {
            model->setData(index, lineEdit->text(), Qt::EditRole);
        }
    }
    else if (index.column() == 1)
    {
        if (auto comboBox = qobject_cast<QComboBox*>(editor))
        {
            model->setData(index, comboBox->currentText(), Qt::EditRole);
        }
    }
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
