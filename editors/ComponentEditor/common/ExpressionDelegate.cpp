//-----------------------------------------------------------------------------
// File: ExpressionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 27.01.2015
//
// Description:
// Base class for delegates providing editors for expressions.
//-----------------------------------------------------------------------------

#include "ExpressionDelegate.h"

#include "ExpressionEditor.h"

#include <editors/ComponentEditor/common/ExpressionLineEditor.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <QAbstractItemView>
#include <QKeyEvent>

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::ExpressionDelegate()
//-----------------------------------------------------------------------------
ExpressionDelegate::ExpressionDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
MultilineDescriptionDelegate(parent), 
completionModel_(completionModel),
parameterFinder_(parameterFinder)
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
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
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
        MultilineDescriptionDelegate::setEditorData(editor, index);
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
    }
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::createExpressionEditor()
//-----------------------------------------------------------------------------
ExpressionEditor* ExpressionDelegate::createExpressionEditor(QWidget* parent) const
{
    ExpressionEditor* editor = new ExpressionEditor(parameterFinder_, parent);
    editor->setFrameShadow(QFrame::Plain);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(editor);
    parameterCompleter->setModel(completionModel_);

    editor->setAppendingCompleter(parameterCompleter);

    connect(editor, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(editor, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::createExpressionLineEditor()
//-----------------------------------------------------------------------------
ExpressionLineEditor* ExpressionDelegate::createExpressionLineEditor(QWidget* parent) const
{
    ExpressionLineEditor* editor = new ExpressionLineEditor(parameterFinder_, parent);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(editor);
    parameterCompleter->setModel(completionModel_);

    editor->setAppendingCompleter(parameterCompleter);

    connect(editor, SIGNAL(increaseReference(QString)), this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(editor, SIGNAL(decreaseReference(QString)), this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: ExpressionDelegate::getParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterFinder> ExpressionDelegate::getParameterFinder() const
{
    return parameterFinder_;
}
