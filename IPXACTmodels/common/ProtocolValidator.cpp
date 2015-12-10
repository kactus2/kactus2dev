//-----------------------------------------------------------------------------
// Protocol: ProtocolValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 07.12.2015
//
// Description:
// Validator for ipxact:Protocol.
//-----------------------------------------------------------------------------

#include "ProtocolValidator.h"

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::ProtocolValidator()
//-----------------------------------------------------------------------------
ProtocolValidator::ProtocolValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::~ProtocolValidator()
//-----------------------------------------------------------------------------
ProtocolValidator::~ProtocolValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool ProtocolValidator::validate(QSharedPointer<Protocol> protocol) const
{
	if ( protocol->getProtocolType() != "tlm" && protocol->getProtocolType() != "custom" )
	{
		return false;
	}

	if ( !protocol->getPayloadName().isEmpty() && !hasValidName( protocol->getPayloadName() ) )
	{
		return false;
	}

	if ( protocol->getPayloadType() != "generic" && protocol->getPayloadType() != "specific" )
	{
		return false;
	}

	if ( !protocol->getPayloadExtension().isEmpty() && protocol->getPayloadExtension() != "mandatory" &&
		protocol->getPayloadExtension() != "optional" )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ProtocolValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<Protocol> protocol, QString const&) const
{
	if ( protocol->getProtocolType() != "tlm" && protocol->getProtocolType() != "custom" )
	{
		errors.append(QObject::tr("The transactional protocol type is invalid: %1").arg(protocol->getProtocolType()));
	}

	if ( !protocol->getPayloadName().isEmpty() && !hasValidName( protocol->getPayloadName() ) )
	{
		errors.append(QObject::tr("The payload name is invalid: %1").arg(protocol->getPayloadName()));
	}

	if ( protocol->getPayloadType() != "generic" && protocol->getPayloadType() != "specific" )
	{
		errors.append(QObject::tr("The payload type is invalid: %1").arg(protocol->getPayloadType()));
	}

	if ( !protocol->getPayloadExtension().isEmpty() && protocol->getPayloadExtension() != "mandatory"
		&& protocol->getPayloadExtension() != "optional" )
	{
		errors.append(QObject::tr("The payload extension is invalid: %1").arg(protocol->getPayloadExtension()));
	}
}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ProtocolValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression;
	whiteSpaceExpression.setPattern("^\\s*$");
	QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

	if (name.isEmpty() || whiteSpaceMatch.hasMatch())
	{
		return false;
	}

	return true;
}
