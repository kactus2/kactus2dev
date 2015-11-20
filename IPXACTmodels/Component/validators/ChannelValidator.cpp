//-----------------------------------------------------------------------------
// File: ChannelValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Channel.
//-----------------------------------------------------------------------------

#include "ChannelValidator.h"

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ChannelValidator::ChannelValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
ChannelValidator::~ChannelValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::validate()
//-----------------------------------------------------------------------------
bool ChannelValidator::validate(QSharedPointer<Channel> channel) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( channel->name() ) )
	{
		return false;
	}
	else if ( !channel->getIsPresent().isEmpty() &&
		!parser->isValidExpression( channel->getIsPresent() ) )
	{
		return false;
	}
	else if ( channel->getInterfaces().count() < 2 )
	{
		return false;
	}

	foreach ( QString currentRef, channel->getInterfaces() )
	{
		if ( !currentRef.isEmpty() && !hasValidName( currentRef ) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ChannelValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Channel> channel,
    QString const&) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( channel->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(channel->name()));
	}

	if ( !channel->getIsPresent().isEmpty() && !parser->isValidExpression( channel->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(channel->getIsPresent()));
	}
	
	if ( channel->getInterfaces().count() < 2 )
	{
		errors.append(QObject::tr("Must have at least two bus interface references."));
	}

	foreach ( QString currentRef, channel->getInterfaces() )
	{
		if ( !hasValidName( currentRef ) )
		{
			errors.append(QObject::tr("A bus interface reference has an invalid name: %1"
				).arg(currentRef));
		}
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ChannelValidator::hasValidName(QString const& name) const
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
