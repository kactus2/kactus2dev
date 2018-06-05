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
