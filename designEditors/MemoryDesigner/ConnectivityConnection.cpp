//-----------------------------------------------------------------------------
// File: ConnectivityConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A connection in a connectivity graph.
//-----------------------------------------------------------------------------

#include "ConnectivityConnection.h"

//-----------------------------------------------------------------------------
// Function: ConnectivityConnection::ConnectivityConnection()
//-----------------------------------------------------------------------------
ConnectivityConnection::ConnectivityConnection(QString const& name, QSharedPointer<ConnectivityInterface> startPoint, 
    QSharedPointer<ConnectivityInterface> endPoint): name_(name), firstInterface_(startPoint), secondInterface_(endPoint)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityConnection::~ConnectivityConnection()
//-----------------------------------------------------------------------------
ConnectivityConnection::~ConnectivityConnection()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityConnection::getName()
//-----------------------------------------------------------------------------
QString ConnectivityConnection::getName() const
{
    return name_;         
}

//-----------------------------------------------------------------------------
// Function: ConnectivityConnection::getFirstInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityConnection::getFirstInterface() const
{
    return firstInterface_;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityConnection::getSecondInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityConnection::getSecondInterface() const
{
    return secondInterface_;
}
