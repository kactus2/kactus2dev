//-----------------------------------------------------------------------------
// File: MemoryRemap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------

#include "MemoryRemap.h"

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(QString const& name /* = QString() */, QString const& remapState /* = QString() */):
MemoryMapBase(name),
remapState_(remapState)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(const MemoryRemap& other):
MemoryMapBase(other),
remapState_(other.remapState_)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::operator=()
//-----------------------------------------------------------------------------
MemoryRemap& MemoryRemap::operator=(const MemoryRemap& other)
{
    if (this != &other)
    {
        MemoryMapBase::operator=(other);
        remapState_ = other.remapState_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::~MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::~MemoryRemap()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getRemapState()
//-----------------------------------------------------------------------------
QString MemoryRemap::getRemapState() const
{
    return remapState_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setRemapState()
//-----------------------------------------------------------------------------
void MemoryRemap::setRemapState(QString newRemapState)
{
    remapState_ = newRemapState;
}
/*
//-----------------------------------------------------------------------------
// Function: MemoryRemap::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames, QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices, errorList, parentIdentifier);

    if (!remapStateNames.contains(remapState_))
    {
        if (remapState_.isEmpty())
        {
            errorList.append(QObject::tr("No remap state assigned in memory remap %1 of %2").arg(name()).
                arg(parentIdentifier));
        }
        else
        {
            errorList.append(QObject::tr("Remap state %1 in memory remap %2 of %3 is not a remap state of component").
                arg(remapState_).arg(name()).arg(parentIdentifier));
        }
    }

    return valid && !remapState_.isEmpty() && remapStateNames.contains(remapState_);
}*/
/*
//-----------------------------------------------------------------------------
// Function: MemoryRemap::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList componentRemapStateNames) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices);

    return valid && !remapState_.isEmpty() && componentRemapStateNames.contains(remapState_);
}*/
