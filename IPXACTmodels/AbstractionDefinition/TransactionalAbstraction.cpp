//-----------------------------------------------------------------------------
// File: TransactionalAbstraction.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation for ipaxct:transactional within abstraction definition.
//-----------------------------------------------------------------------------

#include "TransactionalAbstraction.h"

#include "TransactionalPort.h"

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <QList>
#include <QObject>
#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::TransactionalAbstraction()
//-----------------------------------------------------------------------------
TransactionalAbstraction::TransactionalAbstraction() :
qualifier_(),
    onSystem_(new QList<QSharedPointer<TransactionalPort> >()),
    onMaster_(),
    onSlave_()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::TransactionalAbstraction()
//-----------------------------------------------------------------------------
TransactionalAbstraction::TransactionalAbstraction(TransactionalAbstraction const& other):
qualifier_(other.qualifier_),
    onSystem_(new QList<QSharedPointer<TransactionalPort> >()),
    onMaster_(),
    onSlave_()
{
	if (other.onMaster_)
    {
		onMaster_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onMaster_.data()));
	}
	if (other.onSlave_)
    {
		onSlave_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onSlave_.data()));
	}

    Utilities::copyList(onSystem_, other.onSystem_);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::()
//-----------------------------------------------------------------------------
TransactionalAbstraction& TransactionalAbstraction::operator=(TransactionalAbstraction const& other )
{
	if (this != &other)
    {
	    qualifier_ = other.qualifier_;

		if (other.onMaster_)
        {
			onMaster_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onMaster_.data()));
		}
		else
        {
			onMaster_ = QSharedPointer<TransactionalPort>();
        }
		if (other.onSlave_)
        {
			onSlave_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onSlave_.data()));
		}
		else
        {
			onSlave_ = QSharedPointer<TransactionalPort>();
        }

        Utilities::copyList(onSystem_, other.onSystem_);
    }
	return *this;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::~TransactionalAbstraction()
//-----------------------------------------------------------------------------
TransactionalAbstraction::~TransactionalAbstraction()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getQualifier()
//-----------------------------------------------------------------------------
Qualifier TransactionalAbstraction::getQualifier() const
{
	return qualifier_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setQualifier(Qualifier::Type qualifierType) 
{
    qualifier_.setType(qualifierType);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::addSystemPort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::addSystemPort(QSharedPointer<TransactionalPort> systemPort)
{
    onSystem_->append(systemPort);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getSystemPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TransactionalPort> > > TransactionalAbstraction::getSystemPorts() const
{
    return onSystem_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasMasterPort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasMasterPort() const
{
    return !onMaster_.isNull();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setMasterPort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setMasterPort(QSharedPointer<TransactionalPort> masterPort)
{
	onMaster_ = masterPort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getMasterPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getMasterPort() const
{
    return onMaster_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasSlavePort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasSlavePort() const
{
    return !onSlave_.isNull();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setSlavePort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setSlavePort(QSharedPointer<TransactionalPort> slavePort)
{
    onSlave_ = slavePort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getSlavePort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getSlavePort() const
{
	return onSlave_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasMode(General::InterfaceMode mode, QString const& systemGroup) const
{
    if (mode == General::SYSTEM || mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup);
        return !systemPort.isNull();
    }

    return (mode == General::MASTER && hasMasterPort()) ||
        (mode == General::MIRROREDMASTER && hasMasterPort()) ||
        (mode == General::SLAVE && hasSlavePort()) ||
        (mode == General::MIRROREDSLAVE && hasSlavePort());
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::findSystemPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::findSystemPort(QString const& systemGroup) const
{
    foreach(QSharedPointer<TransactionalPort> systemPort, *onSystem_)
    {
        if (systemPort->getSystemGroup() == systemGroup)
        {
            return systemPort;
        }
    }

    return QSharedPointer<TransactionalPort>();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getInitiative()
//-----------------------------------------------------------------------------
QString TransactionalAbstraction::getInitiative(General::InterfaceMode mode, QString const& systemGroup) const
{
    QString initiative = QLatin1String("");

    if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && hasMasterPort())
    {
        initiative = getMasterPort()->getInitiative();
    }
    else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && hasSlavePort())
    {
        initiative = getSlavePort()->getInitiative();
    }
    else if (mode == General::SYSTEM || mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup);
        if (systemPort)
        {
            initiative = systemPort->getInitiative();
        }
    }

    if (mode == General::MIRROREDMASTER || mode == General::MIRROREDSLAVE || mode == General::MIRROREDSYSTEM)
    {
        TransactionalTypes::Initiative mirroredInitiative = TransactionalTypes::convertToMirrored(initiative);
        initiative = TransactionalTypes::initiativeToString(mirroredInitiative);
    }

    return initiative;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getWidth()
//-----------------------------------------------------------------------------
QString TransactionalAbstraction::getWidth(General::InterfaceMode mode, QString const& systemGroup) const
{
    if ((mode == General::MASTER || mode == General::MIRROREDMASTER) && hasMasterPort())
    {
        return getMasterPort()->getBusWidth();
    }
    else if ((mode == General::SLAVE || mode == General::MIRROREDSLAVE) && hasSlavePort())
    {
        return getSlavePort()->getBusWidth();
    }
    else if (mode == General::SYSTEM || mode == General::MIRROREDSYSTEM)
    {
        QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup);

        if (systemPort)
        {
            return systemPort->getBusWidth();
        }
    }

    return QStringLiteral("");
}
