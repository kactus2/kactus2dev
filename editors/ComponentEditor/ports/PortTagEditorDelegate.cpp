//-----------------------------------------------------------------------------
// File: PortTagEditorDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.07.2015
//
// Description:
// Delegate for selecting and creating tags for ports.
//-----------------------------------------------------------------------------

#include "PortTagEditorDelegate.h"

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: PortTagEditorDelegate::PortTagEditorDelegate()
//-----------------------------------------------------------------------------
PortTagEditorDelegate::PortTagEditorDelegate(QStringList existingPortTags, QObject* parent):
QStyledItemDelegate(parent),
existingTags_(existingPortTags)
{

}

//-----------------------------------------------------------------------------
// Function: PortTagEditorDelegate::~PortTagEditorDelegate()
//-----------------------------------------------------------------------------
PortTagEditorDelegate::~PortTagEditorDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: PortTagEditorDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortTagEditorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QComboBox* tagCombo = new QComboBox(parent);
    tagCombo->setEditable(true);
    tagCombo->addItems(existingTags_);

    return tagCombo;
}

//-----------------------------------------------------------------------------
// Function: PortTagEditorDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortTagEditorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString text = index.model()->data(index, Qt::DisplayRole).toString();

    QComboBox* tagCombo = qobject_cast<QComboBox*>(editor);

    if (QString::compare(text, "Double click to add new item.") != 0)
    {
        tagCombo->setCurrentText(text);
    }
    else
    {
        tagCombo->setCurrentText(QString());
    }
}

//-----------------------------------------------------------------------------
// Function: PortTagEditorDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortTagEditorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* tagCombo = qobject_cast<QComboBox*>(editor);
    QString text = tagCombo->currentText();
    model->setData(index, text, Qt::EditRole);
}
