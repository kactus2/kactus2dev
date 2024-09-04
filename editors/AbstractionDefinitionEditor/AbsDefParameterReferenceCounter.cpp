//-----------------------------------------------------------------------------
// File: AbsDefParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.08.2024
//
// Description:
// Calculates the amount of references made to abstraction definition parameters.
//-----------------------------------------------------------------------------

#include "AbsDefParameterReferenceCounter.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/AbstractParameterInterface.h>


//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::AbsDefParameterReferenceCounter()
//-----------------------------------------------------------------------------
AbsDefParameterReferenceCounter::AbsDefParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<AbstractionDefinition> absDef):
    ParameterReferenceCounter(parameterFinder),
    absDef_(absDef)
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::setAbstractionDefinition()
//-----------------------------------------------------------------------------
void AbsDefParameterReferenceCounter::setAbstractionDefinition(QSharedPointer<AbstractionDefinition> absDef)
{
    absDef_ = absDef;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::countReferencesInPorts()
//-----------------------------------------------------------------------------
int AbsDefParameterReferenceCounter::countReferencesInWirePorts(QString const& parameterID)
{
    int references = 0;

    if (!absDef_)
    {
        return 0;
    }

    for (auto const& port : absDef_->getPortsWithWire())
    {
        references += countReferencesInWirePort(parameterID, port->getWire());
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::countReferencesInTransactionalPorts()
//-----------------------------------------------------------------------------
int AbsDefParameterReferenceCounter::countReferencesInTransactionalPorts(QString const& parameterID)
{
    int references = 0;

    if (!absDef_)
    {
        return 0;
    }

    for (auto const& port : absDef_->getPortsWithTransactional())
    {
        references += countReferencesInTransactionalPort(parameterID, port->getTransactional());
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::countReferencesInWirePort()
//-----------------------------------------------------------------------------
int AbsDefParameterReferenceCounter::countReferencesInWirePort(QString const& parameterID, QSharedPointer<WireAbstraction> wireAbstraction)
{
    int references = 0;

    references += countReferencesInExpression(parameterID, wireAbstraction->getDefaultValue());

    auto initiatorMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::INITIATOR
        : General::InterfaceMode::MASTER;

    auto targetMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::TARGET
        : General::InterfaceMode::SLAVE;

    if (wireAbstraction->hasInitiatorPort())
    {
        references += countReferencesInExpression(parameterID, wireAbstraction->getWidth(initiatorMode, parameterID));
    }
    
    if (wireAbstraction->hasTargetPort())
    {
        references += countReferencesInExpression(parameterID, wireAbstraction->getWidth(targetMode, parameterID));
    }
    
    if (wireAbstraction->getSystemPorts()->isEmpty() == false)
    {
        for (auto const& systemPort : *wireAbstraction->getSystemPorts())
        {
            references += countReferencesInExpression(parameterID, systemPort->getWidth());
        }
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::countReferencesInTransactionalPort()
//-----------------------------------------------------------------------------
int AbsDefParameterReferenceCounter::countReferencesInTransactionalPort(QString const& parameterID, QSharedPointer<TransactionalAbstraction> transactional)
{
    int references = 0;

    auto initiatorMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::INITIATOR
        : General::InterfaceMode::MASTER;

    auto targetMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::TARGET
        : General::InterfaceMode::SLAVE;

    if (transactional->hasInitiatorPort())
    {
        references += countReferencesInExpression(parameterID, transactional->getWidth(initiatorMode, parameterID));
    }
    
    if (transactional->hasTargetPort())
    {
        references += countReferencesInExpression(parameterID, transactional->getWidth(targetMode, parameterID));
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceCounter::recalculateReferencesToParameters()
//-----------------------------------------------------------------------------
void AbsDefParameterReferenceCounter::recalculateReferencesToParameters(QStringList const& parameterList, AbstractParameterInterface* parameterInterface)
{
    for (auto const& parameterName : parameterList)
    {
        int referenceCount = 0;
        QString parameterID = QString::fromStdString(parameterInterface->getID(parameterName.toStdString()));
        if (!parameterID.isEmpty())
        {
            referenceCount += countReferencesInWirePorts(parameterID);
            referenceCount += countReferencesInTransactionalPorts(parameterID);
            referenceCount += countReferencesInParameters(parameterID, absDef_->getParameters());

            parameterInterface->setUsageCount(parameterName.toStdString(), referenceCount);
        }
    }
}
