//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying references to a parameter.
//-----------------------------------------------------------------------------

#include "ParameterReferenceTree.h"

#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/Parameter.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::ParameterReferenceTree(QSharedPointer<ExpressionFormatter> formatter, QWidget* parent):
QTreeWidget(parent),
targetID_(""),
expressionFormatter_(formatter)
{
    QStringList labels;
    labels << QStringList("Referencing elements") << QString("Referencing expression");
    setHeaderLabels(labels);

    setColumnCount(COLUMN_COUNT);
    setColumnWidth(ITEM_NAME, 240);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::~ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::~ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::getTargetID()
//-----------------------------------------------------------------------------
QString ParameterReferenceTree::getTargetID() const
{
    return targetID_;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::openReferenceTree()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::openReferenceTree(QString const& parameterID)
{
    targetID_ = parameterID;

    clear();
    setupTree();
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) const
{
    foreach (QSharedPointer<Parameter> parameter, *parameterList)
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::parameterHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::parameterHasReference(QSharedPointer<Parameter> parameter) const
{
    if (parameter->getValue().contains(targetID_) ||
        parameter->getVectorLeft().contains(targetID_) || parameter->getVectorRight().contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayLeft").contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createParameterReferences()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createParameterReferences(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<Parameter> parameter, *parameters)
    {
        if (parameterHasReference(parameter))
        {
            QTreeWidgetItem* parameterItem = createMiddleItem(parameter->name(), parentItem);
            createItemsForParameter(parameter, parameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInConfigurableElementValues()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elementValues) const
{
    foreach (QSharedPointer<ConfigurableElementValue> element, *elementValues)
    {
        if (referenceExistsInSingleConfigurableElementValue(element))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSingleConfigurableElementValue()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleConfigurableElementValue(
    QSharedPointer<ConfigurableElementValue> element) const
{
    return element->getConfigurableValue().contains(targetID_);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForConfigurableElementValues()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForConfigurableElementValues(
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elements, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* elementsItem = createMiddleItem("Configurable Element Values", parent);
    colourItemGrey(elementsItem);

    foreach (QSharedPointer<ConfigurableElementValue> element, *elements)
    {
        if (referenceExistsInSingleConfigurableElementValue(element))
        {
            createItem(QString("Configurable Element"), element->getConfigurableValue(), elementsItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createTopItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createTopItem(QString const& itemName)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(ITEM_NAME, itemName);

    addTopLevelItem(newItem);

    colourItemGrey(newItem);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createMiddleItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createMiddleItem(QString const& itemnName, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemnName);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForParameter()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForParameter(QSharedPointer<Parameter> parameter, QTreeWidgetItem* parent)
{
    if (parameter->getValue().contains(targetID_))
    {
        createItem("Value", parameter->getValue(), parent);
    }
    if (parameter->getVectorLeft().contains(targetID_))
    {
        createItem("Bit Width Left", parameter->getVectorLeft(), parent);
    }
    if (parameter->getVectorRight().contains(targetID_))
    {
        createItem("Bit Width Right", parameter->getVectorRight(), parent);
    }
    if (parameter->getAttribute("kactus2:arrayLeft").contains(targetID_))
    {
        createItem("Array Left", parameter->getAttribute("kactus2:arrayLeft"), parent);
    }
    if (parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        createItem("Array Right", parameter->getAttribute("kactus2:arrayRight"), parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItem()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItem
    (QString const& itemName, QString const& expression, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemName);
    newItem->setText(ITEM_EXPRESSION, expressionFormatter_->formatReferringExpression(expression));
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::colourItemGrey()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::colourItemGrey(QTreeWidgetItem* item)
{
    for (int columnIndex = 0; columnIndex < COLUMN_COUNT; ++columnIndex)
    {
        item->setBackgroundColor(columnIndex, KactusColors::STRONG_FIELD);
    }
}
