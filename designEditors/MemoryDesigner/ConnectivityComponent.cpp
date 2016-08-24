//-----------------------------------------------------------------------------
// File: ConnectivityComponent.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A component instance in a connectivity graph.
//-----------------------------------------------------------------------------

#include "ConnectivityComponent.h"

#include "MemoryItem.h"

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::ConnectivityComponent()
//-----------------------------------------------------------------------------
ConnectivityComponent::ConnectivityComponent(QString const& name): name_(name), vlnv_(), uuid_(), activeView_(), 
    memories_()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::~ConnectivityComponent()
//-----------------------------------------------------------------------------
ConnectivityComponent::~ConnectivityComponent()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::getName()
//-----------------------------------------------------------------------------
QString ConnectivityComponent::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::setInstanceUuid()
//-----------------------------------------------------------------------------
void ConnectivityComponent::setInstanceUuid(QString const& uuid)
{
    uuid_ = uuid;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::getInstanceUuid()
//-----------------------------------------------------------------------------
QString ConnectivityComponent::getInstanceUuid() const
{
    return uuid_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::setVlnv()
//-----------------------------------------------------------------------------
void ConnectivityComponent::setVlnv(QString const& vlnv)
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::getVlnv()
//-----------------------------------------------------------------------------
QString ConnectivityComponent::getVlnv() const
{
    return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::setActiveView()
//-----------------------------------------------------------------------------
void ConnectivityComponent::setActiveView(QString const& viewName)
{
    activeView_ = viewName;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::addMemory()
//-----------------------------------------------------------------------------
void ConnectivityComponent::addMemory(QSharedPointer<MemoryItem> spaceItem)
{
    memories_.append(spaceItem);

}

//-----------------------------------------------------------------------------
// Function: ConnectivityComponent::getMemories()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<MemoryItem> > ConnectivityComponent::getMemories() const
{
    return memories_;
}
