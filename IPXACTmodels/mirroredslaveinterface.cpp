/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "mirroredslaveinterface.h"

#include <QMap>
#include <QString>
#include <QObject>

MirroredSlaveInterface::RemapAddress::RemapAddress(const QString& remapAddress):
remapAddress_(remapAddress),
state_(),
remapAttributes_()
{
}

MirroredSlaveInterface::MirroredSlaveInterface():
range_(), remapAddresses_()
{
}

MirroredSlaveInterface::MirroredSlaveInterface( const MirroredSlaveInterface& other ):
range_(other.range_),
remapAddresses_()
{
	foreach (QSharedPointer<RemapAddress> remapAddr, other.remapAddresses_)
	{
		if (remapAddr)
		{
			QSharedPointer<RemapAddress> copy = QSharedPointer<RemapAddress>(
				new RemapAddress(*remapAddr.data()));
			remapAddresses_.append(copy);
		}
	}

}


MirroredSlaveInterface& MirroredSlaveInterface::operator=( const MirroredSlaveInterface& other )
{
	if (this != &other) {

		range_ = other.range_;

		remapAddresses_.clear();
		foreach (QSharedPointer<RemapAddress> remapAddr, other.remapAddresses_)
		{
			if (remapAddr)
			{
				QSharedPointer<RemapAddress> copy = QSharedPointer<RemapAddress>(
					new RemapAddress(*remapAddr.data()));
				remapAddresses_.append(copy);
			}
		}

	}
	return *this;
}


// the destructor
MirroredSlaveInterface::~MirroredSlaveInterface()
{
}

QString MirroredSlaveInterface::getRange() const
{
	return range_;
}

void MirroredSlaveInterface::setRange(const QString& range)
{
	range_ = range;
}

//-----------------------------------------------------------------------------
// Function: mirroredslaveinterface::setRemapAddress()
//-----------------------------------------------------------------------------
void MirroredSlaveInterface::setRemapAddresses( const QList<QSharedPointer<RemapAddress> > remapAddresses )
{
	remapAddresses_.clear();
	remapAddresses_ = remapAddresses;
}

//-----------------------------------------------------------------------------
// Function: mirroredslaveinterface::getRemapAddress()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MirroredSlaveInterface::RemapAddress> > MirroredSlaveInterface::getRemapAddresses() const
{
	return remapAddresses_;
}
