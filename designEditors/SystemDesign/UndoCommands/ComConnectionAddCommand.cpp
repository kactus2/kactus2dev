//-----------------------------------------------------------------------------
// File: ComConnectionAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2016
//
// Description:
// Undo add commands for COM connections.
//-----------------------------------------------------------------------------

#include "ComConnectionAddCommand.h"

#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/SystemDesignDiagram.h>
#include <designEditors/SystemDesign/SWPortItem.h>

#include <common/graphicsItems/ConnectionEndpoint.h>
#include <common/graphicsItems/ComponentItem.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::ComConnectionAddCommand()
//-----------------------------------------------------------------------------
ComConnectionAddCommand::ComConnectionAddCommand(QGraphicsScene* scene, ComGraphicsConnection* connection,
                                                 QSharedPointer<Design> containingDesign, QUndoCommand* parent):
QUndoCommand(parent),
connection_(connection),
scene_(scene),
del_(false),
containingDesign_(containingDesign)
{

}

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::~ComConnectionAddCommand()
//-----------------------------------------------------------------------------
ComConnectionAddCommand::~ComConnectionAddCommand()
{
    if (del_)
    {
        delete connection_;
    }
}

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::undo()
//-----------------------------------------------------------------------------
void ComConnectionAddCommand::undo()
{
    scene_->removeItem(connection_);
    connection_->setSelected(false);

    connection_->disconnectEnds();

    QList<QSharedPointer<ComInterconnection> > comInterconnections = containingDesign_->getComConnections();
    comInterconnections.removeAll(connection_->getComInterconnection());
    containingDesign_->setComConnections(comInterconnections);

    containingDesign_->removeRoute(connection_->getRouteExtension());

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::redo()
//-----------------------------------------------------------------------------
void ComConnectionAddCommand::redo()
{
    if (!scene_->items().contains(connection_))
    {
        scene_->addItem(connection_);
    }

    if (connection_->connectEnds())
    {
        ConnectionEndpoint* startPoint = connection_->endpoint1();
        ConnectionEndpoint* endPoint = connection_->endpoint2();

        if (startPoint->getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
        {
            changePortItemFromApiToCom(startPoint);
        }
        else if (endPoint->getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
        {
            changePortItemFromApiToCom(endPoint);
        }

        QList<QSharedPointer<ComInterconnection> > comInterconnections = containingDesign_->getComConnections();
        comInterconnections.append(connection_->getComInterconnection());
        containingDesign_->setComConnections(comInterconnections);

        containingDesign_->addRoute(connection_->getRouteExtension());
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::changePortItemFromApiToCom()
//-----------------------------------------------------------------------------
void ComConnectionAddCommand::changePortItemFromApiToCom(ConnectionEndpoint* endPoint)
{
    SWPortItem* portEndpoint = dynamic_cast<SWPortItem*>(endPoint);
    if (portEndpoint)
    {
        QSharedPointer<ComponentInstance> containingInstance = endPoint->encompassingComp()->getComponentInstance();

        QMap<QString, QPointF> apiPositions = containingInstance->getApiInterfacePositions();

        if (apiPositions.contains(endPoint->name()) && apiPositions.value(endPoint->name()) == endPoint->pos())
        {
            containingInstance->removeApiInterfacePosition(endPoint->name());
        }

        containingInstance->updateComInterfacePosition(endPoint->name(), endPoint->pos());
    }
}
