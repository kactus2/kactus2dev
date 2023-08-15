//-----------------------------------------------------------------------------
// File: ModeValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Validator for ipxact:mode.
//-----------------------------------------------------------------------------

#include "ModeValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Mode.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>



#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ModeValidator::ModeValidator()
//-----------------------------------------------------------------------------
ModeValidator::ModeValidator(QSharedPointer<Component> component, 
	QSharedPointer<ExpressionParser> expressionParser):
	component_(component),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: ModeValidator::~ModeValidator()
//-----------------------------------------------------------------------------
ModeValidator::~ModeValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ModeValidator::validate()
//-----------------------------------------------------------------------------
bool ModeValidator::validate(QSharedPointer<Mode> mode) const
{
	if (hasValidName(mode->name()) == false)
	{
		return false;
	}
	
	if (hasValidPortSlices(mode) == false)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ModeValidator::hasValidName(QString const& name) const
{
	return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::hasValidPortSlices()
//-----------------------------------------------------------------------------
bool ModeValidator::hasValidPortSlices(QSharedPointer<Mode> mode) const
{
	for (auto const& portSlice : *mode->getPortSlices())
	{
		if (hasValidName(portSlice->name()) == false)
		{
			return false;
		}

		if (component_ && component_->hasPort(portSlice->getPortRef()) == false)
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ModeValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Mode> mode,
    QString const& context) const
{
    if (!hasValidName(mode->name()))
    {
        errors.append(QObject::tr("Invalid name '%1' set for mode within %2.").arg(mode->name(), context));
    }

    for (auto const& portSlice : *mode->getPortSlices())
    {
        if (hasValidName(portSlice->name()) == false)
        {
            errors.append(QObject::tr("Invalid name '%1' set for port condition within mode %2.").arg(
				portSlice->name(), mode->name()));
        }

		if (portSlice->getPortRef().isEmpty())
		{
			errors.append(QObject::tr("No port reference set for '%1' within mode %2.").arg(
				portSlice->name(), mode->name()));
		}
		else if (component_ && component_->hasPort(portSlice->getPortRef()) == false)
		{
			errors.append(QObject::tr(
				"Port '%1' in port condition '%2' in mode '%3' could not be found in the component.").arg(
					portSlice->getPortRef(), portSlice->name(), mode->name()));
		}
    }
}
