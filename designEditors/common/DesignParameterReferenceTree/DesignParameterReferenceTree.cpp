//-----------------------------------------------------------------------------
// File: DesignParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 28.09.2018
//
// Description:
// Tree widget for displaying references to a parameter within a design.
//-----------------------------------------------------------------------------

#include "DesignParameterReferenceTree.h"

#include <designEditors/common/DesignParameterReferenceTree/DesignParameterReferenceCounter.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::DesignParameterReferenceTree()
//-----------------------------------------------------------------------------
DesignParameterReferenceTree::DesignParameterReferenceTree(QSharedPointer<ExpressionFormatter> formatter,
    QSharedPointer<DesignParameterReferenceCounter> referenceCounter, QWidget* parent):
ParameterReferenceTree(formatter, parent),
design_(0),
referenceCounter_(referenceCounter)
{

}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::~DesignParameterReferenceTree()
//-----------------------------------------------------------------------------
DesignParameterReferenceTree::~DesignParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::setDesign()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::setDesign(QSharedPointer<Design> newDesign)
{
    design_ = newDesign;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::setupTree()
{
    if (design_)
    {
        if (referenceCounter_->countReferencesInParameters(getTargetID(), design_->getParameters()) > 0)
        {
            QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
            createParameterReferences(design_->getParameters(), topParametersItem);
        }

        if (referenceExistsInComponentInstances())
        {
            createReferencesForComponentInstances();
        }

        if (topLevelItemCount() == 0)
        {
            createTopItem("No references found.");
        }
    }
    else
    {
        createTopItem("Design does not exist.");
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::getReferenceCounter()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterReferenceCounter> DesignParameterReferenceTree::getReferenceCounter() const
{
    return referenceCounter_;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::referenceExistsInComponentInstances()
//-----------------------------------------------------------------------------
bool DesignParameterReferenceTree::referenceExistsInComponentInstances() const
{
    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        if (referenceExistsInSingleComponentInstance(instance))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::referenceExistsInSingleComponentInstance()
//-----------------------------------------------------------------------------
bool DesignParameterReferenceTree::referenceExistsInSingleComponentInstance(
    QSharedPointer<ComponentInstance> instance) const
{
    return referenceCounter_->countReferencesInConfigurableElementValues(
        getTargetID(), instance->getConfigurableElementValues()) > 0;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::createReferencesForComponentInstances()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createReferencesForComponentInstances()
{
    QTreeWidgetItem* instancesItem = createTopItem(QString("Component Instances"));

    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        if (referenceExistsInSingleComponentInstance(instance))
        {
            createReferencesForSingleComponentInstance(instance, instancesItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::createReferencesForSingleComponentInstance()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createReferencesForSingleComponentInstance(
    QSharedPointer<ComponentInstance> instance, QTreeWidgetItem* instancesItem)
{
    QTreeWidgetItem* singleInstanceItem = createMiddleItem(instance->getInstanceName(), instancesItem);

    if (referenceCounter_->countReferencesInConfigurableElementValues(
        getTargetID(), instance->getConfigurableElementValues()) > 0)
    {
        createReferencesForConfigurableElementValues(instance->getConfigurableElementValues(), singleInstanceItem);
    }
}
