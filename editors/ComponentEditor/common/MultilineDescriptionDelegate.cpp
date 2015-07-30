//-----------------------------------------------------------------------------
// File: MultilineDescriptionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.07.2015
//
// Description:
// A delegate that provides a multiline editor for descriptions.
//-----------------------------------------------------------------------------

#include "MultilineDescriptionDelegate.h"

#include <QKeyEvent>
#include <QTextEdit>

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::MultilineDescriptionDelegate()
//-----------------------------------------------------------------------------
MultilineDescriptionDelegate::MultilineDescriptionDelegate(QObject* parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::~MultilineDescriptionDelegate()
//-----------------------------------------------------------------------------
MultilineDescriptionDelegate::~MultilineDescriptionDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* MultilineDescriptionDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == descriptionColumn())
    {
        QTextEdit* editor = new QTextEdit(parent);
        editor->setMinimumHeight(120);
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::setEditorData()
//-----------------------------------------------------------------------------
void MultilineDescriptionDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == descriptionColumn())
    {
        QString text = index.data(Qt::EditRole).toString();
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(editor);
        textEdit->setText(text);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::setModelData()
//-----------------------------------------------------------------------------
void MultilineDescriptionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == descriptionColumn())
    {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(editor);
        model->setData(index, textEdit->toPlainText(), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MultilineDescriptionDelegate::eventFilter()
//-----------------------------------------------------------------------------
bool MultilineDescriptionDelegate::eventFilter(QObject* editor, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) && 
            keyEvent->modifiers() == Qt::NoModifier)
        {
            QWidget* editorWidget = dynamic_cast<QWidget*>(editor);
            commitData(editorWidget);
            closeEditor(editorWidget);
            return true;
        }
    }

    return QStyledItemDelegate::eventFilter(editor, event);
}
