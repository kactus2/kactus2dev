//-----------------------------------------------------------------------------
// File: ChoiceCreatorDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2015
//
// Description:
// Delegate for providing functionality for choices inside parameters.
//-----------------------------------------------------------------------------

#include "ChoiceCreatorDelegate.h"

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::ChoiceCreatorDelegate()
//-----------------------------------------------------------------------------
ChoiceCreatorDelegate::ChoiceCreatorDelegate(QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> finder, QObject* parent):
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
// Function: ChoiceCreatorDelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ChoiceCreatorDelegate::findChoice(QModelIndex const& index) const
{
    QString choiceName = choiceNameOnRow(index);
    foreach (QSharedPointer<Choice> currentChoice, *choices_)
    {
        if (currentChoice->name() == choiceName)
        {
            return currentChoice;
        }
    }

    return QSharedPointer<Choice>(new Choice());
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::getChoiceComboItems()
//-----------------------------------------------------------------------------
QStringList ChoiceCreatorDelegate::getChoiceComboItems(QModelIndex const& index) const
{
    QStringList comboItems;

    QSharedPointer<Choice> indexedChoice = findChoice(index);
    for (auto enumeration : *indexedChoice->enumerations())
    {
        QString enumerationText = enumeration->getValue();
        if (!enumeration->getText().isEmpty())
        {
            enumerationText.append(":" + enumeration->getText());
        }

        comboItems.append(enumerationText);
    }

    return comboItems;
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::findEnumerationText()
//-----------------------------------------------------------------------------
QString ChoiceCreatorDelegate::findEnumerationText(QModelIndex const& index, QString const& enumerationValue) const
{
    QSharedPointer<Choice> indexedChoice = findChoice(index);
    for (auto enumeration : *indexedChoice->enumerations())
    {
        if (enumeration->getValue() == enumerationValue)
        {
            return enumeration->getText();
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: ChoiceCreatorDelegate::findEnumerationValue()
//-----------------------------------------------------------------------------
QString ChoiceCreatorDelegate::findEnumerationValue(QModelIndex const& index, QString const& enumerationText) const
{
    QSharedPointer<Choice> indexedChoice = findChoice(index);
    for (auto enumeration : *indexedChoice->enumerations())
    {
        if (enumeration->getText() == enumerationText)
        {
            return enumeration->getValue();
        }
    }

    return QString("");
}
