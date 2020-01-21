//-----------------------------------------------------------------------------
// File: AbstractionExtendHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2020
//
// Description:
// Handles the extend ports of abstraction definition.
//-----------------------------------------------------------------------------

#include "AbstractionExtendHandler.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::removeExtendSignalFromAbstraction()
//-----------------------------------------------------------------------------
bool AbstractionExtendHandler::removeExtendSignalFromAbstraction(AbstractionDefinitionSignalRow const& signal,
    QList<QSharedPointer<PortAbstraction>> const& extendPorts, QSharedPointer<AbstractionDefinition> abstraction)
{
    bool removeSignal = false;
    if (signalRowIsExtend(signal, extendPorts))
    {
        QSharedPointer<PortAbstraction> signalAbstraction = signal.getPortAbstraction();
        if (abstraction->getLogicalPorts()->contains(signalAbstraction))
        {
            abstraction->getLogicalPorts()->removeAll(signalAbstraction);
        }

        removeSignal = true;
    }

    return removeSignal;
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::signalRowIsExtend()
//-----------------------------------------------------------------------------
bool AbstractionExtendHandler::signalRowIsExtend(AbstractionDefinitionSignalRow const& signal,
    QList<QSharedPointer<PortAbstraction>> const& extendPorts)
{
    for (auto extendPort : extendPorts)
    {
        if (signal.getPortAbstraction()->name() == extendPort->name())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::getEditedExtendSignals()
//-----------------------------------------------------------------------------
QVector<AbstractionDefinitionSignalRow> AbstractionExtendHandler::getEditedExtendSignals(
    QVector<AbstractionDefinitionSignalRow> const& oldExtendSignals,
    QList<QSharedPointer<PortAbstraction> > const& newExtends)
{
    QVector<AbstractionDefinitionSignalRow> existingSignals = oldExtendSignals;
    for (auto extendPort : newExtends)
    {
        QSharedPointer<WireAbstraction> extendWire = extendPort->getWire();
        QSharedPointer<TransactionalAbstraction> extendTransactional = extendPort->getTransactional();

        for (auto signalRow : oldExtendSignals)
        {
            QSharedPointer<PortAbstraction> oldAbstraction = signalRow.getPortAbstraction();
            if (oldAbstraction->name() == extendPort->name())
            {
                bool signalIsRedifined = false;

                if (extendPort->hasWire())
                {
                    QSharedPointer<WireAbstraction> oldWire = oldAbstraction->getWire();
                    extendWire->setDefaultValue(oldWire->getDefaultValue());
                    extendWire->setDriverType(oldWire->getDriverType());

                    if (signalRow.getMode() == General::MASTER && extendWire->hasMasterPort())
                    {
                        QSharedPointer<WirePort> extendWirePort = extendWire->getMasterPort();
                        extendWirePort->setWidth(signalRow.getWire()->getWidth());
                        extendWirePort->setPresence(signalRow.getWire()->getPresence());

                        signalIsRedifined = true;
                    }
                    else if (signalRow.getMode() == General::SLAVE && extendWire->hasSlavePort())
                    {
                        QSharedPointer<WirePort> extendWirePort = extendWire->getSlavePort();
                        extendWirePort->setWidth(signalRow.getWire()->getWidth());
                        extendWirePort->setPresence(signalRow.getWire()->getPresence());

                        signalIsRedifined = true;
                    }
                    else if (signalRow.getMode() == General::SYSTEM && extendWire->getSystemPorts() &&
                        extendWire->getSystemPorts()->size() > 0)
                    {
                        QSharedPointer<WirePort> extendWirePort =
                            extendWire->findSystemPort(signalRow.getWire()->getSystemGroup());
                        if (extendWirePort)
                        {
                            extendWirePort->setWidth(signalRow.getWire()->getWidth());
                            extendWirePort->setPresence(signalRow.getWire()->getPresence());

                            signalIsRedifined = true;
                        }
                    }
                }

                if (extendPort->hasTransactional())
                {
                    if (signalRow.getMode() == General::MASTER && extendTransactional->hasMasterPort())
                    {
                        extendTransactional->getMasterPort()->setPresence(
                            signalRow.getTransactional()->getPresence());

                        signalIsRedifined = true;
                    }
                    else if (signalRow.getMode() == General::SLAVE && extendTransactional->hasSlavePort())
                    {
                        extendTransactional->getMasterPort()->setPresence(
                            signalRow.getTransactional()->getPresence());

                        signalIsRedifined = true;
                    }
                    else if (signalRow.getMode() == General::SYSTEM && extendWire->getSystemPorts() &&
                        extendWire->getSystemPorts()->size() > 0)
                    {
                        QSharedPointer<TransactionalPort> extendTransactionalPort =
                            extendTransactional->findSystemPort(signalRow.getTransactional()->getSystemGroup());
                        if (extendTransactionalPort)
                        {
                            extendTransactionalPort->setPresence(signalRow.getTransactional()->getPresence());

                            signalIsRedifined = true;
                        }
                    }
                }

                if (signalIsRedifined)
                {
                    existingSignals.removeAll(signalRow);
                }
                else
                {
                    signalRow.getPortAbstraction() = extendPort;
                    existingSignals.replace(existingSignals.indexOf(signalRow), signalRow);
                }
            }
        }
    }

    return existingSignals;
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::getPortAbstractionRowCount()
//-----------------------------------------------------------------------------
int AbstractionExtendHandler::getPortAbstractionRowCount(QList<QSharedPointer<PortAbstraction>> const& extendPorts)
{
    int extendRows = 0;

    for (auto logicalPort : extendPorts)
    {
        if (logicalPort->hasWire())
        {
            QSharedPointer<WireAbstraction> logicalWire = logicalPort->getWire();
            if (logicalWire->hasMasterPort())
            {
                extendRows++;
            }
            if (logicalWire->hasSlavePort())
            {
                extendRows++;
            }
            for (auto systemPort : *logicalWire->getSystemPorts())
            {
                extendRows++;
            }
        }
        if (logicalPort->hasTransactional())
        {
            QSharedPointer<TransactionalAbstraction> logicalTransactional = logicalPort->getTransactional();
            if (logicalTransactional->hasMasterPort())
            {
                extendRows++;
            }
            if (logicalTransactional->hasSlavePort())
            {
                extendRows++;
            }
            for (auto systemPort : *logicalTransactional->getSystemPorts())
            {
                extendRows++;
            }
        }
    }

    return extendRows;
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::addSignalRowToPortAbstraction()
//-----------------------------------------------------------------------------
void AbstractionExtendHandler::addSignalRowToPortAbstraction(AbstractionDefinitionSignalRow& signal,
    QSharedPointer<AbstractionDefinition> abstraction)
{
    QSharedPointer<PortAbstraction> signalPort = abstraction->getPort(signal.getPortAbstraction()->name());
    signal.setPortAbstraction(signalPort);
    if (signal.getWire())
    {
        if (signal.getMode() == General::MASTER)
        {
            signalPort->getWire()->setMasterPort(signal.getWire());
        }
        else if (signal.getMode() == General::SLAVE)
        {
            signalPort->getWire()->setSlavePort(signal.getWire());
        }
        else if (signal.getMode() == General::SYSTEM)
        {
            signalPort->getWire()->getSystemPorts()->append(signal.getWire());
        }
    }
    if (signal.getTransactional())
    {
        if (signal.getMode() == General::MASTER)
        {
            signalPort->getTransactional()->setMasterPort(signal.getTransactional());
        }
        else if (signal.getMode() == General::SLAVE)
        {
            signalPort->getTransactional()->setSlavePort(signal.getTransactional());
        }
        else if (signal.getMode() == General::SYSTEM)
        {
            signalPort->getTransactional()->getSystemPorts()->append(signal.getTransactional());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::removeExtendedPortsFromAbstraction()
//-----------------------------------------------------------------------------
void AbstractionExtendHandler::removeExtendedPortsFromAbstraction(
    QSharedPointer<AbstractionDefinition> abstraction, QList<QSharedPointer<PortAbstraction>> const& extendPorts)
{
    for (auto port : extendPorts)
    {
        QSharedPointer<PortAbstraction> logicalPort = abstraction->getPort(port->name());
        if (logicalPort)
        {
            abstraction->getLogicalPorts()->removeAll(logicalPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::getAllExtendPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortAbstraction> > AbstractionExtendHandler::getAllExtendPorts(
    QSharedPointer<AbstractionDefinition> abstraction, LibraryInterface* libraryAccess)
{
    QList<QSharedPointer<PortAbstraction> > extendPorts;
    if (!abstraction->getExtends().isEmpty() && abstraction->getExtends().isValid())
    {
        QSharedPointer<const AbstractionDefinition> extendAbstraction =
            getExtendAbstraction(abstraction, libraryAccess);
        if (extendAbstraction)
        {
            extendPorts =
                BusDefinitionUtils::getExtendedLogicalSignals(extendAbstraction, libraryAccess);
        }
    }

    return extendPorts;
}

//-----------------------------------------------------------------------------
// Function: AbstractionExtendHandler::getExtendAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<const AbstractionDefinition> AbstractionExtendHandler::getExtendAbstraction(
    QSharedPointer<AbstractionDefinition> abstraction, LibraryInterface* libraryAccess)
{
    QSharedPointer<const Document> extendDocument = libraryAccess->getModelReadOnly(abstraction->getExtends());
    if (extendDocument)
    {
        QSharedPointer<const AbstractionDefinition> extendAbstraction =
            extendDocument.dynamicCast<const AbstractionDefinition>();
        if (extendAbstraction)
        {
            return extendAbstraction;
        }
    }

    return QSharedPointer<const AbstractionDefinition>();
}
