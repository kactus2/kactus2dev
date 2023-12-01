//-----------------------------------------------------------------------------
// File: CPUValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 18.11.2015
//
// Description:
// Validator for ipxact:CPU.
//-----------------------------------------------------------------------------

#include "CPUValidator.h"

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/MemoryReserve.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: CPUValidator::CPUValidator()
//-----------------------------------------------------------------------------
CPUValidator::CPUValidator(QSharedPointer<ParameterValidator> parameterValidator,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces, 
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    Document::Revision revision) :
    parameterValidator_(parameterValidator),
    regionValidator_(new RegionValidator(expressionParser)),
    expressionParser_(expressionParser),
    addressSpaces_(addressSpaces),
    memoryMaps_(memoryMaps),
    revision_(revision)
{

}

//-----------------------------------------------------------------------------
// Function: CPUValidator::componentChange()
//-----------------------------------------------------------------------------
void CPUValidator::componentChange(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    Document::Revision revision)
{
    addressSpaces_ = newAddressSpaces;
    memoryMaps_ = memoryMaps;
    revision_ = revision;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::validate()
//-----------------------------------------------------------------------------
bool CPUValidator::validate(QSharedPointer<Cpu> cpu) const
{
	if (hasValidName( cpu->name()) == false)
	{
		return false;
	}

    if (revision_ == Document::Revision::Std14)
    {
        bool isValidPresence = false;
        expressionParser_->parseExpression(cpu->getIsPresent(), &isValidPresence);
        if (isValidPresence == false)
        {
            return false;
        }

        if (hasValidAddressSpaceReferences(cpu) == false)
        {
            return false;
        }
    }

    if (revision_ == Document::Revision::Std22 &&
        (hasValidMemoryMapReference(cpu) == false || hasValidRange(cpu) == false || 
         hasValidWidth(cpu) == false || hasValidAddressUnitBits(cpu) == false ||
         hasValidRegions(cpu) == false))
    {
        return false;
    }

	for (QSharedPointer<Parameter> const& currentPara : *cpu->getParameters())
	{
		if (parameterValidator_->hasValidValue(currentPara) == false)
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidAddressSpaceReferences()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidAddressSpaceReferences(QSharedPointer<Cpu> cpu) const
{
    if (cpu->getAddressSpaceReferences()->count() < 1)
    {
        return false;
    }

    for (QSharedPointer<Cpu::AddressSpaceRef> currentRef : *cpu->getAddressSpaceReferences())
    {
        if (isValidAddressSpaceReference(currentRef->getAddressSpaceRef()) == false)
        {
            return false;
        }
        
	    bool isValidPresence = false;
        expressionParser_->parseExpression(currentRef->getIsPresent(), &isValidPresence);
        if (isValidPresence == false)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidMemoryMapReference()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidMemoryMapReference(QSharedPointer<Cpu> cpu) const
{
    QString ref = cpu->getMemoryMapReference();
    if (ref.isEmpty())
    {
        return false;
    }

    if (memoryMaps_)
    {
        return std::any_of(memoryMaps_->cbegin(), memoryMaps_->cend(),
            [&ref](auto const& memoryMap) {return memoryMap->name() == ref; });
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidRegions()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidRegions(QSharedPointer<Cpu> cpu) const
{
    if (cpu->getRegions()->isEmpty())
    {
        return true;
    }

    MemoryReserve reservedArea;

    QStringList regionNames;

    qint64 availableRange = expressionParser_->parseExpression(cpu->getRange()).toLongLong();

    for (QSharedPointer<Region> region : *cpu->getRegions())
    {
        qint64 regionBegin = expressionParser_->parseExpression(region->getAddressOffset()).toLongLong();
        qint64 regionRange = expressionParser_->parseExpression(region->getRange()).toLongLong();
        qint64 regionEnd = regionBegin + regionRange - 1;

        if (regionNames.contains(region->name()) ||
            regionValidator_->validate(region) == false ||
            regionEnd > availableRange)
        {
            return false;
        }
        else
        {
            regionNames.append(region->name());
            reservedArea.addArea(region->name(), regionBegin, regionEnd);
        }
    }

    return reservedArea.hasOverlap() == false;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void CPUValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> cpu,
    QString const& context) const
{
	if (hasValidName(cpu->name()) == false)
	{
        errors.append(QObject::tr("Invalid name '%1' set for CPU within %2.").arg(cpu->name(), context));
	}

    if (revision_ == Document::Revision::Std14)
    {
        bool isValidPresence = false;
        expressionParser_->parseExpression(cpu->getIsPresent(), &isValidPresence);
        if (isValidPresence == false)
        {
            errors.append(QObject::tr("Is present expression '%1' in CPU %2 is invalid.").arg(
                cpu->getIsPresent(), cpu->name()));
        }

        if (cpu->getAddressSpaceReferences()->count() < 1)
        {
            errors.append(QObject::tr("No address space reference set for CPU %1 within %2.")
                .arg(cpu->name()).arg(context));
        }

        for (QSharedPointer<Cpu::AddressSpaceRef> const& currentRef : *cpu->getAddressSpaceReferences())
        {
            if (!isValidAddressSpaceReference(currentRef->getAddressSpaceRef()))
            {
                errors.append(QObject::tr("Address space '%1' referenced within CPU %2 is not found.").arg(
                    currentRef->getAddressSpaceRef(), cpu->name()));
            }

            bool isValidRefPresence = false;
            expressionParser_->parseExpression(currentRef->getIsPresent(), &isValidRefPresence);
            if (isValidRefPresence == false)
            {
                errors.append(QObject::tr(
                    "Is present expression '%1' for address space reference %2 in CPU %3 is invalid."
                ).arg(currentRef->getIsPresent(), currentRef->getAddressSpaceRef(), cpu->name()));
            }
        }
    }
	
    if (revision_ == Document::Revision::Std22)
    {
        if (cpu->getMemoryMapReference().isEmpty())
        {
            errors.append(QObject::tr("No memory map reference set for CPU %1 within %2.").arg(
                cpu->name(), context));
        }
        else if (isValidMemoryMapReference(cpu->getMemoryMapReference()) == false)
        {
            errors.append(QObject::tr("Memory map '%1' referenced within CPU %2 is not found.").arg(
                cpu->getMemoryMapReference(), cpu->name()));
        }

        if (hasValidRange(cpu) == false)
        {
            errors.append(QObject::tr("Invalid range set for CPU %1.").arg(cpu->name()));
        }

        if (hasValidWidth(cpu) == false)
        {
            errors.append(QObject::tr("Invalid width set for CPU %1.").arg(cpu->name()));
        }

        if (hasValidAddressUnitBits(cpu) == false)
        {
            errors.append(QObject::tr("Invalid address unit bits set for CPU %1.").arg(cpu->name()));
        }

        findErrorsInRegions(errors, cpu, context);
    }

    QString cpuContext = QObject::tr("cpu %1").arg(cpu->name());
	for (QSharedPointer<Parameter> const& currentPara : *cpu->getParameters())
	{
		parameterValidator_->findErrorsIn(errors, currentPara, cpuContext);
	}
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::findErrorsInRegions()
//-----------------------------------------------------------------------------
void CPUValidator::findErrorsInRegions(QVector<QString>& errors,
    QSharedPointer<Cpu> cpu, QString const& context) const
{
    if (cpu->getRegions()->isEmpty() == false)
    {
        qint64 availableRange = expressionParser_->parseExpression(cpu->getRange()).toLongLong();

        MemoryReserve reservedArea;
        QStringList regionNames;
        QStringList foundNames;

        for (QSharedPointer<Region> const& region : *cpu->getRegions())
        {
            regionValidator_->findErrorsIn(errors, region, context);

            if (regionNames.contains(region->name()) && !foundNames.contains(region->name()))
            {
                errors.append(QObject::tr("Name %1 of regions in %2 is not unique.").arg(region->name(), context));
                foundNames.append(region->name());
            }
            else
            {
                regionNames.append(region->name());
            }

            qint64 regionBegin = expressionParser_->parseExpression(region->getAddressOffset()).toLongLong();
            qint64 regionRange = expressionParser_->parseExpression(region->getRange()).toLongLong();
            qint64 regionEnd = regionBegin + regionRange - 1;

            reservedArea.addArea(region->name(), regionBegin, regionEnd);

            if (regionEnd > availableRange)
            {
                errors.append(QObject::tr("Region %1 is not contained within address space of CPU %2.").arg(
                    region->name(), cpu->name()));
            }
        }

        reservedArea.findErrorsInOverlap(errors, QLatin1String("Regions"),
            QObject::tr("address space of CPU %1").arg(cpu->name()));
    }
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::getRegionValidator()
//-----------------------------------------------------------------------------
QSharedPointer<RegionValidator> CPUValidator::getRegionValidator() const
{
    return regionValidator_;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidName(QString const& name) const
{
    return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::isValidAddressSpaceReference()
//-----------------------------------------------------------------------------
bool CPUValidator::isValidAddressSpaceReference(QString const& reference) const
{
    if (!reference.isEmpty() && addressSpaces_)
    {
        return std::any_of(addressSpaces_->cbegin(), addressSpaces_->cend(),
            [&reference](auto const& addressSpace) {return addressSpace->name() == reference; });
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::isValidMemoryMapReference()
//-----------------------------------------------------------------------------
bool CPUValidator::isValidMemoryMapReference(QString const& reference) const
{
    if (memoryMaps_)
    {
        return std::any_of(memoryMaps_->cbegin(), memoryMaps_->cend(),
            [&reference](auto const& memoryMap) {return memoryMap->name() == reference; });
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidRange()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidRange(QSharedPointer<Cpu> cpu) const
{
    auto range = cpu->getRange();
    return range.isEmpty() == false && CommonItemsValidator::isValidExpression(range, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidWidth()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidWidth(QSharedPointer<Cpu> cpu) const
{
    auto width = cpu->getWidth();
    return width.isEmpty() == false && CommonItemsValidator::isValidExpression(width, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::hasValidAddressUnitBits()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidAddressUnitBits(QSharedPointer<Cpu> cpu) const
{
    auto aub = cpu->getAddressUnitBits();
    return aub.isEmpty() || CommonItemsValidator::isValidExpression(aub, expressionParser_);
}
