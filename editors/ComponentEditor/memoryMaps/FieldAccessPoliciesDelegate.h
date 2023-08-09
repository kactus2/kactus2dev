//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICIESDELEGATE_H
#define FIELDACCESSPOLICIESDELEGATE_H

#include <QStyledItemDelegate>

class FieldAccessPoliciesDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    FieldAccessPoliciesDelegate(QWidget* parent);

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index);

    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;


};

#endif // FIELDACCESSPOLICIESDELEGATE_H
