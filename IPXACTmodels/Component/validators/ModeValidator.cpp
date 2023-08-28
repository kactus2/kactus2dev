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

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ModeValidator::ModeValidator()
//-----------------------------------------------------------------------------
ModeValidator::ModeValidator(QSharedPointer<PortSliceValidator> sliceValidator, 
	QSharedPointer<Component> component, 
	QSharedPointer<ExpressionParser> expressionParser):
	sliceValidator_(sliceValidator),
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
	QStringList sliceNames;

	for (auto const& portSlice : *mode->getPortSlices())
    {
        if (sliceNames.contains(portSlice->name()))
        {
            return false;
        }

		if (sliceValidator_->validate(portSlice) == false)
		{
			return false;
		}

		sliceNames.append(portSlice->name());
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

	QString modeContext(QStringLiteral("mode '%1'").arg(mode->name()));
	QStringList sliceNames;
	QStringList reportedNames;

    for (auto const& portSlice : *mode->getPortSlices())
    {
		if (sliceNames.contains(portSlice->name()) && reportedNames.contains(portSlice->name()) == false)
        {
            errors.append(QObject::tr("Port condition name '%1' is not unique within %2.").arg(
                portSlice->name(), modeContext));

			reportedNames.append(portSlice->name());
		}
        sliceNames.append(portSlice->name());

		sliceValidator_->findErrorsIn(errors, portSlice, modeContext);
    }
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::componentChange()
//-----------------------------------------------------------------------------
void ModeValidator::componentChange(QSharedPointer<Component> newComponent)
{
	component_ = newComponent;
	sliceValidator_->componentChange(newComponent);
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::getPortSliceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortSliceValidator> ModeValidator::getPortSliceValidator() const
{
	return sliceValidator_;
}
