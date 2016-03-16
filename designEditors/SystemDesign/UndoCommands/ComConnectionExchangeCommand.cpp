//-----------------------------------------------------------------------------
// File: ComConnectionExchangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.03.2016
//
// Description:
// Undo command for exchanging COM connections.
//-----------------------------------------------------------------------------

#include "ComConnectionExchangeCommand.h"

#include <common/graphicsItems/ConnectionEndpoint.h>
#include <common/graphicsItems/ComponentItem.h>

#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/SWPortItem.h>
#include <designEditors/SystemDesign/SWInterfaceItem.h>

#include <IPXACTmodels/Design/HierInterface.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::ComConnectionExchangeCommand()
//-----------------------------------------------------------------------------
ComConnectionExchangeCommand::ComConnectionExchangeCommand(ComGraphicsConnection* connection,
                                                           ConnectionEndpoint* oldEndpoint,
                                                           ConnectionEndpoint* newEndpoint, QUndoCommand* parent):
QUndoCommand(parent),
connection_(connection),
oldEndpoint_(oldEndpoint),
newEndpoint_(newEndpoint),
oldInterface_(),
newInterface_()
{
    createExchangedInterfaces();
}

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::createExchangedInterfaces()
//-----------------------------------------------------------------------------
void ComConnectionExchangeCommand::createExchangedInterfaces()
{
    if (connection_->isOffPage())
    {
        ConnectionEndpoint* oldParentPoint = dynamic_cast<ConnectionEndpoint*>(oldEndpoint_->parentItem());
        if (oldParentPoint)
        {
            oldInterface_ = createConnectionInterface(oldParentPoint);
        }

        ConnectionEndpoint* newParentPoint = dynamic_cast<ConnectionEndpoint*>(newEndpoint_->parentItem());
        if (newParentPoint)
        {
            newInterface_ = createConnectionInterface(newParentPoint);
        }
    }
    else
    {
        oldInterface_ = createConnectionInterface(oldEndpoint_);
        newInterface_ = createConnectionInterface(newEndpoint_);
    }
}

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::~ComConnectionExchangeCommand()
//-----------------------------------------------------------------------------
ComConnectionExchangeCommand::~ComConnectionExchangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::undo()
//-----------------------------------------------------------------------------
void ComConnectionExchangeCommand::undo()
{
    QUndoCommand::undo();

    QSharedPointer<ComInterconnection> interconnection = connection_->getComInterconnection();

    if (interconnection && oldInterface_)
    {
        if (connection_->endpoint1() == newEndpoint_)
        {
            connection_->setEndpoint1(oldEndpoint_);
            
            QSharedPointer<ActiveInterface> oldActiveInterface = oldInterface_.dynamicCast<ActiveInterface>();
            if (oldActiveInterface)
            {
                interconnection->setStartInterface(oldActiveInterface);
            }
        }
        else
        {
            connection_->setEndpoint2(oldEndpoint_);
            
            interconnection->setInterface(oldInterface_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::redo()
//-----------------------------------------------------------------------------
void ComConnectionExchangeCommand::redo()
{
    QSharedPointer<ComInterconnection> interconnection = connection_->getComInterconnection();

    if (interconnection && newInterface_)
    {
        if (connection_->endpoint1() == oldEndpoint_)
        {
            connection_->setEndpoint1(newEndpoint_);
            
            QSharedPointer<ActiveInterface> newActiveInterface = newInterface_.dynamicCast<ActiveInterface>();
            if (newActiveInterface)
            {
                interconnection->setStartInterface(newActiveInterface);
            }
        }
        else
        {
            connection_->setEndpoint2(newEndpoint_);
            
            interconnection->setInterface(newInterface_);
        }
    }

    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ComConnectionExchangeCommand::createConnectionInterface()
//-----------------------------------------------------------------------------
QSharedPointer<HierInterface> ComConnectionExchangeCommand::createConnectionInterface(ConnectionEndpoint* endPoint)
{
    SWPortItem* portEndPoint = dynamic_cast<SWPortItem*>(endPoint);
    SWInterfaceItem* interfaceEndPoint = dynamic_cast<SWInterfaceItem*>(endPoint);
    if (portEndPoint)
    {
        QString instanceName = portEndPoint->encompassingComp()->getComponentInstance()->getInstanceName();
        return QSharedPointer<ActiveInterface> (new ActiveInterface(instanceName, endPoint->name()));
    }
    else if (interfaceEndPoint)
    {
        return QSharedPointer<HierInterface> (new HierInterface(endPoint->name()));
    }

    return QSharedPointer<HierInterface> ();
}
