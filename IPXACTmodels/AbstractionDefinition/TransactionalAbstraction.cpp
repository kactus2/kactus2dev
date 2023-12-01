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

#include <IPXACTmodels/utilities/Copy.h>

#include <QList>
#include <QObject>
#include <QString>
#include <QSharedPointer>


//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::TransactionalAbstraction()
//-----------------------------------------------------------------------------
TransactionalAbstraction::TransactionalAbstraction(TransactionalAbstraction const& other):
qualifier_(new Qualifier(*other.qualifier_))
{
	if (other.onInitiator_)
    {
		onInitiator_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onInitiator_));
	}

	if (other.onTarget_)
    {
		onTarget_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onTarget_));
	}

    Copy::copyList(other.onSystem_, onSystem_);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::()
//-----------------------------------------------------------------------------
TransactionalAbstraction& TransactionalAbstraction::operator=(TransactionalAbstraction const& other )
{
	if (this != &other)
    {
	    qualifier_ = other.qualifier_;

        onInitiator_.clear();
		if (other.onInitiator_)
        {
			onInitiator_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onInitiator_));
		}

        onTarget_.clear();
		if (other.onTarget_)
        {
			onTarget_ = QSharedPointer<TransactionalPort>(new TransactionalPort(*other.onTarget_));
		}

        onSystem_->clear();
        Copy::copyList(other.onSystem_, onSystem_);
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
QSharedPointer<Qualifier> TransactionalAbstraction::getQualifier()
{
	return qualifier_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setQualifier(QSharedPointer<Qualifier> qualifier)
{
    qualifier_ = qualifier;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::addQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::addQualifier(Qualifier::Type qualifierType)
{
    qualifier_->setType(qualifierType);
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
// Function: TransactionalAbstraction::hasInitiatorPort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasInitiatorPort() const
{
    return !onInitiator_.isNull();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setInitiatorPort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setInitiatorPort(QSharedPointer<TransactionalPort> initiatorPort)
{
    onInitiator_ = initiatorPort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getInitiatorPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getInitiatorPort() const
{
    return onInitiator_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasTargetPort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasTargetPort() const
{
    return !onTarget_.isNull();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setTargetPort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setTargetPort(QSharedPointer<TransactionalPort> targetPort)
{
    onTarget_ = targetPort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getTargetPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getTargetPort() const
{
    return onTarget_;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasMasterPort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasMasterPort() const
{
    return hasInitiatorPort();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setMasterPort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setMasterPort(QSharedPointer<TransactionalPort> masterPort)
{
    setInitiatorPort(masterPort);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getMasterPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getMasterPort() const
{
    return getInitiatorPort();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasSlavePort()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasSlavePort() const
{
    return hasTargetPort();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::setSlavePort()
//-----------------------------------------------------------------------------
void TransactionalAbstraction::setSlavePort(QSharedPointer<TransactionalPort> slavePort)
{
    setTargetPort(slavePort);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getSlavePort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::getSlavePort() const
{
    return getTargetPort();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::hasMode()
//-----------------------------------------------------------------------------
bool TransactionalAbstraction::hasMode(General::InterfaceMode mode, QString const& systemGroup) const
{
    if (mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM)
    {
        QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup);
        return !systemPort.isNull();
    }

    return (mode == General::MASTER && hasMasterPort()) ||
        (mode == General::MIRRORED_MASTER && hasMasterPort()) ||
        (mode == General::INITIATOR && hasMasterPort()) ||
        (mode == General::MIRRORED_INITIATOR && hasMasterPort()) ||
        (mode == General::SLAVE && hasSlavePort()) ||
        (mode == General::MIRRORED_SLAVE && hasSlavePort()) ||
        (mode == General::TARGET && hasSlavePort()) ||
        (mode == General::MIRRORED_TARGET && hasSlavePort());
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::findSystemPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstraction::findSystemPort(QString const& systemGroup) const
{
    for (QSharedPointer<TransactionalPort> systemPort : *onSystem_)
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
    using namespace TransactionalTypes;

    if ((mode == General::MASTER || mode == General::INITIATOR) && hasMasterPort())
    {
        return getMasterPort()->getInitiative();
    }
    else if ((mode == General::MIRRORED_MASTER || mode == General::MIRRORED_INITIATOR) && hasMasterPort())
    {
        return initiativeToString(convertToMirrored(getMasterPort()->getInitiative()));
    }
    else if ((mode == General::SLAVE || mode == General::TARGET) && hasSlavePort())
    {
        return getSlavePort()->getInitiative();
    }
    else if ((mode == General::MIRRORED_SLAVE || mode == General::MIRRORED_TARGET) && hasSlavePort())
    {
        return initiativeToString(convertToMirrored(getSlavePort()->getInitiative()));
    }
    else if (mode == General::SYSTEM)
    {
        if (QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup))
        {
            return systemPort->getInitiative();
        }
    }
    else if (mode == General::MIRRORED_SYSTEM)
    {
        if (QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup))
        {
            return initiativeToString(convertToMirrored(systemPort->getInitiative()));
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstraction::getWidth()
//-----------------------------------------------------------------------------
QString TransactionalAbstraction::getWidth(General::InterfaceMode mode, QString const& systemGroup) const
{
    if ((mode == General::MASTER || mode == General::MIRRORED_MASTER ||
        mode == General::INITIATOR || mode == General::MIRRORED_INITIATOR) && hasMasterPort())
    {
        return getMasterPort()->getBusWidth();
    }
    else if ((mode == General::SLAVE || mode == General::MIRRORED_SLAVE ||
        mode == General::TARGET || mode == General::MIRRORED_TARGET) && hasSlavePort())
    {
        return getSlavePort()->getBusWidth();
    }
    else if (mode == General::SYSTEM || mode == General::MIRRORED_SYSTEM)
    {
        if (QSharedPointer<TransactionalPort> systemPort = findSystemPort(systemGroup))
        {
            return systemPort->getBusWidth();
        }
    }

    return QString();
}
