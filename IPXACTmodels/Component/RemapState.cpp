//-----------------------------------------------------------------------------
// File: RemapState.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:remapState element.
//-----------------------------------------------------------------------------

#include "RemapState.h"
#include "RemapPort.h"

//-----------------------------------------------------------------------------
// Function: RemapState::RemapState()
//-----------------------------------------------------------------------------
RemapState::RemapState(QString const& name /* = QString() */) :
NameGroup(name)
{

}

//-----------------------------------------------------------------------------
// Function: RemapState::RemapState()
//-----------------------------------------------------------------------------
RemapState::RemapState(const RemapState &other) :
NameGroup(other)
{
    copyRemapPorts(other);
}

//-----------------------------------------------------------------------------
// Function: RemapState::operator=()
//-----------------------------------------------------------------------------
RemapState& RemapState::operator=(const RemapState& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);

        remapPorts_->clear();
        copyRemapPorts(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RemapState::~RemapState()
//-----------------------------------------------------------------------------
RemapState::~RemapState()
{

}

//-----------------------------------------------------------------------------
// Function: RemapState::getRemapPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<RemapPort> > > RemapState::getRemapPorts() const
{
    return remapPorts_;
}

//-----------------------------------------------------------------------------
// Function: RemapState::setRemapPorts()
//-----------------------------------------------------------------------------
void RemapState::setRemapPorts(QSharedPointer<QList<QSharedPointer<RemapPort> > > newRemapPorts)
{
    remapPorts_ = newRemapPorts;
}

//-----------------------------------------------------------------------------
// Function: RemapState::copyRemapPorts()
//-----------------------------------------------------------------------------
void RemapState::copyRemapPorts(const RemapState& other)
{
    for (QSharedPointer<RemapPort> remapPort : *other.remapPorts_)
    {
        auto copy = QSharedPointer<RemapPort>(new RemapPort(*remapPort));
        remapPorts_->append(copy);
    }
}