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
ExpressionDelegate(parameterCompleter, finder, parent),
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
        return createEnumerationSelector(parent);
    }

    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
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
        return ExpressionDelegate::setEditorData(editor, index);
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
        return ExpressionDelegate::setModelData(editor, model, index);
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
// Function: ArrayDelegate::isIndexForValueUsingChoice()
//-----------------------------------------------------------------------------
bool ArrayDelegate::isIndexForValueUsingChoice(QModelIndex const& index) const
{
    return index.column() == ArrayColumns::VALUE && !selectedChoice_->getName().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ArrayDelegate::createEnumerationSelector()
//-----------------------------------------------------------------------------
QWidget* ArrayDelegate::createEnumerationSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);

    if (!selectedChoice_.isNull())
    {
        foreach(QSharedPointer<Enumeration> enumeration, *selectedChoice_->enumerations())
        {
            QString itemText = enumeration->getValue();
            if (!enumeration->getText().isEmpty())
            {
                itemText.append(":" + enumeration->getText());
            }

            combo->addItem(itemText);
        }
    }

    return combo;
}
