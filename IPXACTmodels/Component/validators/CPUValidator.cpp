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

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: CPUValidator::CPUValidator()
//-----------------------------------------------------------------------------
CPUValidator::CPUValidator(QSharedPointer<ParameterValidator2014> parameterValidator,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces):
parameterValidator_(parameterValidator),
    expressionParser_(expressionParser),
    addressSpaces_(addressSpaces)
{

}

//-----------------------------------------------------------------------------
// Function: CPUValidator::~CPUValidator()
//-----------------------------------------------------------------------------
CPUValidator::~CPUValidator()
{

}

//-----------------------------------------------------------------------------
// Function: CPUValidator::componentChange()
//-----------------------------------------------------------------------------
void CPUValidator::componentChange(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces)
{
    addressSpaces_ = newAddressSpaces;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::validate()
//-----------------------------------------------------------------------------
bool CPUValidator::validate(QSharedPointer<Cpu> cpu) const
{
	if (!hasValidName( cpu->name()))
	{
		return false;
	}
	else if (!cpu->getIsPresent().isEmpty() && !expressionParser_->isValidExpression(cpu->getIsPresent()))
	{
		return false;
	}

    if (!hasValidAddressSpaceReferences(cpu))
    {
        return false;
    }


	foreach (QSharedPointer<Parameter> currentPara, *cpu->getParameters())
	{
		if (!parameterValidator_->hasValidValue(currentPara))
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

    foreach (QSharedPointer<Cpu::AddressSpaceRef> currentRef, *cpu->getAddressSpaceReferences())
    {
        if (!isValidAddressSpaceReference(currentRef->getAddressSpaceRef()))
        {
            return false;
        }
        else if (!currentRef->getIsPresent().isEmpty() && 
            !expressionParser_->isValidExpression(currentRef->getIsPresent()))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void CPUValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> cpu,
    QString const& context) const
{
	if (!hasValidName(cpu->name()))
	{
        errors.append(QObject::tr("Invalid name '%1' set for CPU within %2.").arg(cpu->name()).arg(context));
	}

	if (!cpu->getIsPresent().isEmpty() && !expressionParser_->isValidExpression( cpu->getIsPresent()))
	{
		errors.append(QObject::tr("Is present expression '%1' in cpu %2 is invalid.").arg(
            cpu->getIsPresent(), cpu->name()));
	}
	
	if (cpu->getAddressSpaceReferences()->count() < 1)
	{
		errors.append(QObject::tr("No address space reference set for CPU %1 within %2.")
            .arg(cpu->name()).arg(context));
	}

	foreach (QSharedPointer<Cpu::AddressSpaceRef> currentRef, *cpu->getAddressSpaceReferences())
	{
        if (!isValidAddressSpaceReference(currentRef->getAddressSpaceRef()))
        {
            errors.append(QObject::tr("Address space '%1' referenced within cpu %2 not found.").arg(
                currentRef->getAddressSpaceRef(), cpu->name()));
        }
		
		if (!currentRef->getIsPresent().isEmpty() &&
            !expressionParser_->isValidExpression(currentRef->getIsPresent()))
		{
			errors.append(QObject::tr(
                "Is present expression '%1' for address space reference %2 in cpu %3 is invalid."
				).arg(currentRef->getIsPresent(), currentRef->getAddressSpaceRef(), cpu->name()));
		}
	}

    QString cpuContext = QObject::tr("cpu %1").arg(cpu->name());
	foreach (QSharedPointer<Parameter> currentPara, *cpu->getParameters())
	{
		parameterValidator_->findErrorsIn(errors, currentPara, cpuContext);
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression("^\\s*$");

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::isValidAddressSpaceReference()
//-----------------------------------------------------------------------------
bool CPUValidator::isValidAddressSpaceReference(QString const& reference) const
{
    if (!reference.isEmpty() && addressSpaces_)
    {
        foreach (QSharedPointer<AddressSpace> addressSpace, *addressSpaces_)
        {
            if (addressSpace->name() == reference)
            {
                return true;
            }
        }
    }

    return false;
}
