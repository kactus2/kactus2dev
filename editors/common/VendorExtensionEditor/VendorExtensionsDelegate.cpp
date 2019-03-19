//-----------------------------------------------------------------------------
// File: VendorExtensionsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the vendor extensions.
//-----------------------------------------------------------------------------

#include "VendorExtensionsDelegate.h"

#include <editors/common/VendorExtensionEditor/VendorExtensionColumns.h>

#include <QLineEdit>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::VendorExtensionsDelegate()
//-----------------------------------------------------------------------------
VendorExtensionsDelegate::VendorExtensionsDelegate(QObject* parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::~VendorExtensionsDelegate()
//-----------------------------------------------------------------------------
VendorExtensionsDelegate::~VendorExtensionsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* VendorExtensionsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::TYPE || index.column() == VendorExtensionColumns::VALUE ||
        index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::TYPE || index.column() == VendorExtensionColumns::VALUE ||
        index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: VendorExtensionsDelegate::setModelData()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == VendorExtensionColumns::NAMESPACE || index.column() == VendorExtensionColumns::NAME ||
        index.column() == VendorExtensionColumns::TYPE || index.column() == VendorExtensionColumns::VALUE ||
        index.column() == VendorExtensionColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsdelegate::paint()
//-----------------------------------------------------------------------------
void VendorExtensionsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    if (index.column() != VendorExtensionColumns::DESCRIPTION)
    {
        painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    }

    painter->setPen(oldPen);
}
