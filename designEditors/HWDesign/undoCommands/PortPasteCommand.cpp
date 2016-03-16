//-----------------------------------------------------------------------------
// File: PortPasteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.02.2016
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "PortPasteCommand.h"

#include <designEditors/HWDesign/BusPortItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWAddCommands.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::PortPasteCommand(HWComponentItem* destComponent, QSharedPointer<Component> srcComponent, 
    BusPortItem* port, QUndoCommand* parent) : 
QUndoCommand(parent),
    component_(destComponent), 
    port_(port), 
    scene_(destComponent->scene())
{
    // Create child commands for adding physical ports to target component. 
    // Physical ports must have a unique name within the component.
    foreach (QString const& portName, port_->getBusInterface()->getPhysicalPortNames())
    {	
        QString uniquePortName = portName;
        unsigned int trailingNumber = 0;
        while (component_->componentModel()->getPort(uniquePortName) != 0)
        {
            trailingNumber++;
            uniquePortName = portName + "_" + QString::number(trailingNumber);
        }

        // Create copies of the physical ports in the source component and rename them.
        if (srcComponent->hasPort(portName))
        {
            QSharedPointer<Port> physPortCopy = QSharedPointer<Port>(new Port(*srcComponent->getPort(portName)));
            physPortCopy->setName(uniquePortName);

            // If port name changed, it is also changed in bus interface.
            if(uniquePortName != portName)
            {
                foreach (QSharedPointer<PortMap> portMap, *port_->getBusInterface()->getPortMaps())
                {
                    if(portMap->getPhysicalPort()->name_ == portName)
                    {
                        portMap->getPhysicalPort()->name_ = uniquePortName;
                    }
                }
            }

            new AddPhysicalPortCommand(component_->componentModel(), physPortCopy, this);
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::~PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::~PortPasteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::undo()
//-----------------------------------------------------------------------------
void PortPasteCommand::undo()
{
    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);

    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::redo()
//-----------------------------------------------------------------------------
void PortPasteCommand::redo()
{
    QUndoCommand::redo();

    Q_ASSERT(port_ != 0);

    // Copy a port to the component.
    component_->addPort(port_);
    port_->updateInterface();

    del_ = false;
}
