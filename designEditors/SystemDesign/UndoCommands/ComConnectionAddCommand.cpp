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

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ComConnectionAddCommand::ComConnectionAddCommand()
//-----------------------------------------------------------------------------
ComConnectionAddCommand::ComConnectionAddCommand(QGraphicsScene* scene, ComGraphicsConnection* connection,
                                                 QSharedPointer<Design> containingDesign,
                                                 QUndoCommand* parent /* = 0 */):
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
        QList<QSharedPointer<ComInterconnection> > comInterconnections = containingDesign_->getComConnections();
        comInterconnections.append(connection_->getComInterconnection());
        containingDesign_->setComConnections(comInterconnections);

        containingDesign_->addRoute(connection_->getRouteExtension());
    }

    del_ = false;
}
