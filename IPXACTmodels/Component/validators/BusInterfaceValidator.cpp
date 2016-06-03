//-----------------------------------------------------------------------------
// File: BusInterfaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.12.2015
//
// Description:
// Validator for the ipxact:busInterface.
//-----------------------------------------------------------------------------

#include "BusInterfaceValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/RemapState.h>

#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/MasterInterface.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::BusInterfaceValidator()
//-----------------------------------------------------------------------------
BusInterfaceValidator::BusInterfaceValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<QList<QSharedPointer<View> > > views,
    QSharedPointer<QList<QSharedPointer<Port> > > ports,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces,
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,
    QSharedPointer<PortMapValidator> portMapValidator,
    QSharedPointer<ParameterValidator2014> parameterValidator,
    LibraryInterface* libraryHandler):
expressionParser_(expressionParser),
availableChoices_(choices),
availableViews_(views),
availablePorts_(ports),
availableAddressSpaces_(addressSpaces),
availableMemoryMaps_(memoryMaps),
availableBusInterfaces_(busInterfaces),
availableFileSets_(fileSets),
availableRemapStates_(remapStates),
libraryHandler_(libraryHandler),
parameterValidator_(parameterValidator),
portMapValidator_(portMapValidator)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::~BusInterfaceValidator()
//-----------------------------------------------------------------------------
BusInterfaceValidator::~BusInterfaceValidator()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::componentChange()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::componentChange(QSharedPointer<QList<QSharedPointer<Choice> > > newChoices,
    QSharedPointer<QList<QSharedPointer<View> > > newViews, QSharedPointer<QList<QSharedPointer<Port> > > newPorts,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > newMemoryMaps,
    QSharedPointer<QList<QSharedPointer<BusInterface> > > newBusInterfaces,
    QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets,
    QSharedPointer<QList<QSharedPointer<RemapState> > > newRemapStates)
{
    availableChoices_ = newChoices;
    availableViews_ = newViews;
    availablePorts_ = newPorts;
    availableAddressSpaces_ = newAddressSpaces;
    availableMemoryMaps_ = newMemoryMaps;
    availableBusInterfaces_ = newBusInterfaces;
    availableFileSets_ = newFileSets;
    availableRemapStates_ = newRemapStates;

    portMapValidator_->componentChanged(availablePorts_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::getPortMapValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortMapValidator> BusInterfaceValidator::getPortMapValidator()
{
    return portMapValidator_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::validate()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::validate(QSharedPointer<BusInterface> busInterface) const
{
    return hasValidName(busInterface) && hasValidIsPresent(busInterface->getIsPresent()) &&
        hasValidBusType(busInterface) && hasValidAbstractionTypes(busInterface) &&
        hasValidInterfaceMode(busInterface) && hasValidBitsInLAU(busInterface) &&
        hasValidBitSteering(busInterface) && hasValidParameters(busInterface);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidName(QSharedPointer<BusInterface> busInterface) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(busInterface->name());

    if (busInterface->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidIsPresent(QString const& isPresent) const
{
    if (!isPresent.isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(isPresent);

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidBusType()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidBusType(QSharedPointer<BusInterface> busInterface) const
{
    return busInterface->getBusType().isValid() && libraryHandler_->contains(busInterface->getBusType()) &&
        libraryHandler_->getDocumentType(busInterface->getBusType()) == VLNV::BUSDEFINITION;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidAbstractionType()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidAbstractionTypes(QSharedPointer<BusInterface> busInterface) const
{
    foreach (QSharedPointer<AbstractionType> abstraction, *busInterface->getAbstractionTypes())
    {
        if (!abstractionTypeHasValidAbstractionReference(abstraction) ||
            !abstractionTypeHasValidViewReference(abstraction) ||
            !abstractionTypeHasValidPortMaps(busInterface, abstraction))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::abstractionTypeHasValidAbstractionReference()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::abstractionTypeHasValidAbstractionReference(
    QSharedPointer<AbstractionType> abstraction) const
{
    if (abstraction->getAbstractionRef())
    {
        return libraryHandler_->contains(*abstraction->getAbstractionRef()) && 
            libraryHandler_->getDocumentType(*abstraction->getAbstractionRef()) == VLNV::ABSTRACTIONDEFINITION;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::abstractionTypeHasValidViewReference()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::abstractionTypeHasValidViewReference(QSharedPointer<AbstractionType> abstraction) const
{
    if (abstraction->getViewRef().isEmpty())
    {
        return true;
    }

    foreach (QSharedPointer<View> view, *availableViews_)
    {
        if (abstraction->getViewRef() == view->name())
        {
            return true;
        }
    }

    return abstraction->getViewRef().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::abstractionTypeHasValidPortMaps()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::abstractionTypeHasValidPortMaps(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<AbstractionType> abstraction) const
{
    if (abstraction && abstraction->getAbstractionRef())
    {
        QSharedPointer<AbstractionDefinition const> abstractionDefinition =
            getAbstractionDefinition(abstraction->getAbstractionRef());

        if (!requiredPortAbstractionsAreMapped(busInterface, abstractionDefinition, abstraction->getPortMaps()))
        {
            return false;
        }
        else if (!abstraction->getPortMaps()->isEmpty())
        {
            portMapValidator_->busInterfaceChanged(abstraction->getAbstractionRef(), busInterface->getInterfaceMode());

            MemoryReserve reservedArea;
            foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
            {
                if (abstractionDefinition && portMap->getLogicalPort() &&
                    !logicalPortHasValidPresence(abstractionDefinition, portMap->getLogicalPort()->name_,
                    busInterface->getInterfaceMode()))
                {
                    return false;
                }

                if (!portMapValidator_->validate(portMap))
                {
                    return false;
                }

                qint64 logicalAreaBegin = 0;
                qint64 LogicalAreaEnd = 0;

                if (portMap->getLogicalPort()->range_)
                {
                    logicalAreaBegin =
                        expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toLongLong();
                    LogicalAreaEnd =
                        expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toLongLong();
                }

                if (LogicalAreaEnd < logicalAreaBegin)
                {
                    int temporary = logicalAreaBegin;
                    logicalAreaBegin = LogicalAreaEnd;
                    LogicalAreaEnd = temporary;
                }

                reservedArea.addArea(portMap->getLogicalPort()->name_, logicalAreaBegin, LogicalAreaEnd);
            }

            return !reservedArea.hasIdDependantOverlap();
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::portAbstractionsHaveValidPresences()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::requiredPortAbstractionsAreMapped(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<AbstractionDefinition const> abstractionDefinition,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    if (abstractionDefinition && abstractionDefinition->getLogicalPorts())
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *abstractionDefinition->getLogicalPorts())
        {
            if (logicalPort->hasMode(busInterface->getInterfaceMode()) &&
                logicalPort->getPresence(busInterface->getInterfaceMode()) == PresenceTypes::REQUIRED &&
                !logicalPortHasReferencingPortMaps(logicalPort->name(), portMaps))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::logicalPortHasReferencingPortMaps()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::logicalPortHasReferencingPortMaps(QString const& portName,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    foreach (QSharedPointer<PortMap> currentMap, *portMaps)
    {
        if (currentMap->getLogicalPort() && currentMap->getLogicalPort()->name_ == portName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::getAbstractionDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition const> BusInterfaceValidator::getAbstractionDefinition(
    QSharedPointer<ConfigurableVLNVReference> abstractionReference) const
{
    QSharedPointer<AbstractionDefinition const> abstractionDefinition;

    if (abstractionReference)
    {
        QSharedPointer<Document const> abstractionDocument =
            libraryHandler_->getModelReadOnly(*abstractionReference.data());

        abstractionDefinition = abstractionDocument.dynamicCast<AbstractionDefinition const>();
    }

    return abstractionDefinition;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::logicalPortHasValidPresence()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::logicalPortHasValidPresence(
    QSharedPointer<AbstractionDefinition const> abstractionDefinition, QString const& logicalPortName,
    General::InterfaceMode mode) const
{
    QSharedPointer<PortAbstraction> logicalPort =
        getLogicalPort(logicalPortName, abstractionDefinition);

    if (logicalPort && logicalPort->getPresence(mode) == PresenceTypes::ILLEGAL)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::getLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> BusInterfaceValidator::getLogicalPort(QString const& portName,
    QSharedPointer<AbstractionDefinition const> abstractionDefinition) const
{
    if (abstractionDefinition)
    {
        foreach (QSharedPointer<PortAbstraction> definitionPort, *abstractionDefinition->getLogicalPorts())
        {
            if (portName == definitionPort->getLogicalName())
            {
                return definitionPort;
            }
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidInterfaceMode()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidInterfaceMode(QSharedPointer<BusInterface> busInterface) const
{
    General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();

    if (interfaceMode == General::MASTER || interfaceMode == General::MIRROREDMASTER)
    {
        return hasValidMasterInterface(busInterface->getMaster());
    }
    else if (interfaceMode == General::SLAVE)
    {
        return hasValidSlaveInterface(busInterface, busInterface->getSlave());
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRROREDSYSTEM)
    {
        return hasValidSystemInterface(busInterface, busInterface->getSystem());
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        return hasValidMirroredSlaveInterface(busInterface->getMirroredSlave());
    }
    else if (interfaceMode == General::MONITOR)
    {
        return hasValidMonitorInterface(busInterface, busInterface->getMonitor());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidMasterInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidMasterInterface(QSharedPointer<MasterInterface> master) const
{
    if (master->getAddressSpaceRef().isEmpty() && master->getIsPresent().isEmpty() &&
        master->getBaseAddress().isEmpty())
    {
        return true;
    }

    else if (!master->getAddressSpaceRef().isEmpty())
    {
        foreach (QSharedPointer<AddressSpace> space, *availableAddressSpaces_)
        {
            if (master->getAddressSpaceRef() == space->name())
            {
                bool changeOk = true;
                quint64 baseAddress =
                    expressionParser_->parseExpression(master->getBaseAddress()).toULongLong(&changeOk);

                return (master->getBaseAddress().isEmpty() || (changeOk && baseAddress >= 0)) &&
                    hasValidIsPresent(master->getIsPresent()) &&
                    interfaceReferenceHasValidPresence(master->getIsPresent(), space->getIsPresent());
            }
        }

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::masterInterfaceHasValidPresenceForReference()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::interfaceReferenceHasValidPresence(QString const& interfacePresence,
    QString const& targetPresence) const
{
    if (!interfacePresence.isEmpty())
    {
        int masterPresence = 1;
        if (!interfacePresence.isEmpty())
        {
            masterPresence = expressionParser_->parseExpression(interfacePresence).toInt();
        }

        if (masterPresence == 1)
        {
            int spacePresence = 1;
            if (!targetPresence.isEmpty())
            {
                spacePresence = expressionParser_->parseExpression(targetPresence).toInt();
            }

            return spacePresence == 1;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidSlaveInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidSlaveInterface(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<SlaveInterface> slave) const
{
    bool validMemoryMapRef = slaveInterfaceHasValidMemoryMapRef(busInterface, slave);
    bool validBridges = slaveInterfaceHasValidBridges(slave);
    bool validFileSetRefs = slaveInterfaceFileSetRefGroupsAreValid(slave);

    if (validMemoryMapRef && validBridges)
    {
        return false;
    }

    if ((slave->getMemoryMapRef().isEmpty() && slave->getBridges()->isEmpty() && slave->getFileSetRefGroup()->isEmpty())
        || (validFileSetRefs && 
        ((!slave->getMemoryMapRef().isEmpty() && validMemoryMapRef) ||
        (!slave->getBridges()->isEmpty() && validBridges) ||
        !slave->getFileSetRefGroup()->isEmpty())))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidSlaveInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveInterfaceHasValidMemoryMapRef(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<SlaveInterface> slave) const
{
    if (!slave->getMemoryMapRef().isEmpty())
    {
        foreach (QSharedPointer<MemoryMap> memoryMap, *availableMemoryMaps_)
        {
            if (slave->getMemoryMapRef() == memoryMap->name())
            {
                return interfaceReferenceHasValidPresence(busInterface->getIsPresent(), memoryMap->getIsPresent());
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveInterfaceHasValidBridges()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveInterfaceHasValidBridges(QSharedPointer<SlaveInterface> slave) const
{
    foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *slave->getBridges())
    {
        if (slaveBridgeReferencesValidMaster(bridge))
        {
            return hasValidIsPresent(bridge->isPresent_);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveBridgeReferencesValidMaster()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveBridgeReferencesValidMaster(QSharedPointer<SlaveInterface::Bridge> bridge) const
{
    foreach (QSharedPointer<BusInterface> busInterface, *availableBusInterfaces_)
    {
        if (busInterface->getInterfaceMode() == General::MASTER && bridge->masterRef_ == busInterface->name())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveInterfaceFileSetRefGroupsAreValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveInterfaceFileSetRefGroupsAreValid(QSharedPointer<SlaveInterface> slave) const
{
    foreach (QSharedPointer<SlaveInterface::FileSetRefGroup> group, *slave->getFileSetRefGroup())
    {
        foreach (QString fileSetReference, group->fileSetRefs_)
        {
            if (!slaveFileSetReferenceIsValid(fileSetReference))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveFileSetReferenceIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveFileSetReferenceIsValid(QString const& fileSetReference) const
{
    foreach (QSharedPointer<FileSet> fileset, *availableFileSets_)
    {
        if (fileSetReference == fileset->name())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidSystemInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidSystemInterface(QSharedPointer<BusInterface> busInterface,
    QString const& systemGroup) const
{
    if (!systemGroup.isEmpty())
    {
        QSharedPointer<Document const> definitionDocument = libraryHandler_->getModelReadOnly(busInterface->getBusType());
		QSharedPointer<BusDefinition const> busDefinition = definitionDocument.dynamicCast<BusDefinition const>();

        if (busDefinition)
        {
            return busDefinition->getSystemGroupNames().contains(systemGroup);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidMirroredSlaveInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidMirroredSlaveInterface(QSharedPointer<MirroredSlaveInterface> mirroredSlave)
    const
{
    if (mirroredSlave->getRemapAddresses()->isEmpty() && mirroredSlave->getRange().isEmpty())
    {
        return true;
    }

    if (mirroredSlaveRangeIsValid(mirroredSlave))
    {
        if (!mirroredSlave->getRemapAddresses()->isEmpty())
        {
            foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress,
                *mirroredSlave->getRemapAddresses())
            {
                if (!mirroredSlaveRemapAddressIsValid(remapAddress) || !mirroredSlaveStateIsValid(remapAddress))
                {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::mirroredSlaveRangeIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::mirroredSlaveRangeIsValid(QSharedPointer<MirroredSlaveInterface> mirroredSlave) const
{
    bool changeOk = true;
    quint64 range = expressionParser_->parseExpression(mirroredSlave->getRange()).toULongLong(&changeOk);

    return changeOk && range > 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::mirroredSlaveRemapAddressIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::mirroredSlaveRemapAddressIsValid(
    QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const
{
    if (!remapAddress->remapAddress_.isEmpty())
    {
        bool changeOk = true;
        quint64 addressInt =
            expressionParser_->parseExpression(remapAddress->remapAddress_).toULongLong(&changeOk);

        return changeOk && addressInt >= 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::mirroredSlaveStateIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::mirroredSlaveStateIsValid(
    QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress) const
{
    if (!remapAddress->state_.isEmpty())
    {
        foreach (QSharedPointer<RemapState> remapState, *availableRemapStates_)
        {
            if (remapState->name() == remapAddress->state_)
            {
                return true;
            }
        }

        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidMonitorInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidMonitorInterface(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<BusInterface::MonitorInterface> monitorInterface) const
{
    General::InterfaceMode interfaceMode = monitorInterface->interfaceMode_;

    if (interfaceMode != General::INTERFACE_MODE_COUNT && interfaceMode != General::MONITOR)
    {
        if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRROREDSYSTEM)
        {
            return hasValidSystemInterface(busInterface, monitorInterface->group_);
        }

        return monitorInterface->group_.isEmpty();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidBitsInLAU()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidBitsInLAU(QSharedPointer<BusInterface> busInterface) const
{
    if (!busInterface->getBitsInLau().isEmpty())
    {
        bool changeOk = true;
        quint64 bitsInLAU =
            expressionParser_->parseExpression(busInterface->getBitsInLau()).toULongLong(&changeOk);

        if (!changeOk || bitsInLAU <= 0)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidBitSteering()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidBitSteering(QSharedPointer<BusInterface> busInterface) const
{
    if (busInterface->getBitSteering() != BusInterface::BITSTEERING_UNSPECIFIED)
    {
        General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();
        
        if (interfaceMode == General::MIRROREDMASTER || interfaceMode == General::SYSTEM ||
            interfaceMode == General::MIRROREDSYSTEM)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidParameters(QSharedPointer<BusInterface> busInterface) const
{
    if (!busInterface->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *busInterface->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
    QString const& context) const
{
    QString busInterfaceContext = "bus interface " + busInterface->name();

    findErrorsInName(errors, busInterface, context);
    findErrorsInIsPresent(errors, busInterface, context);
    findErrorsInBusType(errors, busInterface, context);
    findErrorsInAbstractionTypes(errors, busInterface, busInterfaceContext);
    findErrorsInInterfaceMode(errors, busInterface, busInterfaceContext, context);
    findErrorsInBitsInLAU(errors, busInterface, context);
    findErrorsInBitSteering(errors, busInterface, context);
    findErrorsInParameters(errors, busInterface, busInterfaceContext);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<BusInterface> busInterface,
    QString const& context) const
{
    if (!hasValidName(busInterface))
    {
        errors.append(QObject::tr("Invalid name specified for bus interface %1 within %2")
            .arg(busInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (!hasValidIsPresent(busInterface->getIsPresent()))
    {
        errors.append(QObject::tr("Invalid isPresent set for bus interface %1 within %2")
            .arg(busInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInBusType()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInBusType(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (!busInterface->getBusType().isValid())
    {
        errors.append(QObject::tr("Bus definition must be given for bus interface %1 within %2")
            .arg(busInterface->name(), context));
    }
    else
    {
        if (!libraryHandler_->contains(busInterface->getBusType()))
        {
            errors.append(QObject::tr("Bus definition %1 set for bus interface %2 within %3 could not be found in "
                "the library").arg(busInterface->getBusType().toString(), busInterface->name(), context));
        }
        if (libraryHandler_->getDocumentType(busInterface->getBusType()) != VLNV::BUSDEFINITION)
        {
            errors.append(QObject::tr("Invalid bus definition set for %1 within %2")
                .arg(busInterface->name(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInAbstractionTypes()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInAbstractionTypes(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    foreach (QSharedPointer<AbstractionType> abstraction, *busInterface->getAbstractionTypes())
    {
        if (!abstractionTypeHasValidViewReference(abstraction))
        {
            errors.append(QObject::tr("Invalid view reference %1 set for abstraction type in %2")
                .arg(abstraction->getViewRef()).arg(context));
        }

        QString portMapContext = QObject::tr("%1 %2")
            .arg(General::interfaceMode2Str(busInterface->getInterfaceMode())).arg(context);

        findErrorsInAbstractionReference(errors, abstraction, context);
        findErrorsInAbstractionPortMaps(errors, busInterface, abstraction, portMapContext);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInAbstractionReference()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInAbstractionReference(QVector<QString>& errors,
    QSharedPointer<AbstractionType> abstraction, QString const& context) const
{
    if (!abstraction->getAbstractionRef())
    {
        errors.append(QObject::tr("Abstraction reference must be set for each abstraction type in %1")
            .arg(context));
    }
    else
    {
        if (!libraryHandler_->contains(*abstraction->getAbstractionRef().data()))
        {
            errors.append(QObject::tr("Abstraction reference %1 set for %2 could not be found in the library")
                .arg(abstraction->getAbstractionRef()->toString()).arg(context));
        }
        if (libraryHandler_->getDocumentType(*abstraction->getAbstractionRef()) !=
            VLNV::ABSTRACTIONDEFINITION)
        {
            errors.append(QObject::tr("Invalid abstraction reference set within %1").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInAbstractionPortMaps()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInAbstractionPortMaps(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<AbstractionType> abstraction, QString const& context)
    const
{
    if (abstraction && abstraction->getAbstractionRef())
    {
        QSharedPointer<AbstractionDefinition const> abstractionDefinition =
            getAbstractionDefinition(abstraction->getAbstractionRef());

        if (!requiredPortAbstractionsAreMapped(busInterface, abstractionDefinition, abstraction->getPortMaps()))
        {
            findErrorsInRequiredPortAbstractions(
                errors, context, busInterface, abstractionDefinition, abstraction->getPortMaps());
        }

        if (!abstraction->getPortMaps()->isEmpty())
        {
            portMapValidator_->busInterfaceChanged(abstraction->getAbstractionRef(), busInterface->getInterfaceMode());

            QStringList portNames;

            MemoryReserve logicalReservedArea;

            foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
            {
                if (abstractionDefinition && portMap->getLogicalPort() &&
                    !logicalPortHasValidPresence(abstractionDefinition, portMap->getLogicalPort()->name_,
                    busInterface->getInterfaceMode()) && !portNames.contains(portMap->getLogicalPort()->name_))
                {
                    errors.append(QObject::tr("Logical port %1 with presence 'ILLEGAL' mapped within %2.")
                        .arg(portMap->getLogicalPort()->name_)
                        .arg(context));

                    portNames.append(portMap->getLogicalPort()->name_);
                }

                portMapValidator_->findErrorsIn(errors, portMap, context);

                if (portMap->getLogicalPort())
                {
                    qint64 logicalAreaBegin = 0;
                    qint64 LogicalAreaEnd = 0;

                    if (portMap->getLogicalPort()->range_)
                    {
                        logicalAreaBegin =
                            expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toLongLong();
                        LogicalAreaEnd =
                            expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toLongLong();
                    }
                    if (LogicalAreaEnd < logicalAreaBegin)
                    {
                        int temporary = logicalAreaBegin;
                        logicalAreaBegin = LogicalAreaEnd;
                        LogicalAreaEnd = temporary;
                    }

                    logicalReservedArea.addArea(portMap->getLogicalPort()->name_, logicalAreaBegin, LogicalAreaEnd);
                }
            }
            logicalReservedArea.findErrorsInIdDependantOverlap(errors, QLatin1String("logical port"), context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInRequiredPortAbstractions()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInRequiredPortAbstractions(QVector<QString>& errors, QString const& context,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<AbstractionDefinition const> abstractionDefinition,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    if (!requiredPortAbstractionsAreMapped(busInterface, abstractionDefinition, portMaps))
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *abstractionDefinition->getLogicalPorts())
        {
            if (logicalPort->hasMode(busInterface->getInterfaceMode()) &&
                logicalPort->getPresence(busInterface->getInterfaceMode()) == PresenceTypes::REQUIRED &&
                !logicalPortHasReferencingPortMaps(logicalPort->name(), portMaps))
            {
                errors.append(QObject::tr("Logical port %1 with presence 'REQUIRED' is not mapped within %2.")
                    .arg(logicalPort->name())
                    .arg(context));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (!busInterface->getParameters()->isEmpty())
    {
        QVector<QString> parameterNames(busInterface->getParameters()->count());
        foreach (QSharedPointer<Parameter> parameter, *busInterface->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Parameter name %1 is not unique within %2.").arg(parameter->name(), 
                    context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInInterfaceMode(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& busInterfaceContext,
    QString const& containingContext) const
{
    General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();

    QString newContext = General::interfaceMode2Str(busInterface->getInterfaceMode()) + " " + busInterfaceContext;

    if (interfaceMode == General::MASTER || interfaceMode == General::MIRROREDMASTER)
    {
        findErrorsInMasterInterface(errors, busInterface->getMaster(), newContext);
    }
    else if (interfaceMode == General::SLAVE)
    {
        findErrorsInSlaveInterface(errors, busInterface, busInterface->getSlave(), newContext);
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRROREDSYSTEM)
    {
        findErrorsInSystemInterface(errors, busInterface->getSystem(), busInterface, newContext);
    }
    else if (interfaceMode == General::MIRROREDSLAVE)
    {
        findErrorsInMirroredSlaveInterface(errors, busInterface->getMirroredSlave(), newContext);
    }
    else if (interfaceMode == General::MONITOR)
    {
        findErrorsInMonitorInterface(errors, busInterface, busInterface->getMonitor(), newContext);
    }
    else
    {
        errors.append(QObject::tr("Unknown interface mode set for bus interface %1 within %2")
            .arg(busInterface->name(), containingContext));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInMasterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInMasterInterface(QVector<QString>& errors,
    QSharedPointer<MasterInterface> master, QString const& context) const
{
    if (!master->getAddressSpaceRef().isEmpty())
    {
        bool found = false;
        foreach (QSharedPointer<AddressSpace> space, *availableAddressSpaces_)
        {
            if (master->getAddressSpaceRef() == space->name())
            {
                found = true;

                if (!interfaceReferenceHasValidPresence(master->getIsPresent(), space->getIsPresent()))
                {
                    errors.append(QObject::tr("Cannot refer to non-present address space %1 in %2")
                        .arg(space->name()).arg(context));
                }
                break;
            }
        }
        if (!found)
        {
            errors.append(QObject::tr("Could not find address space %1 referenced by the %2")
                .arg(master->getAddressSpaceRef()).arg(context));
        }
        if (!master->getBaseAddress().isEmpty())
        {
            bool changeOk = true;
            int baseAddress = expressionParser_->parseExpression(master->getBaseAddress()).toInt(&changeOk);

            if (!changeOk || baseAddress < 0)
            {
                errors.append(QObject::tr("Invalid base address set for %1").arg(context));
            }
        }
        if (!hasValidIsPresent(master->getIsPresent()))
        {
            errors.append(QObject::tr("Invalid is present set for address space reference in %1").arg(context));
        }
    }
    else if (!master->getIsPresent().isEmpty() || !master->getBaseAddress().isEmpty())
    {
        errors.append(QObject::tr("Invalid address space reference set for %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInSlaveInterface(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<SlaveInterface> slave, QString const& context) const
{
    if (!slave->getMemoryMapRef().isEmpty() && !slave->getBridges()->isEmpty())
    {
        errors.append(QObject::tr("Both a memory map reference and transparent bridges are contained within %1")
            .arg(context));
    }
    if (!slave->getMemoryMapRef().isEmpty() &&!slaveInterfaceHasValidMemoryMapRef(busInterface, slave))
    {
        errors.append(QObject::tr("Memory map %1 referenced by the %2 was not found")
            .arg(slave->getMemoryMapRef()).arg(context));
    }

    foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *slave->getBridges())
    {
        if (!slaveBridgeReferencesValidMaster(bridge))
        {
            errors.append(QObject::tr("Master bus interface %1 referenced by the %2 was not found")
                .arg(bridge->masterRef_).arg(context));
        }
        if (!hasValidIsPresent(bridge->isPresent_))
        {
            errors.append(QObject::tr("Transparent bridge within the %1 has invalid isPresent")
                .arg(context));
        }
    }

    foreach (QSharedPointer<SlaveInterface::FileSetRefGroup> group, *slave->getFileSetRefGroup())
    {
        foreach (QString fileSetReference, group->fileSetRefs_)
        {
            if (!slaveFileSetReferenceIsValid(fileSetReference))
            {
                errors.append(QObject::tr("Invalid file set %1 referenced within group %2 of %3").arg(
                    fileSetReference, group->group_, context));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInSystemInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInSystemInterface(QVector<QString>& errors, QString const& systemGroup,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (systemGroup.isEmpty())
    {
        errors.append(QObject::tr("No system group given for %1").arg(context));
    }

    else
    {
        if (busInterface->getBusType().isValid())
        {
			QSharedPointer<Document const> definitionDocument = libraryHandler_->getModelReadOnly(busInterface->getBusType());
			QSharedPointer<BusDefinition const> busDefinition = definitionDocument.dynamicCast<BusDefinition const>();

            if (busDefinition && !busDefinition->getSystemGroupNames().contains(systemGroup))
            {
                errors.append(QObject::tr("Could not find system group %1 set for %2 with bus definition %3.")
                    .arg(systemGroup, context, busInterface->getBusType().toString()));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInMirroredSlaveInterface(QVector<QString>& errors,
    QSharedPointer<MirroredSlaveInterface> mirroredSlave, QString const& context) const
{
    if (!mirroredSlave->getRange().isEmpty() || !mirroredSlave->getRemapAddresses()->isEmpty())
    {
        if (mirroredSlave->getRemapAddresses()->isEmpty())
        {
            errors.append(QObject::tr("Invalid remap address set for %1").arg(context));
        }

        foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remapAddress,
            *mirroredSlave->getRemapAddresses())
        {
            if (!mirroredSlaveRemapAddressIsValid(remapAddress))
            {
                errors.append(QObject::tr("Invalid remap address set for %1").arg(context));
            }
            if (!mirroredSlaveStateIsValid(remapAddress))
            {
                errors.append(QObject::tr("Invalid remap state set for %1").arg(context));
            }
        }

        if (!mirroredSlaveRangeIsValid(mirroredSlave))
        {
            errors.append(QObject::tr("Invalid range set for %1").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInMonitorInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInMonitorInterface(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<BusInterface::MonitorInterface> monitor,
    QString const& context) const
{
    General::InterfaceMode interfaceMode = monitor->interfaceMode_;

    if (interfaceMode == General::INTERFACE_MODE_COUNT || interfaceMode == General::MONITOR)
    {
        errors.append(QObject::tr("Invalid interface mode set for %1").arg(context));
    }

    else
    {
        if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRROREDSYSTEM)
        {
            findErrorsInSystemInterface(errors, monitor->group_, busInterface, context);
        }

        else if (!monitor->group_.isEmpty())
        {
            errors.append(QObject::tr("Group cannot be set for %1 with interface mode master, slave, "
                "mirrored master or mirrored slave").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInBitsInLAU()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInBitsInLAU(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (!hasValidBitsInLAU(busInterface))
    {
        errors.append(QObject::tr("Invalid bits in LAU set for bus interface %1 within %2")
            .arg(busInterface->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInBitSteering()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInBitSteering(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QString const& context) const
{
    if (!hasValidBitSteering(busInterface))
    {
        QString usedInterface = General::interfaceMode2Str(busInterface->getInterfaceMode());

        errors.append(QObject::tr("Bit steering value is not allowed in %1 bus interface %2 within %3")
            .arg(usedInterface).arg(busInterface->name()).arg(context));
    }
}
