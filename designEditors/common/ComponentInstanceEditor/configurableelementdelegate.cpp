/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.cpp
 *		Project: Kactus 2
 */

#include "configurableelementdelegate.h"
#include "ConfigurableElementsColumns.h"

#include <IPXACTmodels/Enumeration.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::ConfigurableElemenetDelegate()
//-----------------------------------------------------------------------------
ConfigurableElementDelegate::ConfigurableElementDelegate(QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterCompleter, parameterFinder, parent),
choices_(new QList<QSharedPointer<Choice> > ())
{

}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::~ConfigurableElementDelegate()
//-----------------------------------------------------------------------------
ConfigurableElementDelegate::~ConfigurableElementDelegate() 
{

}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ConfigurableElementDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (isIndexForValueUsingChoice(index))
    {
        return createEnumerationSelector(parent, index);
    }

    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setEditorData()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (isIndexForValueUsingChoice(index))
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text, Qt::MatchEndsWith);
        if (comboIndex == -1)
        {
            comboIndex = combo->findText(text, Qt::MatchStartsWith);
        }
        combo->setCurrentIndex(comboIndex);
    }

    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setModelData()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    QModelIndex const& index) const
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
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setChoices()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    choices_ = choices;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ConfigurableElementDelegate::columnAcceptsExpression(int column) const
{
    return column == ConfigurableElementsColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::isIndexForValueUsingChoice()
//-----------------------------------------------------------------------------
bool ConfigurableElementDelegate::isIndexForValueUsingChoice(QModelIndex const& index) const
{
    return index.column() == ConfigurableElementsColumns::VALUE && !getChoiceNameAtIndex(index).isEmpty();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::getChoiceNameAtIndex()
//-----------------------------------------------------------------------------
QString ConfigurableElementDelegate::getChoiceNameAtIndex(QModelIndex const& index) const
{
    return index.sibling(index.row(), ConfigurableElementsColumns::CHOICE).data(Qt::DisplayRole).toString();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::createEnumerationSelector()
//-----------------------------------------------------------------------------
QWidget* ConfigurableElementDelegate::createEnumerationSelector(QWidget* parent, QModelIndex const& index) const
{
    QComboBox* combo = new QComboBox(parent);

    QSharedPointer<Choice> selectedChoice = findChoice(index);
    foreach (QSharedPointer<Enumeration> enumeration, *selectedChoice->enumerations())
    {
        QString itemText = enumeration->getValue();
        if (!enumeration->getText().isEmpty())
        {
            itemText.append(":" + enumeration->getText());
        }
        combo->addItem(itemText);
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ConfigurableElementDelegate::findChoice(QModelIndex const& index) const
{
    QString choiceName = getChoiceNameAtIndex(index);
    foreach (QSharedPointer<Choice> currentChoice, *choices_)
    {
        if (currentChoice->getName() == choiceName)
        {
            return currentChoice;
        }
    }

    return QSharedPointer<Choice>(new Choice());
}