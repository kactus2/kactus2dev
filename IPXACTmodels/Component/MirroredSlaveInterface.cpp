//-----------------------------------------------------------------------------
// File: mirroredslaveinterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:mirroredSlave in bus interface.
//-----------------------------------------------------------------------------

#include "MirroredSlaveInterface.h"

#include <QMap>
#include <QString>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::RemapAddress::RemapAddress()
//-----------------------------------------------------------------------------
MirroredSlaveInterface::RemapAddress::RemapAddress(QString const& remapAddress):
remapAddress_(remapAddress),
    state_(),
    remapAttributes_()
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredSlaveInterface::MirroredSlaveInterface():
range_(),
    remapAddresses_(new QList<QSharedPointer<RemapAddress> >())
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredSlaveInterface::MirroredSlaveInterface(MirroredSlaveInterface const& other):
range_(other.range_),
    remapAddresses_(new QList<QSharedPointer<RemapAddress> >())
{
    foreach (QSharedPointer<RemapAddress> remapAddr, *other.remapAddresses_)
    {
        QSharedPointer<RemapAddress> copy(new RemapAddress(*remapAddr.data()));
        remapAddresses_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::operator=()
//-----------------------------------------------------------------------------
MirroredSlaveInterface& MirroredSlaveInterface::operator=(MirroredSlaveInterface const& other)
{
	if (this != &other)
    {
		range_ = other.range_;

        remapAddresses_->clear();
        foreach (QSharedPointer<RemapAddress> remapAddr, *other.remapAddresses_)
        {
            QSharedPointer<RemapAddress> copy(new RemapAddress(*remapAddr.data()));
            remapAddresses_->append(copy);
        }

    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::~MirroredSlaveInterface()
//-----------------------------------------------------------------------------
MirroredSlaveInterface::~MirroredSlaveInterface()
{
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::getRange()
//-----------------------------------------------------------------------------
QString MirroredSlaveInterface::getRange() const
{
	return range_;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::setRange()
//-----------------------------------------------------------------------------
void MirroredSlaveInterface::setRange(const QString& range)
{
	range_ = range;
}


//-----------------------------------------------------------------------------
// Function: mirroredslaveinterface::getRemapAddress()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MirroredSlaveInterface::RemapAddress> > > 
    MirroredSlaveInterface::getRemapAddresses() const
{
	return remapAddresses_;
}

//-----------------------------------------------------------------------------
// Function: MirroredSlaveInterface::setRemapAddress()
//-----------------------------------------------------------------------------
void MirroredSlaveInterface::setRemapAddress(QString const& newRemapAddress)
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
            QSharedPointer<RemapAddress> firstRemapAddress = getRemapAddresses()->first();
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
