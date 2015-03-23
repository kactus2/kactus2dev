//-----------------------------------------------------------------------------
// File: ChoiceCreatorDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.03.2015
//
// Description:
// Delegate for providing functionality for choices inside parameters.
//-----------------------------------------------------------------------------

#include "ChoiceCreatorDelegate.h"

#include <IPXACTmodels/Enumeration.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::ChoiceCreatorDelegate()
//-----------------------------------------------------------------------------
ChoiceCreatorDelegate::ChoiceCreatorDelegate(QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> finder, QObject* parent /* = 0 */):
ExpressionDelegate(parameterCompleter, finder, parent),
choices_(new QList<QSharedPointer<Choice> > ())
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::~ChoiceCreatorDelegate()
//-----------------------------------------------------------------------------
ChoiceCreatorDelegate::~ChoiceCreatorDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::setChoices()
//-----------------------------------------------------------------------------
void ChoiceCreatorDelegate::setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    choices_ = choices;
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::getChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > ChoiceCreatorDelegate::getChoices() const
{
    return choices_;
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::isIndexForValueUsingChoice()
//-----------------------------------------------------------------------------
bool ChoiceCreatorDelegate::isIndexForValueUsingChoice(QModelIndex const& index) const
{
    return index.column() == valueColumn() && !choiceNameOnRow(index).isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::createEnumerationSelector()
//-----------------------------------------------------------------------------
QWidget* ChoiceCreatorDelegate::createEnumerationSelector(QWidget* parent, QModelIndex const& index) const
{
    QComboBox* choiceCombo = new QComboBox(parent);

    QSharedPointer<Choice> selectedChoice = findChoice(index);
    foreach (QSharedPointer<Enumeration> enumeration, *selectedChoice->enumerations())
    {
        QString itemText = enumeration->getValue();
        if (!enumeration->getText().isEmpty())
        {
            itemText.append(":" + enumeration->getText());
        }
        choiceCombo->addItem(itemText);
    }

    return choiceCombo;
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ChoiceCreatorDelegate::findChoice(QModelIndex const& index) const
{
    QString choiceName = choiceNameOnRow(index);
    foreach (QSharedPointer<Choice> currentChoice, *choices_)
    {
        if (currentChoice->getName() == choiceName)
        {
            return currentChoice;
        }
    }

    return QSharedPointer<Choice>(new Choice());
}
