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

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::CellSpecificationValidator()
//-----------------------------------------------------------------------------
CellSpecificationValidator::CellSpecificationValidator()
{
	functions_.append("nd2");
	functions_.append("buf");
	functions_.append("inv");
	functions_.append("mux21");
	functions_.append("dff");
	functions_.append("latch");
	functions_.append("xor2");
	functions_.append("other");
}

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::~CellSpecificationValidator()
//-----------------------------------------------------------------------------
CellSpecificationValidator::~CellSpecificationValidator()
{

}

//-----------------------------------------------------------------------------
// Function: CellSpecificationValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool CellSpecificationValidator::validate(QSharedPointer<CellSpecification> cellSpecification) const
{
	// Must be known cell strength.
	if ( cellSpecification->getCellStrength() == CellSpecification::UNKNOWN )
	{
		return false;
	}

	// Must be supported function.
	if ( !functions_.contains( cellSpecification->getCellFunction() ) )
	{
		// Alternatively, must be known cell strength.
		if ( cellSpecification->getCellClass() == CellSpecification::NO_CLASS )
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
	QSharedPointer<CellSpecification> cellSpecification, QString const& contex) const
{
	// Must be known cell strength.
	if ( cellSpecification->getCellStrength() == CellSpecification::UNKNOWN )
	{
		errors.append(QObject::tr("Unknown cell strength in cell specification."));
	}

	// Must be supported function.
	if ( !functions_.contains( cellSpecification->getCellFunction() ) )
	{
		// Alternatively, must be known cell strength.
		if ( cellSpecification->getCellClass() == CellSpecification::NO_CLASS )
		{
			errors.append(QObject::tr("No class specified for cell specification."));
		}
		else
		{
			errors.append(QObject::tr("Invalid function in cell specification: %1")
				.arg(cellSpecification->getCellFunction()));
		}
	}
}
