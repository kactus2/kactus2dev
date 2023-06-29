//-----------------------------------------------------------------------------
// File: TargetInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.06.2023
//
// Description:
// Implementation of ipxact:target in bus interface.
//-----------------------------------------------------------------------------

#include "TargetInterface.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: TargetInterface::TargetInterface()
//-----------------------------------------------------------------------------
TargetInterface::TargetInterface()
{
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::TargetInterface()
//-----------------------------------------------------------------------------
TargetInterface::TargetInterface(TargetInterface const& other) :
    memoryMapRef_(other.memoryMapRef_),
    modeRefs_(other.modeRefs_)
{
    for (QSharedPointer<TransparentBridge> bridge : *other.bridges_)
    {
        QSharedPointer<TransparentBridge> copy(new TransparentBridge(*bridge));
        bridges_->append(copy);
    }

    for (QSharedPointer<FileSetRefGroup> refGroup : *other.fileSetRefGroup_)
    {
        QSharedPointer<FileSetRefGroup> copy(new FileSetRefGroup(*refGroup));
        fileSetRefGroup_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::operator=()
//-----------------------------------------------------------------------------
TargetInterface& TargetInterface::operator=(TargetInterface const& other)
{
	if (this != &other)
    {
        memoryMapRef_ = other.memoryMapRef_;

        modeRefs_ = other.modeRefs_;

        bridges_->clear();
        for (QSharedPointer<TransparentBridge> bridge : *other.bridges_) 
        {
            QSharedPointer<TransparentBridge> copy(new TransparentBridge(*bridge));
            bridges_->append(copy);
        }

        fileSetRefGroup_.clear();
        for (QSharedPointer<FileSetRefGroup> refGroup: *other.fileSetRefGroup_)
        {
            QSharedPointer<FileSetRefGroup> copy(new FileSetRefGroup(*refGroup));
            fileSetRefGroup_->append(copy);
        }
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::~TargetInterface()
//-----------------------------------------------------------------------------
TargetInterface::~TargetInterface()
{
	bridges_->clear();
	fileSetRefGroup_->clear();
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::getMemoryMapRef()
//-----------------------------------------------------------------------------
QString TargetInterface::getMemoryMapRef() const
{
    return memoryMapRef_;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::setMemoryMapRef()
//-----------------------------------------------------------------------------
void TargetInterface::setMemoryMapRef(QString const& memoryMapRef)
{
    memoryMapRef_ = memoryMapRef;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::getModeRefs()
//-----------------------------------------------------------------------------
QStringList TargetInterface::getModeRefs() const
{
    return modeRefs_;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::setModeRefs()
//-----------------------------------------------------------------------------
void TargetInterface::setModeRefs(QStringList const& modeRefs)
{
    modeRefs_ = modeRefs;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::getBridges()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TransparentBridge> > > TargetInterface::getBridges() const
{
	return bridges_;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::hasBridge()
//-----------------------------------------------------------------------------
bool TargetInterface::hasBridge() const
{
    return !bridges_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::getMasterReferences()
//-----------------------------------------------------------------------------
QStringList TargetInterface::getMasterReferences() const 
{
    QStringList masterNames;
    for (QSharedPointer<TransparentBridge> bridge: *bridges_)
    {
        masterNames.append(bridge->getMasterRef());
    }

    return masterNames;
}

//-----------------------------------------------------------------------------
// Function: TargetInterface::getFileSetRefGroup()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TargetInterface::FileSetRefGroup> > > TargetInterface::getFileSetRefGroup() const
{
    return fileSetRefGroup_;
}
