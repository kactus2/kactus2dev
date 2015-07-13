//-----------------------------------------------------------------------------
// File: ExpressionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.01.2015
//
// Description:
// Base class for delegates providing editors for expressions.
//-----------------------------------------------------------------------------

#include "ExpressionDelegate.h"

#include "ExpressionEditor.h"

#include <QAbstractItemView>
#include <QKeyEvent>

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::ExpressionDelegate()
//-----------------------------------------------------------------------------
ExpressionDelegate::ExpressionDelegate(QCompleter* parameterNameCompleter, 
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent)
    : QStyledItemDelegate(parent), 
    parameterNameCompleter_(parameterNameCompleter), parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::~ExpressionDelegate()
//-----------------------------------------------------------------------------
ExpressionDelegate::~ExpressionDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ExpressionDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    if (columnAcceptsExpression(index.column()))
    {
        return createExpressionEditor(parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ExpressionDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (columnAcceptsExpression(index.column()))
    {
        ExpressionEditor* expressionEditor = qobject_cast<ExpressionEditor*>(editor);
        expressionEditor->setExpression(index.data(Qt::EditRole).toString());
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::setModelData()
//-----------------------------------------------------------------------------
void ExpressionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (columnAcceptsExpression(index.column()))
    {
        ExpressionEditor* expressionEditor = qobject_cast<ExpressionEditor*>(editor);
        expressionEditor->finishEditingCurrentWord();
        model->setData(index, expressionEditor->getExpression(), Qt::EditRole);

        parameterNameCompleter_->popup()->hide();
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::createExpressionEditor()
//-----------------------------------------------------------------------------
QWidget* ExpressionDelegate::createExpressionEditor(QWidget* parent) const
{
    ExpressionEditor* editor = new ExpressionEditor(parameterFinder_, parent);
    editor->setAppendingCompleter(parameterNameCompleter_);

    connect(editor, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(editor, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::eventFilter()
//-----------------------------------------------------------------------------
bool ExpressionDelegate::eventFilter(QObject* editor, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
        {
            QWidget* editorWidget = dynamic_cast<QWidget*>(editor);
            commitData(editorWidget);
            closeEditor(editorWidget);
            return true;
        }
    }

    return QStyledItemDelegate::eventFilter(editor, event);
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::getParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterFinder> ExpressionDelegate::getParameterFinder() const
{
    return parameterFinder_;
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::getNameCompleter()
//-----------------------------------------------------------------------------
QCompleter* ExpressionDelegate::getNameCompleter() const
{
    return parameterNameCompleter_;
}