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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ChannelValidator::ChannelValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<BusInterface> > > componentBusInterfaces):
busInterfaces_(componentBusInterfaces),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
ChannelValidator::~ChannelValidator()
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
	else if (!channel->getIsPresent().isEmpty() && !expressionParser_->isValidExpression(channel->getIsPresent()))
	{
		return false;
	}
	else if (!hasValidBusInterfaceReferences(channel))
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
    if (channel->getInterfaces().count() < 2)
    {
        return false;
    }

    foreach (QString const& currentRef, channel->getInterfaces())
    {
        if (!isReferenceToMirroredInterface(currentRef))
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

	if (!channel->getIsPresent().isEmpty() && !expressionParser_->isValidExpression(channel->getIsPresent()))
	{
        errors.append(QObject::tr("Is present expression '%1' in channel %2 is invalid.").arg(
            channel->getIsPresent(), channel->name()));
	}
	
	if (channel->getInterfaces().count() < 2)
	{
		errors.append(QObject::tr("At least two interfaces must be defined for channel %1.").arg(channel->name()));
	}

	foreach (QString const& currentRef, channel->getInterfaces())
	{
		if (!isValidBusIntefaceReference(currentRef))
		{
			errors.append(QObject::tr("Bus interface '%1' referenced within channel %2 not found.").arg(
                currentRef, channel->name()));
		}
        else if (!isReferenceToMirroredInterface(currentRef))
        {
            errors.append(QObject::tr("Bus interface '%1' referenced within channel %2 is not a mirrored interface."
                ).arg(currentRef, channel->name()));
        }
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ChannelValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression("^\\s*$");

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
        foreach (QSharedPointer<BusInterface> busInterface, *busInterfaces_)
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
        foreach (QSharedPointer<BusInterface> busInterface, *busInterfaces_)
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
    