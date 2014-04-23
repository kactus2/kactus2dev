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

#include <QLabel>
#include <QSpinBox>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::AdHocBoundsDelegate()
//-----------------------------------------------------------------------------
AdHocBoundsDelegate::AdHocBoundsDelegate(QObject *parent /*= 0*/) : QStyledItemDelegate(parent)
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
    // if the column is the one specified for direction items 
    switch (index.column())
    {
    case ADHOC_BOUNDS_COL_NAME:
        {
            QLabel* label = new QLabel(parent);
            return label;
        }

    case ADHOC_BOUNDS_COL_LEFT:
    case ADHOC_BOUNDS_COL_RIGHT:
        {
            QSpinBox* spinBox = new QSpinBox(parent);
            spinBox->setSingleStep(1);
//             connect(spinBox, SIGNAL(editingFinished()),
//                 this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return spinBox;
        }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AdHocBoundsDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    switch (index.column())
    {
    case ADHOC_BOUNDS_COL_NAME:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QLabel* label = qobject_cast<QLabel*>(editor);
            label->setText(text);
            break;
        }

    case ADHOC_BOUNDS_COL_LEFT:
    case ADHOC_BOUNDS_COL_RIGHT:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);

            int value = index.model()->data(index, Qt::DisplayRole).toInt();
            int upperBound = index.model()->data(index, UpperPortBoundRole).toInt();
            int lowerBound = index.model()->data(index, LowerPortBoundRole).toInt();

            spinBox->setRange(lowerBound, upperBound);
            spinBox->setValue(value);
            break;
        }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsDelegate::setModelData()
//-----------------------------------------------------------------------------
void AdHocBoundsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                       QModelIndex const& index) const
{
    switch (index.column())
    {
    case ADHOC_BOUNDS_COL_LEFT:
    case ADHOC_BOUNDS_COL_RIGHT:
        {
            QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
            int value = spinBox->value();
            model->setData(index, value, Qt::EditRole);
            break;
        }

    default:
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            break;
        }
    }
}
