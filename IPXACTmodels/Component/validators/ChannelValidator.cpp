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

#include <IPXACTmodels/Component/BusInterface.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ChannelValidator::ChannelValidator()
//-----------------------------------------------------------------------------
ChannelValidator::ChannelValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<BusInterface> > > componentBusInterfaces):
busInterfaces_(componentBusInterfaces),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::componentChange()
//-----------------------------------------------------------------------------
void ChannelValidator::componentChange(QSharedPointer<QList<QSharedPointer<BusInterface> > > newBusInterfaces)
{
    busInterfaces_ = newBusInterfaces;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::validate()
//-----------------------------------------------------------------------------
bool ChannelValidator::validate(QSharedPointer<Channel> channel) const
{
	if (!hasValidName(channel->name()))
	{
		return false;
	}
	
    bool isValidPresence = false;
    expressionParser_->parseExpression(channel->getIsPresent(), &isValidPresence);
    if (isValidPresence == false)
	{
		return false;
	}
	
    if (!hasValidBusInterfaceReferences(channel))
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::hasValidBusInterfaceReferences()
//-----------------------------------------------------------------------------
bool ChannelValidator::hasValidBusInterfaceReferences(QSharedPointer<Channel> channel) const
{
    if (channel->getInterfaces()->count() < 2)
    {
        return false;
    }

    for (auto const& currentRef : *channel->getInterfaces())
    {
        if (!isReferenceToMirroredInterface(currentRef->localName_))
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
    QString const& context) const
{
	if (!hasValidName(channel->name()))
	{
		errors.append(QObject::tr("Invalid name %1 specified for channel within %2.")
            .arg(channel->name()).arg(context));
	}

    bool isValidPresence = false;
    expressionParser_->parseExpression(channel->getIsPresent(), &isValidPresence);
	if (isValidPresence == false)
	{
        errors.append(QObject::tr("Is present expression '%1' in channel %2 is invalid.").arg(
            channel->getIsPresent(), channel->name()));
	}
	
	if (channel->getInterfaces()->count() < 2)
	{
		errors.append(QObject::tr("At least two interfaces must be defined for channel %1.").arg(channel->name()));
	}

	for (auto const& currentRef : *channel->getInterfaces())
	{
		if (!isValidBusIntefaceReference(currentRef->localName_))
		{
			errors.append(QObject::tr("Bus interface '%1' referenced within channel %2 not found.").arg(
                currentRef->localName_, channel->name()));
		}
        else if (!isReferenceToMirroredInterface(currentRef->localName_))
        {
            errors.append(QObject::tr("Bus interface '%1' referenced within channel %2 is not a mirrored interface."
                ).arg(currentRef->localName_, channel->name()));
        }
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ChannelValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression(QStringLiteral("^\\s*$"));

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::isValidBusIntefaceReference()
//-----------------------------------------------------------------------------
bool ChannelValidator::isValidBusIntefaceReference(QString const& interfaceName) const
{
    if (busInterfaces_)
    {
        for (QSharedPointer<BusInterface> const& busInterface : *busInterfaces_)
        {
            if (busInterface->name() == interfaceName)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::isReferenceToMirroredInterface()
//-----------------------------------------------------------------------------
bool ChannelValidator::isReferenceToMirroredInterface(QString const& interfaceName) const
{
    if (busInterfaces_)
    {
        for (QSharedPointer<BusInterface> const& busInterface : *busInterfaces_)
        {
            if (busInterface->name() == interfaceName)
            {
                return busInterface->getInterfaceMode() == General::MIRROREDMASTER ||
                    busInterface->getInterfaceMode() == General::MIRROREDSLAVE ||
                    busInterface->getInterfaceMode() == General::MIRROREDSYSTEM;
            }
        }
    }

    return false;
}
    