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
ChoiceCreatorDelegate(parameterCompleter, parameterFinder, parent)
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
        return ChoiceCreatorDelegate::createEditor(parent, option, index);
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
        ChoiceCreatorDelegate::setEditorData(editor, index);
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
        ChoiceCreatorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::setChoices()
//-----------------------------------------------------------------------------
void ConfigurableElementDelegate::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    ChoiceCreatorDelegate::setChoices(choices);
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ConfigurableElementDelegate::columnAcceptsExpression(int column) const
{
    return column == ConfigurableElementsColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ConfigurableElementDelegate::choiceNameOnRow(QModelIndex const& index) const
{
    return index.sibling(index.row(), ConfigurableElementsColumns::CHOICE).data(Qt::DisplayRole).toString();
}

//-----------------------------------------------------------------------------
// Function: configurableelementdelegate::valueColumn()
//-----------------------------------------------------------------------------
int ConfigurableElementDelegate::valueColumn() const
{
    return ConfigurableElementsColumns::VALUE;
}
