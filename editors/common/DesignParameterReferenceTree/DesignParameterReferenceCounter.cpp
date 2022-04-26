//-----------------------------------------------------------------------------
// File: DesignParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// Calculates the amount of references made to design parameters.
//-----------------------------------------------------------------------------

#include "DesignParameterReferenceCounter.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/AbstractParameterInterface.h>

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::DesignParameterReferenceCounter()
//-----------------------------------------------------------------------------
DesignParameterReferenceCounter::DesignParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder):
ParameterReferenceCounter(parameterFinder),
design_(0)
{

}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::setDesign()
//-----------------------------------------------------------------------------
void DesignParameterReferenceCounter::setDesign(QSharedPointer<Design> newDesign)
{
    design_ = newDesign;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::recalculateReferencesToParameters()
//-----------------------------------------------------------------------------
void DesignParameterReferenceCounter::recalculateReferencesToParameters(QVector<QString> const& parameterList,
    AbstractParameterInterface* parameterInterface)
{
    for (auto parameterName : parameterList)
    {
        int referenceCount = 0;
        QString parameterID = QString::fromStdString(parameterInterface->getID(parameterName.toStdString()));

        referenceCount += countReferencesInParameters(parameterID, design_->getParameters());
        referenceCount += countReferencesInComponentInstances(parameterID);
        referenceCount += countReferencesInAdHocConnections(parameterID, design_->getAdHocConnections());

        parameterInterface->setUsageCount(parameterName.toStdString(), referenceCount);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInComponentInstances()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInComponentInstances(QString const& parameterID) const
{
    int referenceCounter = 0;

    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        referenceCounter += countReferencesInSingleComponentInstance(parameterID, instance);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInSingleComponentInstance()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInSingleComponentInstance(QString const& parameterID,
    QSharedPointer<ComponentInstance> instance) const
{
    return countReferencesInConfigurableElementValues(parameterID, instance->getConfigurableElementValues());
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInAdHocConnections()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInAdHocConnections(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections) const
{
    int referenceCounter = 0;

    foreach (QSharedPointer<AdHocConnection> connection, *connections)
    {
        referenceCounter += countReferencesInSingleAdHocConnection(parameterID, connection);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInSingleAdHocConnection()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInSingleAdHocConnection(QString const& parameterID,
    QSharedPointer<AdHocConnection> connection) const
{
    int referenceCounter = 0;

    if (!connection->getInternalPortReferences()->isEmpty() || !connection->getExternalPortReferences()->isEmpty())
    {
        referenceCounter += countReferencesInExpression(parameterID, connection->getTiedValue());

        foreach (QSharedPointer<PortReference> port, *connection->getInternalPortReferences())
        {
            if (port->getPartSelect())
            {
                referenceCounter += countReferencesInPartSelect(parameterID, port->getPartSelect());
            }
        }
        foreach (QSharedPointer<PortReference> port, *connection->getExternalPortReferences())
        {
            if (port->getPartSelect())
            {
                referenceCounter += countReferencesInPartSelect(parameterID, port->getPartSelect());
            }
        }
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInPartSelect()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInPartSelect(QString const& parameterID,
    QSharedPointer<PartSelect> partSelect) const
{
    int referenceCount = 0;

    if (partSelect)
    {
        referenceCount += countReferencesInExpression(parameterID, partSelect->getLeftRange());
        referenceCount += countReferencesInExpression(parameterID, partSelect->getRightRange());
    }

    return referenceCount;
}
