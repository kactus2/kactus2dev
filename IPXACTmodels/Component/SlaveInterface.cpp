//-----------------------------------------------------------------------------
// File: slaveinterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.10.2015
//
// Description:
// Implementation of ipxact:slave in bus interface.
//-----------------------------------------------------------------------------

#include "SlaveInterface.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: SlaveInterface::Bridge::Bridge()
//-----------------------------------------------------------------------------
SlaveInterface::Bridge::Bridge(): masterRef_(), isPresent_()
{
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::SlaveInterface()
//-----------------------------------------------------------------------------
SlaveInterface::SlaveInterface():
memoryMapRef_(), 
bridges_(new QList<QSharedPointer<Bridge> >()), 
fileSetRefGroup_(new QList<QSharedPointer<SlaveInterface::FileSetRefGroup> >())
{
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::SlaveInterface()
//-----------------------------------------------------------------------------
SlaveInterface::SlaveInterface(SlaveInterface const& other):
memoryMapRef_(other.memoryMapRef_),
bridges_(new QList<QSharedPointer<Bridge> >()),
fileSetRefGroup_(new QList<QSharedPointer<SlaveInterface::FileSetRefGroup> >()) 
{
    foreach (QSharedPointer<Bridge> bridge, *other.bridges_)
    {
        QSharedPointer<Bridge> copy(new Bridge(*bridge));
        bridges_->append(copy);
    }

    foreach (QSharedPointer<FileSetRefGroup> refGroup, *other.fileSetRefGroup_)
    {
        QSharedPointer<FileSetRefGroup> copy(new FileSetRefGroup(*refGroup.data()));
        fileSetRefGroup_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::operator=()
//-----------------------------------------------------------------------------
SlaveInterface& SlaveInterface::operator=(SlaveInterface const& other)
{
	if (this != &other)
    {
        memoryMapRef_ = other.memoryMapRef_;

        bridges_->clear();
        foreach (QSharedPointer<Bridge> bridge, *other.bridges_) 
        {
            QSharedPointer<Bridge> copy(new Bridge(*bridge));
            bridges_->append(copy);
        }

        fileSetRefGroup_.clear();
        foreach (QSharedPointer<FileSetRefGroup> refGroup, *other.fileSetRefGroup_)
        {
            QSharedPointer<FileSetRefGroup> copy(new FileSetRefGroup(*refGroup.data()));
            fileSetRefGroup_->append(copy);
        }
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::~SlaveInterface()
//-----------------------------------------------------------------------------
SlaveInterface::~SlaveInterface()
{
	bridges_->clear();
	fileSetRefGroup_->clear();
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::getMemoryMapRef()
//-----------------------------------------------------------------------------
QString SlaveInterface::getMemoryMapRef() const
{
    return memoryMapRef_;
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::setMemoryMapRef()
//-----------------------------------------------------------------------------
void SlaveInterface::setMemoryMapRef(QString const& memoryMapRef)
{
    memoryMapRef_ = memoryMapRef;
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::getBridges()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<SlaveInterface::Bridge> > > SlaveInterface::getBridges() const
{
	return bridges_;
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::hasBridge()
//-----------------------------------------------------------------------------
bool SlaveInterface::hasBridge() const
{
    return !bridges_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::getMasterReferences()
//-----------------------------------------------------------------------------
QStringList SlaveInterface::getMasterReferences() const 
{
    QStringList masterNames;
    foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *bridges_)
    {
        masterNames.append(bridge->masterRef_);
    }

    return masterNames;
}

//-----------------------------------------------------------------------------
// Function: SlaveInterface::getFileSetRefGroup()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<SlaveInterface::FileSetRefGroup> > > SlaveInterface::getFileSetRefGroup() const
{
    return fileSetRefGroup_;
}
