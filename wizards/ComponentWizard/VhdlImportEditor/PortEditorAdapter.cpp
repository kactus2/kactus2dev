//-----------------------------------------------------------------------------
// File: PortEditorAdapter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.09.2014
//
// Description:
// Adapter for PortsEditor to implement PortsVisualizer. 
//-----------------------------------------------------------------------------

#include "PortEditorAdapter.h"

#include <IPXACTmodels/port.h>

#include <editors/ComponentEditor/ports/portseditor.h>

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::PortEditorAdapter()
//-----------------------------------------------------------------------------
PortEditorAdapter::PortEditorAdapter(PortsEditor* editor) : editor_(editor)
{

}
//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::~PortEditorAdapter()
//-----------------------------------------------------------------------------
PortEditorAdapter::~PortEditorAdapter()
{

}

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::addPort()
//-----------------------------------------------------------------------------
void PortEditorAdapter::addPort(QSharedPointer<Port> portToAdd)
{
    editor_->addPort(portToAdd);
}

//-----------------------------------------------------------------------------
// Function: PortEditorAdapter::removePort()
//-----------------------------------------------------------------------------
void PortEditorAdapter::removePort(QSharedPointer<Port> portToRemove)
{
     editor_->removePort(portToRemove);
}
