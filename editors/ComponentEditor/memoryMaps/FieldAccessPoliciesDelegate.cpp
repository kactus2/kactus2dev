//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "FieldAccessPoliciesDelegate.h"
#include "FieldAccessPolicyColumns.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate()
//-----------------------------------------------------------------------------
FieldAccessPoliciesDelegate::FieldAccessPoliciesDelegate(QWidget* parent):
    QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FieldAccessPoliciesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    //if (index.column() == FieldAccessPolicyColumns::MODE)
    //{
        QLineEdit* lineEdit = new QLineEdit(parent);
        return lineEdit;
    //}
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPoliciesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FieldAccessPoliciesDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto castEditor = qobject_cast<QLineEdit*>(editor);

    castEditor->setText(index.data(Qt::DisplayRole).toString());
}
