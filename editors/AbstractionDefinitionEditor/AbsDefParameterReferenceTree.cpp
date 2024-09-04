//-----------------------------------------------------------------------------
// File: AbsDefParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.08.2024
//
// Description:
// Tree widget for displaying references to a parameter within an abstraction definition.
//-----------------------------------------------------------------------------

#include "AbsDefParameterReferenceTree.h"

#include "AbsDefParameterReferenceCounter.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceTree::AbsDefParameterReferenceTree()
//-----------------------------------------------------------------------------
AbsDefParameterReferenceTree::AbsDefParameterReferenceTree(QSharedPointer<AbstractionDefinition> absDef,
    QSharedPointer<AbsDefParameterReferenceCounter> referenceCounter, 
    QSharedPointer<ExpressionFormatter> expressionFormatter) :
ParameterReferenceTree(expressionFormatter),
referenceCounter_(referenceCounter),
absDef_(absDef)
{

}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceTree::setAbsDef()
//-----------------------------------------------------------------------------
void AbsDefParameterReferenceTree::setAbsDef(QSharedPointer<AbstractionDefinition> absDef)
{
    absDef_ = absDef;
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void AbsDefParameterReferenceTree::setupTree()
{
    if (!absDef_)
    {
        createTopItem("Abstraction definition does not exist.");
        return;
    }

    QString const& targetId = getTargetID();

    // Create references in other parameters.
    if (referenceCounter_->countReferencesInParameters(targetId, absDef_->getParameters()) > 0)
    {
        QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
        createParameterReferences(absDef_->getParameters(), topParametersItem);
    }

    int referencesInWirePorts = referenceCounter_->countReferencesInWirePorts(targetId);
    int referencesInTransactionalPorts = referenceCounter_->countReferencesInTransactionalPorts(targetId);

    auto initiatorMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::INITIATOR
        : General::InterfaceMode::MASTER;

    auto targetMode = absDef_->getRevision() == Document::Revision::Std22
        ? General::InterfaceMode::TARGET
        : General::InterfaceMode::SLAVE;

    // Create references in ports, if there are any.
    if (referencesInWirePorts + referencesInTransactionalPorts > 0)
    {
        auto topPortsItem = createTopItem("Ports");
        
        if (referencesInWirePorts > 0)
        {
            auto wirePortsItem = createMiddleItem(QStringLiteral("Wires"), topPortsItem);

            for (auto const& portWithWire : absDef_->getPortsWithWire())
            {
                auto wireAbstraction = portWithWire->getWire();
                
                auto singleWirePortItem = createMiddleItem(portWithWire->name(), wirePortsItem);

                if (portWithWire->getDefaultValue().contains(targetId))
                {
                    createItem(QStringLiteral("Default value"), portWithWire->getDefaultValue(), singleWirePortItem);
                }

                if (wireAbstraction->hasMode(initiatorMode, QString()) && 
                    wireAbstraction->getInitiatorPort()->getWidth().contains(targetId))
                {
                    auto initiatorItem = createMiddleItem(QStringLiteral("Initiator"), singleWirePortItem);
                    createItem(QStringLiteral("Width"), targetId, initiatorItem);
                }

                if (wireAbstraction->hasMode(targetMode, QString()) &&
                    wireAbstraction->getTargetPort()->getWidth().contains(targetId))
                {
                    auto targetItem = createMiddleItem(QStringLiteral("Target"), singleWirePortItem);
                    createItem(QStringLiteral("Width"), targetId, targetItem);
                }

                for (auto const& systemPort : *wireAbstraction->getSystemPorts())
                {
                    if (systemPort->getWidth().contains(targetId))
                    {
                        auto systemItem = createMiddleItem(tr("System (%1)").arg(systemPort->getSystemGroup()), singleWirePortItem);
                        createItem(QStringLiteral("Width"), systemPort->getWidth(), systemItem);
                    }
                }
            }
        }

        if (referencesInTransactionalPorts > 0)
        {
            auto transactionalPortsItem = createMiddleItem(QStringLiteral("Transactionals"), topPortsItem);
            
            for (auto const& portWithTransactional : absDef_->getPortsWithTransactional())
            {
                auto transactionalAbstraction = portWithTransactional->getTransactional();
                
                auto singleTransactionalPortItem = createMiddleItem(portWithTransactional->name(), transactionalPortsItem);

                if (transactionalAbstraction->hasMode(initiatorMode, QString()) &&
                    transactionalAbstraction->getInitiatorPort()->getBusWidth().contains(targetId))
                {
                    auto initiatorItem = createMiddleItem(QStringLiteral("Initiator"), singleTransactionalPortItem);
                    createItem(QStringLiteral("Bus width"), targetId, initiatorItem);
                }

                if (transactionalAbstraction->hasMode(targetMode, QString()) &&
                    transactionalAbstraction->getTargetPort()->getBusWidth().contains(targetId))
                {
                    auto targetItem = createMiddleItem(QStringLiteral("Target"), singleTransactionalPortItem);
                    createItem(QStringLiteral("Bus width"), targetId, targetItem);
                }

                for (auto const& systemPort : *transactionalAbstraction->getSystemPorts())
                {
                    if (systemPort->getBusWidth().contains(targetId))
                    {
                        auto systemItem = createMiddleItem(tr("System (%1)").arg(systemPort->getSystemGroup()), singleTransactionalPortItem);
                        createItem(QStringLiteral("Bus width"), systemPort->getBusWidth(), systemItem);
                    }
                }
            }
        }
    }

    if (topLevelItemCount() == 0)
    {
        createTopItem("No references found.");
    }
}

//-----------------------------------------------------------------------------
// Function: AbsDefParameterReferenceTree::getReferenceCounter()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterReferenceCounter> AbsDefParameterReferenceTree::getReferenceCounter() const
{
    return referenceCounter_;
}
