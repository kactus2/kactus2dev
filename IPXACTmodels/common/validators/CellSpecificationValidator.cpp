//-----------------------------------------------------------------------------
// CellSpecification: CellSpecificationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 10.12.2015
//
// Description:
// Validator for ipxact:CellSpecification.
//-----------------------------------------------------------------------------

#include "CellSpecificationValidator.h"

#include <IPXACTmodels/common/CellSpecification.h>

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::CellSpecificationValidator()
//-----------------------------------------------------------------------------
CellSpecificationValidator::CellSpecificationValidator() :
functions_({QStringLiteral("nd2"),
    QStringLiteral("buf"),
    QStringLiteral("inv"),
    QStringLiteral("mux21"),
    QStringLiteral("dff"),
    QStringLiteral("latch"),
    QStringLiteral("xor2"),
    QStringLiteral("other")})
{
	
}

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool CellSpecificationValidator::validate(QSharedPointer<CellSpecification> cellSpecification) const
{
	// Must be known cell strength.
	if (cellSpecification->getCellStrength() == CellSpecification::UNKNOWN)
	{
		return false;
	}

	// Must be supported function.
	if (!functions_.contains(cellSpecification->getCellFunction()))
	{
		// Alternatively, must be known cell strength.
		if (cellSpecification->getCellClass() == CellSpecification::NO_CLASS)
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void CellSpecificationValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<CellSpecification> cellSpecification, QString const& context) const
{
	// Must be known cell strength.
	if (cellSpecification->getCellStrength() == CellSpecification::UNKNOWN)
	{
		errors.append(QObject::tr("Unknown cell strength in cell specification in %1.").arg(context));
	}

	// Must be supported function.
	if (!functions_.contains(cellSpecification->getCellFunction()))
	{
		// Alternatively, must be known cell strength.
		if (cellSpecification->getCellClass() == CellSpecification::NO_CLASS)
		{
			errors.append(QObject::tr("No class specified for cell specification in %1.").arg(context));
		}
		else
		{
			errors.append(QObject::tr("Invalid function '%1' in cell specification in %2.").arg(
                cellSpecification->getCellFunction(), context));
		}
	}
}
