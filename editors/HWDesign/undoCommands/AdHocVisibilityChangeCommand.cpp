//-----------------------------------------------------------------------------
// File: AdHocVisibilityChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for changing port adhoc visibility in design diagram.
//-----------------------------------------------------------------------------

#include "AdHocVisibilityChangeCommand.h"

#include <common/graphicsItems/GraphicsColumn.h>

#include <editors/common/DesignDiagram.h>

#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/AdHocEnabled.h>
#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/HWConnectionEndpoint.h>
#include <editors/HWDesign/AdHocConnectionItem.h>
#include <editors/HWDesign/undoCommands/AdHocConnectionDeleteCommand.h>

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName,
                                                           bool newVisibility, QUndoCommand* parent):
QUndoCommand(parent),
dataSource_(dataSource),
portName_(portName),
pos_(),
newVisibility_(newVisibility),
adhocPort_()
{
    HWConnectionEndpoint* port = dataSource_->getDiagramAdHocPort(portName.toStdString());

    if (port)
    {
        adhocPort_ = dynamic_cast<AdHocItem*>(port);
        
        if (!newVisibility_ && adhocPort_)
        {
            pos_ = port->scenePos();

            for (GraphicsConnection* connection : adhocPort_->getConnections())
            {
                createConnectionDeleteCommand(connection);
            }

            for (GraphicsConnection* connection : adhocPort_->getOffPageConnector()->getConnections())
            {
                createConnectionDeleteCommand(connection);
            }
        }
    }
    else
    {
        adhocPort_ = dataSource_->createAdhocItem(portName.toStdString());
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::createConnectionDeleteCommand()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::createConnectionDeleteCommand(GraphicsConnection* connection)
{
    DesignDiagram* adHocDiagram = dynamic_cast<DesignDiagram*>(dataSource_);
    if (adHocDiagram)
    {
        AdHocConnectionItem* adHocConnection = dynamic_cast<AdHocConnectionItem*>(connection);
        if (adHocConnection)
        {
            new AdHocConnectionDeleteCommand(adHocDiagram, adHocConnection, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::undo()
{
    dataSource_->changeAdhocVisibility(adhocPort_, !newVisibility_);

    changePortItemVisibility(!newVisibility_);

    if (!newVisibility_)
    {
        adhocPort_->setPos(adhocPort_->parentItem()->mapFromScene(pos_));

        if (adhocPort_->type() == GFX_TYPE_DIAGRAM_ADHOC_INTERFACE)
        {
            GraphicsColumn* column = static_cast<GraphicsColumn*>(adhocPort_->parentItem());
            if (column)
            {
                column->onMoveItem(adhocPort_);
            }
        }
        else if (adhocPort_->type() == GFX_TYPE_DIAGRAM_ADHOC_PORT)
        {
            HWComponentItem* componentItem = static_cast<HWComponentItem*>(adhocPort_->parentItem());
            componentItem->onMovePort(adhocPort_);
        }
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    dataSource_->changeAdhocVisibility(adhocPort_, newVisibility_);

    changePortItemVisibility(newVisibility_);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::changePortItemVisibility()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::changePortItemVisibility(bool currentVisibility)
{
    if (currentVisibility)
    {
        adhocPort_->show();
    }
    else
    {
        adhocPort_->hide();
    }
}
