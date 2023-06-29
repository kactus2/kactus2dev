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

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

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
#include <IPXACTmodels/Component/InitiatorInterface.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/generaldeclarations.h>


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
    QSharedPointer<ParameterValidator> parameterValidator,
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
abstractionValidator_(new AbstractionTypeValidator(expressionParser, views, portMapValidator, libraryHandler))
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

    abstractionValidator_->changeComponent(newViews, newPorts);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::getAbstractionValidator()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionTypeValidator> BusInterfaceValidator::getAbstractionValidator() const
{
    return abstractionValidator_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::validate()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::validate(QSharedPointer<BusInterface> busInterface,
    Document::Revision docRevision) const
{
    return hasValidName(busInterface) && hasValidIsPresent(busInterface->getIsPresent()) &&
        hasValidBusType(busInterface) && hasValidAbstractionTypes(busInterface) &&
        hasValidInterfaceMode(busInterface) && hasValidBitsInLAU(busInterface) &&
        hasValidBitSteering(busInterface, docRevision) && hasValidParameters(busInterface);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidName(QSharedPointer<BusInterface> busInterface) const
{
    return CommonItemsValidator::hasValidName(busInterface->name());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidIsPresent(QString const& isPresent) const
{
    return CommonItemsValidator::hasValidIsPresent(isPresent, expressionParser_);
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
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > busAbstractions = busInterface->getAbstractionTypes();
    for (QSharedPointer<AbstractionType> const& abstraction : *busAbstractions)
    {
        if (!abstractionValidator_->validate(abstraction, busAbstractions, busInterface))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::hasValidInterfaceMode()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::hasValidInterfaceMode(QSharedPointer<BusInterface> busInterface) const
{
    General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();

    if (interfaceMode == General::MASTER)
    {
        return hasValidMasterInterface(busInterface->getMaster());
    }
    else if (interfaceMode == General::MIRRORED_MASTER)
    {
        return true;
    }
    else if (interfaceMode == General::SLAVE)
    {
        return hasValidSlaveInterface(busInterface, busInterface->getSlave());
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
    {
        return hasValidSystemInterface(busInterface, busInterface->getSystem());
    }
    else if (interfaceMode == General::MIRRORED_SLAVE)
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
bool BusInterfaceValidator::hasValidMasterInterface(QSharedPointer<InitiatorInterface> master) const
{
    if (master->getAddressSpaceRef().isEmpty() && master->getIsPresent().isEmpty() &&
        master->getBaseAddress().isEmpty())
    {
        return true;
    }

    else if (!master->getAddressSpaceRef().isEmpty())
    {
        for (QSharedPointer<AddressSpace> space : *availableAddressSpaces_)
        {
            if (master->getAddressSpaceRef() == space->name())
            {
                bool changeOk = true;
                bool expressionValid = false;
                expressionParser_->parseExpression(master->getBaseAddress(), &expressionValid).toULongLong(&changeOk);

                return (master->getBaseAddress().isEmpty() || (changeOk && expressionValid)) &&
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
    QSharedPointer<TargetInterface> slave) const
{
    bool validMemoryMapRef = slaveInterfaceHasValidMemoryMapRef(busInterface, slave);
    bool validBridges = slaveInterfaceHasValidBridges(slave->getBridges());
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
    QSharedPointer<TargetInterface> slave) const
{
    if (!slave->getMemoryMapRef().isEmpty())
    {
        for (QSharedPointer<MemoryMap> memoryMap : *availableMemoryMaps_)
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
bool BusInterfaceValidator::slaveInterfaceHasValidBridges(
    QSharedPointer<QList<QSharedPointer<TransparentBridge>>> bridges) const
{
    if (!bridges)
    {
        return true;
    }

    for (QSharedPointer<TransparentBridge> bridge : *bridges)
    {
        if (slaveBridgeReferencesValidMaster(bridge))
        {
            return hasValidIsPresent(bridge->getIsPresent());
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveBridgeReferencesValidMaster()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveBridgeReferencesValidMaster(QSharedPointer<TransparentBridge> bridge) const
{
    for (QSharedPointer<BusInterface> busInterface : *availableBusInterfaces_)
    {
        if (busInterface->getInterfaceMode() == General::MASTER && bridge->getMasterRef() == busInterface->name())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::slaveInterfaceFileSetRefGroupsAreValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::slaveInterfaceFileSetRefGroupsAreValid(QSharedPointer<TargetInterface> slave) const
{
    for (QSharedPointer<TargetInterface::FileSetRefGroup> group : *slave->getFileSetRefGroup())
    {
        for (QString const& fileSetReference : group->fileSetRefs_)
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
    for (QSharedPointer<FileSet> fileset : *availableFileSets_)
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
bool BusInterfaceValidator::hasValidMirroredSlaveInterface(QSharedPointer<MirroredTargetInterface> mirroredSlave)
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
            for (QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress :
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
bool BusInterfaceValidator::mirroredSlaveRangeIsValid(QSharedPointer<MirroredTargetInterface> mirroredSlave) const
{
    bool changeOk = true;
    quint64 range = expressionParser_->parseExpression(mirroredSlave->getRange()).toULongLong(&changeOk);

    return changeOk && range > 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::mirroredSlaveRemapAddressIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::mirroredSlaveRemapAddressIsValid(
    QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress) const
{
    if (!remapAddress->remapAddress_.isEmpty())
    {
        bool changeOk = true;
        expressionParser_->parseExpression(remapAddress->remapAddress_).toULongLong(&changeOk);

        return changeOk;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::mirroredSlaveStateIsValid()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::mirroredSlaveStateIsValid(
    QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress) const
{
    if (remapAddress->state_.isEmpty())
    {
        return true;
    }

    for (QSharedPointer<RemapState> remapState : *availableRemapStates_)
    {
        if (remapState->name() == remapAddress->state_)
        {
            return true;
        }
    }

    return false;
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
        if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
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
bool BusInterfaceValidator::hasValidBitSteering(QSharedPointer<BusInterface> busInterface,
    Document::Revision docRevision) const
{
    auto const& bitStreering = busInterface->getBitSteering();
    if (bitStreering.isEmpty())
    {
        return true;
    }

    if (bitSteeringIsAllowed(busInterface) == false)
    {
        return false;
    }

    if (docRevision == Document::Revision::Std14)
    {
        return bitStreering == QLatin1String("on") || bitStreering == QLatin1String("off");
    }
    else if (docRevision == Document::Revision::Std22)
    {
        bool validExpression = false;
        auto bitSteeringValue = expressionParser_->parseExpression(bitStreering, &validExpression);

        return validExpression && 
            (bitSteeringValue == QLatin1String("1") || bitSteeringValue == QLatin1String("0"));
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::bitSteeringIsAllowed()
//-----------------------------------------------------------------------------
bool BusInterfaceValidator::bitSteeringIsAllowed(QSharedPointer<BusInterface> busInterface) const
{
    General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();
    if (interfaceMode == General::MIRRORED_MASTER || interfaceMode == General::MIRRORED_INITIATOR ||
        interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
    {
        return false;
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
        for (QSharedPointer<Parameter> parameter : *busInterface->getParameters())
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
    QString const& context, Document::Revision docRevision) const
{
    QString busInterfaceContext = QStringLiteral("bus interface ") + busInterface->name();

    findErrorsInName(errors, busInterface, context);
    findErrorsInIsPresent(errors, busInterface, context);
    findErrorsInBusType(errors, busInterface, context);
    findErrorsInAbstractionTypes(errors, busInterface, busInterfaceContext);
    findErrorsInInterfaceMode(errors, busInterface, busInterfaceContext, context);
    findErrorsInBitsInLAU(errors, busInterface, context);
    findErrorsInBitSteering(errors, busInterface, context, docRevision);
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
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > busAbstractions = busInterface->getAbstractionTypes();
    for (QSharedPointer<AbstractionType> abstraction : *busAbstractions)
    {
        abstractionValidator_->findErrorsIn(errors, abstraction, busAbstractions, busInterface, context);
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
        for (QSharedPointer<Parameter> parameter : *busInterface->getParameters())
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

    QString newContext = General::interfaceMode2Str(busInterface->getInterfaceMode());
    newContext.append(QLatin1Char(' '));
    newContext.append(busInterfaceContext);

    if (interfaceMode == General::MASTER)
    {
        findErrorsInMasterInterface(errors, busInterface->getMaster(), newContext);
    }
    else if (interfaceMode == General::SLAVE)
    {
        findErrorsInSlaveInterface(errors, busInterface, busInterface->getSlave(), newContext);
    }
    else if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
    {
        findErrorsInSystemInterface(errors, busInterface->getSystem(), busInterface, newContext);
    }
    else if (interfaceMode == General::MIRRORED_SLAVE)
    {
        findErrorsInMirroredSlaveInterface(errors, busInterface->getMirroredSlave(), newContext);
    }
    else if (interfaceMode == General::MONITOR)
    {
        findErrorsInMonitorInterface(errors, busInterface, busInterface->getMonitor(), newContext);
    }
    else if (interfaceMode != General::MIRRORED_MASTER)
    {
        errors.append(QObject::tr("Unknown interface mode set for bus interface %1 within %2")
            .arg(busInterface->name(), containingContext));
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInMasterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInMasterInterface(QVector<QString>& errors,
    QSharedPointer<InitiatorInterface> master, QString const& context) const
{
    if (master)
    {
        if (!master->getAddressSpaceRef().isEmpty())
        {
            bool found = false;
            for (QSharedPointer<AddressSpace> space : *availableAddressSpaces_)
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
                bool expressionValid = false;

                int baseAddress = expressionParser_->parseExpression(master->getBaseAddress(), &expressionValid).toInt(&changeOk);

                if (!changeOk || !expressionValid || baseAddress < 0)
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
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceValidator::findErrorsInSlaveInterface()
//-----------------------------------------------------------------------------
void BusInterfaceValidator::findErrorsInSlaveInterface(QVector<QString>& errors,
    QSharedPointer<BusInterface> busInterface, QSharedPointer<TargetInterface> slave, QString const& context) const
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

    for (QSharedPointer<TransparentBridge> bridge : *slave->getBridges())
    {
        if (!slaveBridgeReferencesValidMaster(bridge))
        {
            errors.append(QObject::tr("Master bus interface %1 referenced by the %2 was not found")
                .arg(bridge->getMasterRef()).arg(context));
        }
        if (!hasValidIsPresent(bridge->getIsPresent()))
        {
            errors.append(QObject::tr("Transparent bridge within the %1 has invalid isPresent")
                .arg(context));
        }
    }

    for (QSharedPointer<TargetInterface::FileSetRefGroup> group : *slave->getFileSetRefGroup())
    {
        for (QString fileSetReference : group->fileSetRefs_)
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
    QSharedPointer<MirroredTargetInterface> mirroredSlave, QString const& context) const
{
    if (!mirroredSlave->getRange().isEmpty() || !mirroredSlave->getRemapAddresses()->isEmpty())
    {
        if (mirroredSlave->getRemapAddresses()->isEmpty())
        {
            errors.append(QObject::tr("Invalid remap address set for %1").arg(context));
        }

        for (QSharedPointer<MirroredTargetInterface::RemapAddress> remapAddress :
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
        if (interfaceMode == General::SYSTEM || interfaceMode == General::MIRRORED_SYSTEM)
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
    QSharedPointer<BusInterface> busInterface, QString const& context,
    Document::Revision docRevision) const
{
    if (!hasValidBitSteering(busInterface, docRevision))
    {
        QString usedInterface = General::interfaceMode2Str(busInterface->getInterfaceMode());

        if (bitSteeringIsAllowed(busInterface) == false)
        {
            errors.append(QObject::tr("Bit steering value is not allowed in %1 bus interface %2 within %3").arg(
                usedInterface, busInterface->name(), context));
        }
        else
        {
            errors.append(QObject::tr("Bit steering value '%1' is not valid in bus interface %2 within %3").arg(
                busInterface->getBitSteering(), busInterface->name(), context));
        }
    }
}
