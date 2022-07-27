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

#include <editors/common/DesignParameterReferenceTree/DesignParameterReferenceCounter.h>

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
        QString targetID = getTargetID();

        if (referenceCounter_->countReferencesInParameters(targetID, design_->getParameters()) > 0)
        {
            QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
            createParameterReferences(design_->getParameters(), topParametersItem);
        }

        if (referenceCounter_->countReferencesInComponentInstances(targetID) > 0)
        {
            createReferencesForComponentInstances();
        }

        if (referenceCounter_->countReferencesInAdHocConnections(targetID, design_->getAdHocConnections()) > 0)
        {
            createReferencesForAdHocConnections();
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
// Function: DesignParameterReferenceTree::createReferencesForComponentInstances()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createReferencesForComponentInstances()
{
    QTreeWidgetItem* instancesItem = createTopItem(QString("Component Instances"));

    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        if (referenceCounter_->countReferencesInSingleComponentInstance(getTargetID(), instance) > 0)
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

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::createReferencesForAdHocConnections()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createReferencesForAdHocConnections()
{
    QTreeWidgetItem* connectionsItem = createTopItem(QString("Ad Hoc Connections"));

    QString topComponentName("Top Component");

    QMultiMap<QString, QSharedPointer<AdHocConnection> > connectionsInOrder =
        getAdHocConnectionsInComponentOrder(topComponentName);

    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > topComponentConnections(
        new QList<QSharedPointer<AdHocConnection> >(connectionsInOrder.values(topComponentName)));
    connectionsInOrder.remove(topComponentName);

    QString targetID = getTargetID();

    createAdHocItemsForComponent(topComponentName, topComponentConnections, connectionsItem);

    QStringList referencedInstanceList = connectionsInOrder.keys();
    referencedInstanceList.removeDuplicates();
    foreach (QString componentName, referencedInstanceList)
    {
        QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections(
            new QList<QSharedPointer<AdHocConnection> > (connectionsInOrder.values(componentName)));
        if (!connections->isEmpty())
        {
            createAdHocItemsForComponent(componentName, connections, connectionsItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::createAdHocItemsForComponent()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createAdHocItemsForComponent(QString const& componentName,
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections, QTreeWidgetItem* connectionsItem)
{
    QString targetID = getTargetID();
    if (referenceCounter_->countReferencesInAdHocConnections(targetID, connections) > 0)
    {
        QTreeWidgetItem* componentItem = createMiddleItem(componentName, connectionsItem);

        foreach (QSharedPointer<AdHocConnection> singleConnection, *connections)
        {
            if (referenceCounter_->countReferencesInSingleAdHocConnection(targetID, singleConnection) > 0)
            {
                createReferencesForSingleAdHocConnection(singleConnection, componentItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::DesignParameterReferenceTree()
//-----------------------------------------------------------------------------
QMultiMap<QString, QSharedPointer<AdHocConnection> > DesignParameterReferenceTree::getAdHocConnectionsInComponentOrder(
    QString const& topComponentName) const
{
    QMultiMap<QString, QSharedPointer<AdHocConnection> > connectionsInOder;

    foreach (QSharedPointer<AdHocConnection> connection, *design_->getAdHocConnections())
    {
        if (connection->getInternalPortReferences()->size() == 1 &&
            connection->getExternalPortReferences()->isEmpty())
        {
            QSharedPointer<PortReference> port = connection->getInternalPortReferences()->first();
            connectionsInOder.insert(port->getComponentRef(), connection);
        }
        else if (connection->getExternalPortReferences()->size() == 1 &&
            connection->getInternalPortReferences()->isEmpty())
        {
            connectionsInOder.insert(topComponentName, connection);
        }
    }

    return connectionsInOder;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::createReferencesForSingleAdHocConnection()
//-----------------------------------------------------------------------------
void DesignParameterReferenceTree::createReferencesForSingleAdHocConnection(
    QSharedPointer<AdHocConnection> connection, QTreeWidgetItem* componentItem)
{
    QSharedPointer<PortReference> connectedPort = getTiedValuePort(connection);
    QTreeWidgetItem* portItem = createMiddleItem(connectedPort->getPortRef(), componentItem);

    QString targetID = getTargetID();
    QString tiedValue = connection->getTiedValue();
    if (!tiedValue.isEmpty() && referenceCounter_->countReferencesInExpression(targetID, tiedValue) > 0)
    {
        createItem(QStringLiteral("Tied value"), tiedValue, portItem);
    }

    if (connectedPort && connectedPort->getPartSelect())
    {
        QString portLeftBound = connectedPort->getPartSelect()->getLeftRange();
        if (referenceCounter_->countReferencesInExpression(targetID, portLeftBound) > 0)
        {
            createItem(QStringLiteral("Tied value left bound"), portLeftBound, portItem);
        }

        QString portRightBound = connectedPort->getPartSelect()->getRightRange();
        if (referenceCounter_->countReferencesInExpression(targetID, portRightBound) > 0)
        {
            createItem(QStringLiteral("Tied value right bound"), portRightBound, portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceTree::getTiedValuePort()
//-----------------------------------------------------------------------------
QSharedPointer<PortReference> DesignParameterReferenceTree::getTiedValuePort(
    QSharedPointer<AdHocConnection> connection) const
{
    if (!connection->getInternalPortReferences()->isEmpty())
    {
        return connection->getInternalPortReferences()->first();
    }
    else if (!connection->getExternalPortReferences()->isEmpty())
    {
        return connection->getExternalPortReferences()->first();
    }

    return QSharedPointer<PortReference>();
}