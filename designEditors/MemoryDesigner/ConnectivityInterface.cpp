//-----------------------------------------------------------------------------
// File: ConnectivityInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// An interface in a connectivity graph.
//-----------------------------------------------------------------------------

#include "ConnectivityInterface.h"

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::ConnectivityInterface()
//-----------------------------------------------------------------------------
ConnectivityInterface::ConnectivityInterface(QString const& name): name_(name), mode_(), baseAddress_(),
    remapAddress_(), instance_(), memory_(), hierarchical_(false), bridged_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::~ConnectivityInterface()
//-----------------------------------------------------------------------------
ConnectivityInterface::~ConnectivityInterface()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getName()
//-----------------------------------------------------------------------------
QString ConnectivityInterface::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setMode()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setMode(QString const& mode)
{
    mode_ = mode;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getMode()
//-----------------------------------------------------------------------------
QString ConnectivityInterface::getMode() const
{
    return mode_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setBaseAddress()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setBaseAddress(QString const& address)
{
    baseAddress_ = address;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getBaseAddress()
//-----------------------------------------------------------------------------
QString ConnectivityInterface::getBaseAddress() const
{
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setRemapAddress()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setRemapAddress(QString const& address)
{
    remapAddress_ = address;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getRemapAddress()
//-----------------------------------------------------------------------------
QString ConnectivityInterface::getRemapAddress() const
{
    return remapAddress_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setRemapRange()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setRemapRange(QString const& range)
{
    range_ = range;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getRemapRange()
//-----------------------------------------------------------------------------
QString ConnectivityInterface::getRemapRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setInstance()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setInstance(QSharedPointer<ConnectivityComponent> instance)
{
    instance_ = instance;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityComponent> ConnectivityInterface::getInstance() const
{
    return instance_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::isConnectedToMemory()
//-----------------------------------------------------------------------------
bool ConnectivityInterface::isConnectedToMemory() const
{
    return !memory_.isNull();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setConnectedMemory()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setConnectedMemory(QSharedPointer<MemoryItem> item)
{
    memory_ = item;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::getConnectedMemory()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityInterface::getConnectedMemory() const
{
    return memory_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setHierarchical()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setHierarchical()
{
    hierarchical_ = true;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::isHierarchical()
//-----------------------------------------------------------------------------
bool ConnectivityInterface::isHierarchical() const
{
    return hierarchical_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::setBridgd()
//-----------------------------------------------------------------------------
void ConnectivityInterface::setBridged()
{
    bridged_ = true;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityInterface::isBridged()
//-----------------------------------------------------------------------------
bool ConnectivityInterface::isBridged() const
{
    return bridged_;
}