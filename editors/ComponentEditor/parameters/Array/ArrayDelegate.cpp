//-----------------------------------------------------------------------------
// File: ArrayDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.03.2015
//
// Description:
// Delegate that provides widgets for editing arrays.
//-----------------------------------------------------------------------------

#include "ArrayDelegate.h"
#include "ArrayColumns.h"

#include <IPXACTmodels/Enumeration.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::ArrayDelegate()
//-----------------------------------------------------------------------------
ArrayDelegate::ArrayDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<Choice> selectedChoice, QObject* parent):
ChoiceCreatorDelegate(parameterCompleter, finder, parent),
selectedChoice_(selectedChoice)
{

}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::~ArrayDelegate()
//-----------------------------------------------------------------------------
ArrayDelegate::~ArrayDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ArrayDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
    const
{
    if (isIndexForValueUsingChoice(index))
    {
        return createEnumerationSelector(parent, index);
    }

    else
    {
        return ChoiceCreatorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ArrayDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (isIndexForValueUsingChoice(index))
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboindex = combo->findText(text, Qt::MatchEndsWith);
        if (comboindex == -1)
        {
            comboindex = combo->findText(text, Qt::MatchStartsWith);
        }

        combo->setCurrentIndex(comboindex);
    }

    else
    {
        return ChoiceCreatorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::setModelData()
//-----------------------------------------------------------------------------
void ArrayDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (isIndexForValueUsingChoice(index))
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        text = text.left(text.indexOf(':'));
        model->setData(index, text, Qt::EditRole);
    }

    else
    {
        return ChoiceCreatorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ArrayDelegate::columnAcceptsExpression(int column) const
{
    return column == ArrayColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ArrayDelegate::valueColumn() const
{
    return ArrayColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ArrayDelegate::choiceNameOnRow(QModelIndex const& /*index*/) const
{
    return selectedChoice_->getName();
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ArrayDelegate::findChoice(QModelIndex const& /*index*/) const
{
    return selectedChoice_;
}
