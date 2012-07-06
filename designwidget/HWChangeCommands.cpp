//-----------------------------------------------------------------------------
// File: HWChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#include "HWChangeCommands.h"

#include "HWDeleteCommands.h"
#include "HWMoveCommands.h"
#include "HWAddCommands.h"

#include <models/component.h>
#include <models/ComInterface.h>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>

#include <ConfigurationEditor/activeviewmodel.h>

#include "HWConnection.h"
#include "BusPortItem.h"
#include "AdHocPortItem.h"
#include "AdHocInterfaceItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"
#include "columnview/HWColumn.h"

#include <models/businterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::ComponentChangeNameCommand(ComponentItem* component,
                                               QString const& newName,
                                               QUndoCommand* parent) : QUndoCommand(parent),
                                                                       component_(component),
                                                                       oldName_(component->name()),
                                                                       newName_(newName)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::~ComponentChangeNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::undo()
{
    component_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::redo()
{
    component_->setName(newName_);
}

ComponentChangeDisplayNameCommand::ComponentChangeDisplayNameCommand(ComponentItem* component,
													   QString const& newDisplayName,
													   QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
oldDisplayName_(component->displayName()),
newDisplayName_(newDisplayName) {
}

ComponentChangeDisplayNameCommand::~ComponentChangeDisplayNameCommand() {
}

void ComponentChangeDisplayNameCommand::undo() {
	component_->setDisplayName(oldDisplayName_);
}

void ComponentChangeDisplayNameCommand::redo() {
	component_->setDisplayName(newDisplayName_);
}

ComponentChangeDescriptionNameCommand::ComponentChangeDescriptionNameCommand(ComponentItem* component,
																			 QString const& newDescription, 
																			 QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldDescription_(component->description()),
newDescription_(newDescription) {
}

ComponentChangeDescriptionNameCommand::~ComponentChangeDescriptionNameCommand() {
}

void ComponentChangeDescriptionNameCommand::undo() {
	component_->setDescription(oldDescription_);
}

void ComponentChangeDescriptionNameCommand::redo() {
	component_->setDescription(newDescription_);
}

ComponentActiveViewChangeCommand::ComponentActiveViewChangeCommand( 
	const QString& instanceName, 
	QString const& oldActiveView, 
	QString const& newActiveView,
	ActiveViewModel* activeViewModel,
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
instanceName_(instanceName),
newViewName_(newActiveView),
oldViewName_(oldActiveView),
activeViewModel_(activeViewModel) {
}

ComponentActiveViewChangeCommand::~ComponentActiveViewChangeCommand() {

}

void ComponentActiveViewChangeCommand::undo() {
	activeViewModel_->setActiveView(instanceName_, oldViewName_);
}

void ComponentActiveViewChangeCommand::redo() {
	activeViewModel_->setActiveView(instanceName_, newViewName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::ComponentPacketizeCommand(ComponentItem* component,
                                                     VLNV const& vlnv,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                     component_(component),
                                                     vlnv_(vlnv),
                                                     endpointLockedStates_()
{
    // Save the locked states of each endpoint.
    foreach (QGraphicsItem* item, component_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpointLockedStates_.insert(endpoint, endpoint->isTypeLocked());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::~ComponentPacketizeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::undo()
{
    // Set an empty VLNV.
    component_->componentModel()->setVlnv(VLNV());

    // Mark all endpoints as temporary.
    foreach (QGraphicsItem* item, component_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpoint->setTemporary(true);
            endpoint->setTypeLocked(endpointLockedStates_.value(endpoint));
        }
    }

    component_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::redo()
{
    component_->componentModel()->setVlnv(vlnv_);

    // Mark all endpoints as non-temporary.
    foreach (QGraphicsItem* item, component_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpoint->setTemporary(false);
            endpoint->setTypeLocked(true);
        }
    }

    component_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::EndpointChangeCommand(HWConnectionEndpoint* endpoint, 
											 QString const& newName,
                                             General::InterfaceMode newMode,
											 QString const& newDescription,
											 QUndoCommand* parent):
QUndoCommand(parent), 
endpoint_(endpoint),
oldName_(endpoint->name()), 
oldMode_(endpoint->getBusInterface()->getInterfaceMode()),
oldDescription_(endpoint->description()),
newName_(newName),
newMode_(newMode),
newDescription_(newDescription) {
}

//-----------------------------------------------------------------------------
// Function: ~EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::~EndpointChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::undo() {
    endpoint_->getBusInterface()->setInterfaceMode(oldMode_);
    endpoint_->setDescription(oldDescription_);
    endpoint_->setName(oldName_);
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::redo() {
    endpoint_->getBusInterface()->setInterfaceMode(newMode_);
    endpoint_->setDescription(newDescription_);
    endpoint_->setName(newName_);
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::EndpointNameChangeCommand(ConnectionEndpoint* endpoint, 
                                                     QString const& newName,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
       endpoint_(endpoint),
       oldName_(endpoint->name()), 
       newName_(newName)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::~EndpointNameChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::undo()
{
    endpoint_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::redo()
{
    endpoint_->setName(newName_);
}

//-----------------------------------------------------------------------------
// Function: EndpointDescChangeCommand()
//-----------------------------------------------------------------------------
EndpointDescChangeCommand::EndpointDescChangeCommand(ConnectionEndpoint* endpoint, 
                                                     QString const& newDescription,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldDescription_(endpoint->description()), 
      newDescription_(newDescription)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointDescChangeCommand()
//-----------------------------------------------------------------------------
EndpointDescChangeCommand::~EndpointDescChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::undo()
{
    endpoint_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::redo()
{
    endpoint_->setDescription(newDescription_);
}


//-----------------------------------------------------------------------------
// Function: EndpointDependencyDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointDependencyDirectionChangeCommand::EndpointDependencyDirectionChangeCommand(ConnectionEndpoint* endpoint, 
                                                                                   DependencyDirection newDir,
                                                                                   QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldDir_(endpoint->getApiInterface()->getDependencyDirection()), 
      newDir_(newDir)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointDependencyDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointDependencyDirectionChangeCommand::~EndpointDependencyDirectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointDependencyDirectionChangeCommand::undo()
{
    endpoint_->getApiInterface()->setDependencyDirection(oldDir_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointDependencyDirectionChangeCommand::redo()
{
    endpoint_->getApiInterface()->setDependencyDirection(newDir_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointComDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointComDirectionChangeCommand::EndpointComDirectionChangeCommand(ConnectionEndpoint* endpoint, 
                                                                     General::Direction newDir,
                                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldDir_(endpoint->getComInterface()->getDirection()), 
      newDir_(newDir)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointComDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointComDirectionChangeCommand::~EndpointComDirectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointComDirectionChangeCommand::undo()
{
    endpoint_->getComInterface()->setDirection(oldDir_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointComDirectionChangeCommand::redo()
{
    endpoint_->getComInterface()->setDirection(newDir_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointTransferTypeChangeCommand()
//-----------------------------------------------------------------------------
EndpointTransferTypeChangeCommand::EndpointTransferTypeChangeCommand(ConnectionEndpoint* endpoint,
                                                                     QString const& newTransferType,
                                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldTransferType_(endpoint->getComInterface()->getTransferType()),
      newTransferType_(newTransferType)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointTransferTypeChangeCommand()
//-----------------------------------------------------------------------------
EndpointTransferTypeChangeCommand::~EndpointTransferTypeChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointTransferTypeChangeCommand::undo()
{
    endpoint_->getComInterface()->setTransferType(oldTransferType_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointTransferTypeChangeCommand::redo()
{
    endpoint_->getComInterface()->setTransferType(newTransferType_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointPropertyValuesChangeCommand()
//-----------------------------------------------------------------------------
EndpointPropertyValuesChangeCommand::EndpointPropertyValuesChangeCommand(ConnectionEndpoint* endpoint,
                                                                         QMap<QString, QString> const & newValues,
                                                                         QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldValues_(endpoint->getComInterface()->getPropertyValues()),
      newValues_(newValues)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointPropertyValuesChangeCommand()
//-----------------------------------------------------------------------------
EndpointPropertyValuesChangeCommand::~EndpointPropertyValuesChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointPropertyValuesChangeCommand::undo()
{
    endpoint_->getComInterface()->setPropertyValues(oldValues_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointPropertyValuesChangeCommand::redo()
{
    endpoint_->getComInterface()->setPropertyValues(newValues_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::EndPointTypesCommand(HWConnectionEndpoint* endpoint,
                                           VLNV const& oldBusType, VLNV const& oldAbsType,
                                           General::InterfaceMode oldMode,
                                           QString const& oldName, QUndoCommand* parent)
    : QUndoCommand(parent), endpoint_(endpoint), oldBusType_(oldBusType), oldAbsType_(oldAbsType),
      oldMode_(oldMode), oldName_(oldName), newBusType_(), newAbsType_(),
      newMode_(General::MASTER), newName_(""), connModes_()
{
    if (endpoint_->getBusInterface() != 0)
    {
        newBusType_ = endpoint_->getBusInterface()->getBusType();
        newAbsType_ = endpoint_->getBusInterface()->getAbstractionType();
        newMode_ = endpoint_->getBusInterface()->getInterfaceMode();
        newName_ = endpoint_->getBusInterface()->getName();
    }

    // Save the interface modes for each connection.
    foreach (GraphicsConnection* conn, endpoint_->getConnections())
    {
        ConnectionEndpoint* endpoint = conn->endpoint1();

        if (conn->endpoint1() == endpoint_)
        {
            endpoint = conn->endpoint2();
        }
        
        QSharedPointer<BusInterface> busIf = endpoint->getBusInterface();

        if (busIf != 0 && busIf->getBusType().isValid())
        {
            connModes_.insert(endpoint, busIf->getInterfaceMode());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::~EndPointTypesCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::undo()
{
    endpoint_->setTypes(oldBusType_, oldAbsType_, oldMode_);

    if (endpoint_->getBusInterface() != 0 && oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(oldName_);
    }

    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::redo()
{
    endpoint_->setTypes(newBusType_, newAbsType_, newMode_);

    if (oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(newName_);
    }

    endpoint_->updateInterface();

    // Set interface modes for the other end points.
    QMap<ConnectionEndpoint*, General::InterfaceMode>::iterator cur = connModes_.begin();

    while (cur != connModes_.end())
    {
        cur.key()->getBusInterface()->setInterfaceMode(cur.value());
        cur.key()->updateInterface();
        ++cur;
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::EndPointPortMapCommand(HWConnectionEndpoint* endpoint,
                                               QList< QSharedPointer<General::PortMap> > newPortMaps,
                                               QUndoCommand* parent)
    : QUndoCommand(parent), endpoint_(endpoint),
      oldPortMaps_(endpoint->getBusInterface()->getPortMaps()),
      newPortMaps_(newPortMaps)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::~EndPointPortMapCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::undo()
{
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->setPortMaps(oldPortMaps_);
        endpoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::redo()
{
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->setPortMaps(newPortMaps_);
        endpoint_->updateInterface();
    }
}

ComponentConfElementChangeCommand::ComponentConfElementChangeCommand( 
	ComponentItem* component, 
	const QMap<QString, QString>& newConfElements, 
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldConfElements_(component->getConfigurableElements()),
newConfElements_(newConfElements) {
}

ComponentConfElementChangeCommand::~ComponentConfElementChangeCommand() {
}

void ComponentConfElementChangeCommand::undo() {
	component_->setConfigurableElements(oldConfElements_);
}

void ComponentConfElementChangeCommand::redo() {
	component_->setConfigurableElements(newConfElements_);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName,
                                                           bool newVisibility, QUndoCommand* parent)
    : QUndoCommand(parent),
      dataSource_(dataSource),
      portName_(portName),
      pos_(),
      newVisibility_(newVisibility)
{
    if (!newVisibility_)
    {
        // Create child commands for removing interconnections.
        HWConnectionEndpoint* port = dataSource->getDiagramAdHocPort(portName);
        Q_ASSERT(port != 0);

        pos_ = port->scenePos();

        foreach (GraphicsConnection* conn, port->getConnections())
        {
            QUndoCommand* cmd =
                new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
        }

        foreach (GraphicsConnection* conn, port->getOffPageConnector()->getConnections())
        {
            QUndoCommand* cmd =
                new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
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
    dataSource_->setPortAdHocVisible(portName_, !newVisibility_);

    if (!newVisibility_)
    {
        HWConnectionEndpoint* port = dataSource_->getDiagramAdHocPort(portName_);
        port->setPos(port->parentItem()->mapFromScene(pos_));

        // 
        AdHocInterfaceItem* adHocIf = dynamic_cast<AdHocInterfaceItem*>(port);

        if (adHocIf != 0)
        {
            GraphicsColumn* column = static_cast<GraphicsColumn*>(adHocIf->parentItem());
            column->onMoveItem(adHocIf);
        }

        AdHocPortItem* adHocPort = dynamic_cast<AdHocPortItem*>(port);

        if (adHocPort != 0)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(adHocPort->parentItem());
            comp->onMovePort(adHocPort);
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

    dataSource_->setPortAdHocVisible(portName_, newVisibility_);
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::AdHocBoundsChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::AdHocBoundsChangeCommand(HWConnection* connection,
                                                   bool right, int endpointIndex,
                                                   int oldValue, int newValue, QUndoCommand* parent)
    : QUndoCommand(parent),
      connection_(connection),
      right_(right),
      endpointIndex_(endpointIndex),
      oldValue_(oldValue),
      newValue_(newValue)
{
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::~AdHocBoundChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::~AdHocBoundsChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::undo()
{
    if (right_)
    {
        connection_->setAdHocRightBound(endpointIndex_, oldValue_);
    }
    else
    {
        connection_->setAdHocLeftBound(endpointIndex_, oldValue_);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::redo()
{
    if (right_)
    {
        connection_->setAdHocRightBound(endpointIndex_, newValue_);
    }
    else
    {
        connection_->setAdHocLeftBound(endpointIndex_, newValue_);
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::ReplaceComponentCommand()
//-----------------------------------------------------------------------------
ReplaceComponentCommand::ReplaceComponentCommand(HWComponentItem* oldComp, HWComponentItem* newComp,
                                                 bool existing, bool keepOld, QUndoCommand* parent)
    : QUndoCommand(parent),
      oldComp_(oldComp),
      newComp_(newComp),
      existing_(existing)
{
    foreach (ConnectionEndpoint* oldEndpoint, oldComp_->getEndpoints())
    {
        HWConnectionEndpoint* newEndpoint = 0;

        if (oldEndpoint->type() == BusPortItem::Type)
        {
            newEndpoint = newComp_->getBusPort(oldEndpoint->name());
        }
        else if (oldEndpoint->type() == AdHocPortItem::Type)
        {
            newEndpoint = newComp_->getAdHocPort(oldEndpoint->name());
        }

        if (newEndpoint != 0)
        {
            // Create a move command to move the port to the same place where it is in the old component.
            QUndoCommand* moveCmd = new PortMoveCommand(newEndpoint, newEndpoint->pos(), oldEndpoint->pos(), this);

            // Exchange connections between the endpoints.
            foreach (GraphicsConnection* conn, oldEndpoint->getConnections())
            {
                QUndoCommand* exchangeCmd = new ConnectionExchangeCommand(conn, oldEndpoint, newEndpoint, this);
            }

            foreach (GraphicsConnection* conn, oldEndpoint->getOffPageConnector()->getConnections())
            {
                QUndoCommand* exchangeCmd = new ConnectionExchangeCommand(conn, oldEndpoint->getOffPageConnector(),
                                                                          newEndpoint->getOffPageConnector(), this);
            }
        }
    }

    // Create a delete command for the old component if it should not be kept.
    if (!keepOld)
    {
        ComponentDeleteCommand* deleteCmd = new ComponentDeleteCommand(oldComp_, this);

        connect(deleteCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(deleteCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
    }

    // Create a move/add command for the new component.
    if (existing)
    {
        QUndoCommand* moveCmd = new ItemMoveCommand(newComp_, newComp_->scenePos(), newComp_->getParentStack(),
                                                    oldComp_->scenePos(), oldComp_->getParentStack(), this);
    }
    else
    {
        ItemAddCommand* addCmd = new ItemAddCommand(oldComp_->getParentStack(), newComp_, this);

        connect(addCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::~ReplaceComponentCommand()
//-----------------------------------------------------------------------------
ReplaceComponentCommand::~ReplaceComponentCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::undo()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::redo()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::redo()
{
    // Place the new component to the exact same location as the old one.
    newComp_->setPos(oldComp_->scenePos());

    // Execute child commands.
    QUndoCommand::redo();
}
