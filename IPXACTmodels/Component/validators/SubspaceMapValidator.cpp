//-----------------------------------------------------------------------------
// File: SubspaceMapValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2022
//
// Description:
// Validator for ipxact:subspaceMap.
//-----------------------------------------------------------------------------

#include "SubspaceMapValidator.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Segment.h>

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::SubspaceMapValidator()
//-----------------------------------------------------------------------------
SubspaceMapValidator::SubspaceMapValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator):
MemoryBlockValidator(expressionParser, parameterValidator),
availableBuses_(),
availableSpaces_()
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::getBlockType()
//-----------------------------------------------------------------------------
QString SubspaceMapValidator::getBlockType() const
{
    return QStringLiteral("subspace map");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::componentChange()
//-----------------------------------------------------------------------------
void SubspaceMapValidator::componentChange(QSharedPointer<Component> component)
{
    availableBuses_.clear();
    availableSpaces_.clear();

    if (component)
    {
        availableBuses_ = component->getBusInterfaces();
        availableSpaces_ = component->getAddressSpaces();
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::validate()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::validate(QSharedPointer<SubSpaceMap> subspace) const
{
    return MemoryBlockValidator::validate(subspace) &&
        hasValidMasterReference(subspace) &&
        hasValidSegmentReference(subspace);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::hasValidMasterReference()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::hasValidMasterReference(QSharedPointer<SubSpaceMap> subspace) const
{
    QString masterReference = subspace->getMasterReference();
    if (masterReference.isEmpty() || !masterReferenceExists(masterReference) ||
        !referencedBusIsMaster(masterReference))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::masterReferenceExists()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::masterReferenceExists(QString const& masterReference) const
{
    QSharedPointer<BusInterface> bus = getReferencedBusInterface(masterReference);
    if (bus)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::getReferencedBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> SubspaceMapValidator::getReferencedBusInterface(QString const& masterReference) const
{
    for (auto busInterface : *availableBuses_)
    {
        if (busInterface->name() == masterReference)
        {
            return busInterface;
        }
    }

    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::referencedBusIsMaster()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::referencedBusIsMaster(QString const& masterReference) const
{
    QSharedPointer<BusInterface> bus = getReferencedBusInterface(masterReference);
    if (bus && bus->getInterfaceMode() == General::MASTER)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::hasValidSegmentReference()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::hasValidSegmentReference(QSharedPointer<SubSpaceMap> subspace) const
{
    QString segmentReference = subspace->getSegmentReference();
    if (segmentReference.isEmpty() ||
        spaceContainsSegmentReference(subspace->getMasterReference(), segmentReference))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::spaceContainsSegmentReference()
//-----------------------------------------------------------------------------
bool SubspaceMapValidator::spaceContainsSegmentReference(QString const& masterReference,
    QString const& segmentReference) const
{
    QSharedPointer<AddressSpace> space = getReferencedSpace(masterReference);
    if (space)
    {
        for (auto segment : *space->getSegments())
        {
            if (segment->name() == segmentReference)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::getReferencedSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> SubspaceMapValidator::getReferencedSpace(QString const& masterReference) const
{
    QSharedPointer<BusInterface> bus = getReferencedBusInterface(masterReference);
    if (bus)
    {
        for (auto space : *availableSpaces_)
        {
            if (space->name() == bus->getAddressSpaceRef())
            {
                return space;
            }
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void SubspaceMapValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<SubSpaceMap> subspace,
    QString const& context) const
{
    QString subspaceContext = getBlockType() + QStringLiteral(" ") + subspace->name();

    findErrorsInName(errors, subspace, context);
    findErrorsInIsPresent(errors, subspace, context);
    findErrorsInBaseAddress(errors, subspace, context);
    findErrorsInMasterReference(errors, subspace, context);
    findErrorsInSegmentReference(errors, subspace, context);

    findErrorsInParameters(errors, subspace, subspaceContext);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::findErrorsInMasterReference()
//-----------------------------------------------------------------------------
void SubspaceMapValidator::findErrorsInMasterReference(QVector<QString>& errors,
    QSharedPointer<SubSpaceMap> subspace, QString const& context) const
{
    QString masterReference = subspace->getMasterReference();
    if (masterReference.isEmpty())
    {
        errors.append(
            QObject::tr("A master bus interface reference is not specified in subspace map %1 within %2").
            arg(subspace->name(), context));
    }
    else if (!masterReferenceExists(masterReference))
    {
        errors.append(QObject::tr("Bus interface %1 referenced in %2 within %3 does not exist").
            arg(masterReference, subspace->name(), context));
    }
    else if (!referencedBusIsMaster(masterReference))
    {
        errors.append(QObject::tr("Bus interface %1 referenced in %2 within %3 is not a master").
            arg(masterReference, subspace->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapValidator::findErrorsInSegmentReference()
//-----------------------------------------------------------------------------
void SubspaceMapValidator::findErrorsInSegmentReference(QVector<QString>& errors,
    QSharedPointer<SubSpaceMap> subspace, QString const& context) const
{
    QString segmentReference = subspace->getSegmentReference();
    if (!segmentReference.isEmpty() &&
        !spaceContainsSegmentReference(subspace->getMasterReference(), segmentReference))
    {
        errors.append(QObject::tr(
            "Segment %1 does not exist in address space of bus interface %2 referenced in %3 within %4").
            arg(segmentReference, subspace->getMasterReference(), subspace->name(), context));
    }
}
