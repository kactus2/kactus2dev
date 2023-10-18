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
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/utilities/Search.h>

#include <KactusAPI/include/ExpressionParser.h>

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
	
    if (CommonItemsValidator::hasValidIsPresent(channel->getIsPresent(), expressionParser_) == false)
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

    return std::all_of(channel->getInterfaces()->cbegin(), channel->getInterfaces()->cend(),
        [this](auto const& currentRef) { return isReferenceToMirroredInterface(currentRef->localName_); });
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

	if (CommonItemsValidator::hasValidIsPresent(channel->getIsPresent(), expressionParser_) == false)
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
	return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::isValidBusIntefaceReference()
//-----------------------------------------------------------------------------
bool ChannelValidator::isValidBusIntefaceReference(QString const& interfaceName) const
{
    if (busInterfaces_ == nullptr)
    {
        return false;
    }

    auto const& busInterface = Search::findByName(interfaceName, *busInterfaces_);
    return busInterface.isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: ChannelValidator::isReferenceToMirroredInterface()
//-----------------------------------------------------------------------------
bool ChannelValidator::isReferenceToMirroredInterface(QString const& interfaceName) const
{
    if (busInterfaces_ == nullptr)
    {
        return false;
    }

    auto const& busInterface = Search::findByName(interfaceName, *busInterfaces_);
    if (busInterface == nullptr)
    {
        return false;
    }

    auto const& mode = busInterface->getInterfaceMode();
    return mode == General::MIRRORED_MASTER ||
        mode == General::MIRRORED_SLAVE ||
        mode == General::MIRRORED_INITIATOR ||
        mode == General::MIRRORED_TARGET ||
        mode == General::MIRRORED_SYSTEM;
}
    