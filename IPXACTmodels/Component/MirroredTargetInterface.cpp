//-----------------------------------------------------------------------------
// File: MirroredTargetInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.06.2023
//
// Description:
// Implementation of ipxact:mirroredTarget in bus interface.
//-----------------------------------------------------------------------------

#include "MirroredTargetInterface.h"

#include <QMap>
#include <QString>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::RemapAddress::RemapAddress()
//-----------------------------------------------------------------------------
MirroredTargetInterface::RemapAddress::RemapAddress(QString const& remapAddress):
remapAddress_(remapAddress)
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredTargetInterface::MirroredTargetInterface()
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredTargetInterface::MirroredTargetInterface(MirroredTargetInterface const& other):
range_(other.range_)
{
    for (QSharedPointer<RemapAddress> remapAddr : *other.remapAddresses_)
    {
        QSharedPointer<RemapAddress> copy(new RemapAddress(*remapAddr));
        remapAddresses_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::operator=()
//-----------------------------------------------------------------------------
MirroredTargetInterface& MirroredTargetInterface::operator=(MirroredTargetInterface const& other)
{
	if (this != &other)
    {
		range_ = other.range_;

        remapAddresses_->clear();
        for (QSharedPointer<RemapAddress> remapAddr : *other.remapAddresses_)
        {
            QSharedPointer<RemapAddress> copy(new RemapAddress(*remapAddr));
            remapAddresses_->append(copy);
        }

    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::~MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredTargetInterface::~MirroredTargetInterface()
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::getRange()
//-----------------------------------------------------------------------------
QString MirroredTargetInterface::getRange() const
{
	return range_;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::setRange()
//-----------------------------------------------------------------------------
void MirroredTargetInterface::setRange(const QString& range)
{
	range_ = range;
}

//-----------------------------------------------------------------------------
// Function: mirroredslaveinterface::getRemapAddress()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MirroredTargetInterface::RemapAddress> > > 
    MirroredTargetInterface::getRemapAddresses() const
{
	return remapAddresses_;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::setRemapAddress()
//-----------------------------------------------------------------------------
void MirroredTargetInterface::setRemapAddress(QString const& newRemapAddress)
{
    if (!newRemapAddress.isEmpty())
    {
        if (getRemapAddresses()->isEmpty())
        {
            QSharedPointer<RemapAddress> remapAddress (new RemapAddress(newRemapAddress));
            getRemapAddresses()->append(remapAddress);
        }
        else
        {
            QSharedPointer<RemapAddress> firstRemapAddress = remapAddresses_->first();
            firstRemapAddress->remapAddress_ = newRemapAddress;
        }
    }
    else
    {
        if (!getRemapAddresses()->isEmpty())
        {
            getRemapAddresses()->removeFirst();
        }
    }
}
