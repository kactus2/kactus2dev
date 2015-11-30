//-----------------------------------------------------------------------------
// File: AdHocBoundsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#include "AdHocBoundsDelegate.h"

#include "AdHocBoundColumns.h"

#include <QLabel>
#include <QSpinBox>

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::AdHocBoundsDelegate()
//-----------------------------------------------------------------------------
AdHocBoundsDelegate::AdHocBoundsDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::~AdHocBoundsDelegate()
//-----------------------------------------------------------------------------
AdHocBoundsDelegate::~AdHocBoundsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AdHocBoundsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_NAME)
    {
        return new QLabel(parent);
    }
    else if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT || 
        index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
    {
        return new QSpinBox(parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AdHocBoundsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_NAME)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QLabel* label = qobject_cast<QLabel*>(editor);
        label->setText(text);
    }
    else if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT || 
        index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
    {
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);

        int value = index.data(Qt::DisplayRole).toInt();
        int upperBound = index.model()->data(index, AdHocBoundColumns::UpperPortBoundRole).toInt();
        int lowerBound = index.model()->data(index, AdHocBoundColumns::LowerPortBoundRole).toInt();

        spinBox->setRange(lowerBound, upperBound);
        spinBox->setValue(value);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::setModelData()
//-----------------------------------------------------------------------------
void AdHocBoundsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_LEFT || 
        index.column() == AdHocBoundColumns::ADHOC_BOUNDS_COL_RIGHT)
    {
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
        model->setData(index, spinBox->value(), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
