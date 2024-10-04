//-----------------------------------------------------------------------------
// File: BusDefinitionParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 22.08.2024
//
// Description:
// Tree widget for displaying references to a parameter within a bus definition.
//-----------------------------------------------------------------------------


#include "BusDefinitionParameterReferenceTree.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterReferenceTree::BusDefinitionParameterReferenceTree()
//-----------------------------------------------------------------------------
BusDefinitionParameterReferenceTree::BusDefinitionParameterReferenceTree(QSharedPointer<BusDefinition> busDef,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ParameterReferenceCounter> referenceCounter, QWidget* parent):
ParameterReferenceTree(expressionFormatter, parent),
busDef_(busDef),
referenceCounter_(referenceCounter)
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void BusDefinitionParameterReferenceTree::setupTree()
{
    if (!busDef_)
    {
        createTopItem("Bus definition does not exist.");
        return;
    }

    if (referenceCounter_->countReferencesInParameters(getTargetID(), busDef_->getParameters()) > 0)
    {
        QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
        createParameterReferences(busDef_->getParameters(), topParametersItem);
    }

    if (topLevelItemCount() == 0)
    {
        createTopItem("No references found.");
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionParameterReferenceTree::getReferenceCounter()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterReferenceCounter> BusDefinitionParameterReferenceTree::getReferenceCounter() const
{
    return referenceCounter_;
}
