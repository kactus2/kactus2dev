//-----------------------------------------------------------------------------
// File: HWChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#include "HWChangeCommands.h"
#include "HWMoveCommands.h"
#include "HWAddCommands.h"
#include "HWConnection.h"
#include "HWComponentItem.h"
#include "columnview/HWColumn.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/common/DesignDiagram.h>
#include <editors/common/DesignWidget.h>
#include <editors/common/Association/AssociationChangeEndpointCommand.h>
#include <editors/common/ConfigurationEditor/activeviewmodel.h>

#include <editors/HWDesign/HierarchicalBusInterfaceItem.h>
#include <editors/HWDesign/ActiveBusInterfaceItem.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/AdHocConnectionItem.h>
#include <editors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <editors/HWDesign/undoCommands/ComponentDeleteCommand.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/PortReference.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::ComponentChangeNameCommand(ComponentItem* component, QString const& newName,
    QSharedPointer<Design> design, QUndoCommand* parent):
QUndoCommand(parent),
    component_(component),
    oldName_(component->name()),
    newName_(newName),
    containingDesign_(design)
{
    Q_ASSERT(design);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::~ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::~ComponentChangeNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::undo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::undo()
{
    renameInstanceAndConnections(newName_, oldName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::redo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::redo()
{
    renameInstanceAndConnections(oldName_, newName_);
}

//-----------------------------------------------------------------------------
// Function: HWChangeCommands::renameInstanceAndConnections()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::renameInstanceAndConnections(QString const& previousName,
    QString const& newName)
{
    QList<GraphicsConnection*> allConnections;

    // Update component reference in interconnections and ad-hoc connections.
    for (ConnectionEndpoint* endpoint : component_->getEndpoints())
    {
        allConnections.append(endpoint->getConnections()); 
        allConnections.append(endpoint->getOffPageConnector()->getConnections());
    }

    for (GraphicsConnection* connection : allConnections)
    {
         connection->changeConnectionComponentReference(previousName, newName);
    }

    // Find all connections, including ad-hoc, that are using the default naming and should be renamed.
    QList<GraphicsConnection*> renamedConnections;
    for (GraphicsConnection* connection : allConnections)
    {
        if (connection->hasDefaultName())
        {
            renamedConnections.append(connection);
        }
    }

    // Rename component instance.
    component_->setName(newName);

    // Instance must be renamed before renaming the connections with automatic naming.
    for (GraphicsConnection* connection : renamedConnections)
    {
        connection->setName(connection->createDefaultName());
    }

    // Rename ad-hoc connections defining tie-offs for the component instance.
    for (auto const& adhocConnection : *containingDesign_->getAdHocConnections())
    {
        if (!adhocConnection->getTiedValue().isEmpty())
        {
            for (auto const& internalReference : *adhocConnection->getInternalPortReferences())
            {
                if (internalReference->getComponentRef().compare(previousName) == 0)
                {
                    internalReference->setComponentRef(newName);
                    changeAdHocConnectionDefaultName(adhocConnection, internalReference->getPortRef(),
                        previousName, newName);
                }
            }           
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWChangeCommands::changeAdHocConnectionDefaultName()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::changeAdHocConnectionDefaultName(QSharedPointer<AdHocConnection> connection,
    QString const& portReference, QString const& oldReference, QString const& newReference)
{
    QString defaultName = oldReference + "_" + portReference + "_to_tiedValue";
    if (connection->name().compare(defaultName) == 0)
    {
        QString newName = newReference + "_" + portReference + "_to_tiedValue";
        connection->setName(newName);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDisplayNameCommand::ComponentChangeDisplayNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeDisplayNameCommand::ComponentChangeDisplayNameCommand(ComponentItem* component,
    QString const& newDisplayName,
    QUndoCommand* parent):
QUndoCommand(parent),
    component_(component),
    oldDisplayName_(component->displayName()),
    newDisplayName_(newDisplayName)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDisplayNameCommand::~ComponentChangeDisplayNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeDisplayNameCommand::~ComponentChangeDisplayNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDisplayNameCommand::undo()
//-----------------------------------------------------------------------------
void ComponentChangeDisplayNameCommand::undo()
{
	component_->setDisplayName(oldDisplayName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDisplayNameCommand::redo()
//-----------------------------------------------------------------------------
void ComponentChangeDisplayNameCommand::redo()
{
	component_->setDisplayName(newDisplayName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDescriptionNameCommand::ComponentChangeDescriptionNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeDescriptionNameCommand::ComponentChangeDescriptionNameCommand(ComponentItem* component,
    QString const& newDescription, 
    QUndoCommand* parent):
QUndoCommand(parent),
    component_(component),
    oldDescription_(component->description()),
    newDescription_(newDescription)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDescriptionNameCommand::~ComponentChangeDescriptionNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeDescriptionNameCommand::~ComponentChangeDescriptionNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDescriptionNameCommand::undo()
//-----------------------------------------------------------------------------
void ComponentChangeDescriptionNameCommand::undo()
{
	component_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeDescriptionNameCommand::redo()
//-----------------------------------------------------------------------------
void ComponentChangeDescriptionNameCommand::redo()
{
	component_->setDescription(newDescription_);
}

//-----------------------------------------------------------------------------
// Function: ComponentActiveViewChangeCommand::ComponentActiveViewChangeCommand()
//-----------------------------------------------------------------------------
ComponentActiveViewChangeCommand::ComponentActiveViewChangeCommand(QString const& instanceName, 
    QString const& oldActiveView, QString const& newActiveView, ActiveViewModel* activeViewModel,
    QUndoCommand* parent):
QUndoCommand(parent),
instanceName_(instanceName),
newViewName_(newActiveView),
oldViewName_(oldActiveView),
activeViewModel_(activeViewModel)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentActiveViewChangeCommand::~ComponentActiveViewChangeCommand()
//-----------------------------------------------------------------------------
ComponentActiveViewChangeCommand::~ComponentActiveViewChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentActiveViewChangeCommand::undo()
//-----------------------------------------------------------------------------
void ComponentActiveViewChangeCommand::undo()
{
	activeViewModel_->setActiveView(instanceName_, oldViewName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentActiveViewChangeCommand::redo()
//-----------------------------------------------------------------------------
void ComponentActiveViewChangeCommand::redo()
{
	activeViewModel_->setActiveView(instanceName_, newViewName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand::ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::ComponentPacketizeCommand(DesignDiagram* diagram,
    ComponentItem* component,
    VLNV const& vlnv,
    QUndoCommand* parent) : QUndoCommand(parent),
    diagram_(diagram),
    componentItem_(component),
    vlnv_(vlnv)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand::~ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::~ComponentPacketizeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand::undo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::undo()
{
    QUndoCommand::undo();

    diagram_->getParent()->removeRelatedVLNV(vlnv_);

    componentItem_->setDraft();
    componentItem_->getComponentInstance()->getComponentRef()->setVLNV(VLNV());
    componentItem_->componentModel()->setVlnv(VLNV());    

    componentItem_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand::redo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::redo()
{
    QUndoCommand::redo();

    componentItem_->componentModel()->setVlnv(vlnv_);
    componentItem_->getComponentInstance()->getComponentRef()->setVLNV(vlnv_);
    componentItem_->setPackaged();

    diagram_->getParent()->addRelatedVLNV(vlnv_);

    componentItem_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::EndpointChangeCommand(ConnectionEndpoint* endpoint, 
                                             General::InterfaceMode newMode,
											 QUndoCommand* parent):
QUndoCommand(parent), 
endpoint_(endpoint),
oldMode_(endpoint->getBusInterface()->getInterfaceMode()),
newMode_(newMode)
{
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::~EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::~EndpointChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::undo()
{
    QUndoCommand::undo();

    endpoint_->getBusInterface()->setInterfaceMode(oldMode_);
    endpoint_->revalidateConnections();
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::redo()
{
    QUndoCommand::redo();

    endpoint_->getBusInterface()->setInterfaceMode(newMode_);
    endpoint_->revalidateConnections();
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::EndpointNameChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::EndpointNameChangeCommand(ConnectionEndpoint* endpoint, 
    QString const& newName,
    QList<QSharedPointer<HierInterface> > activeIntefaces,
    QUndoCommand* parent) :
    QUndoCommand(parent),
    endpoint_(endpoint),
    oldName_(endpoint->name()),
    newName_(newName),
    activeIntefaces_(activeIntefaces)
{
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::~EndpointNameChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::~EndpointNameChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::undo()
{
    QUndoCommand::undo();

    endpoint_->setName(oldName_);
    for (auto const& interface : activeIntefaces_)
    {
        interface->setBusReference(oldName_);
    }
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::redo()
{
    QUndoCommand::redo();

    endpoint_->setName(newName_);
    for (auto const& interface : activeIntefaces_)
    {
        interface->setBusReference(newName_);
    }
}

//-----------------------------------------------------------------------------
// Function: EndpointDescChangeCommand::EndpointDescChangeCommand()
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
// Function: EndpointDescChangeCommand::~EndpointDescChangeCommand()
//-----------------------------------------------------------------------------
EndpointDescChangeCommand::~EndpointDescChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: EndpointDescChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::undo()
{
    endpoint_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: EndpointDescChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::redo()
{
    endpoint_->setDescription(newDescription_);
}

//-----------------------------------------------------------------------------
// Function: EndpointDependencyDirectionChangeCommand::EndpointDependencyDirectionChangeCommand()
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
// Function: EndpointDependencyDirectionChangeCommand::~EndpointDependencyDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointDependencyDirectionChangeCommand::~EndpointDependencyDirectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointDependencyDirectionChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointDependencyDirectionChangeCommand::undo()
{
    endpoint_->getApiInterface()->setDependencyDirection(oldDir_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointDependencyDirectionChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointDependencyDirectionChangeCommand::redo()
{
    endpoint_->getApiInterface()->setDependencyDirection(newDir_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointComDirectionChangeCommand::EndpointComDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointComDirectionChangeCommand::EndpointComDirectionChangeCommand(ConnectionEndpoint* endpoint, 
                                                                     DirectionTypes::Direction newDir,
                                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldDir_(endpoint->getComInterface()->getDirection()), 
      newDir_(newDir)
{
}

//-----------------------------------------------------------------------------
// Function: EndpointComDirectionChangeCommand::~EndpointComDirectionChangeCommand()
//-----------------------------------------------------------------------------
EndpointComDirectionChangeCommand::~EndpointComDirectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointComDirectionChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointComDirectionChangeCommand::undo()
{
    endpoint_->getComInterface()->setDirection(oldDir_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointComDirectionChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointComDirectionChangeCommand::redo()
{
    endpoint_->getComInterface()->setDirection(newDir_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointTransferTypeChangeCommand::EndpointTransferTypeChangeCommand()
//-----------------------------------------------------------------------------
EndpointTransferTypeChangeCommand::EndpointTransferTypeChangeCommand(ConnectionEndpoint* endpoint,
    QString const& newTransferType,
    QUndoCommand* parent):
QUndoCommand(parent), 
    endpoint_(endpoint),
    oldTransferType_(endpoint->getComInterface()->getTransferType()),
    newTransferType_(newTransferType)
{
}

//-----------------------------------------------------------------------------
// Function: EndpointTransferTypeChangeCommand::~EndpointTransferTypeChangeCommand()
//-----------------------------------------------------------------------------
EndpointTransferTypeChangeCommand::~EndpointTransferTypeChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointTransferTypeChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointTransferTypeChangeCommand::undo()
{
    endpoint_->getComInterface()->setTransferType(oldTransferType_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointTransferTypeChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointTransferTypeChangeCommand::redo()
{
    endpoint_->getComInterface()->setTransferType(newTransferType_);
    endpoint_->updateInterface();
    endpoint_->revalidateConnections();
}

//-----------------------------------------------------------------------------
// Function: EndpointPropertyValuesChangeCommand::EndpointPropertyValuesChangeCommand()
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
// Function: EndpointPropertyValuesChangeCommand::~EndpointPropertyValuesChangeCommand()
//-----------------------------------------------------------------------------
EndpointPropertyValuesChangeCommand::~EndpointPropertyValuesChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndpointPropertyValuesChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointPropertyValuesChangeCommand::undo()
{
    endpoint_->getComInterface()->setPropertyValues(oldValues_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointPropertyValuesChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointPropertyValuesChangeCommand::redo()
{
    endpoint_->getComInterface()->setPropertyValues(newValues_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::EndPointTypesCommand(ConnectionEndpoint* endpoint, VLNV const& busType, VLNV const& absType,
    QString const& activeView, QUndoCommand* parent /* = 0 */):
QUndoCommand(parent),
endpoint_(endpoint),
oldBusType_(),
oldAbsType_(),
newBusType_(busType),
newAbsType_(absType),
activeView_(activeView)
{
    if (endpoint_->getBusInterface() != 0)
    {
        oldBusType_ = endpoint_->getBusInterface()->getBusType();

        if (endpoint_->getBusInterface()->getAbstractionTypes())
        {
            QSharedPointer<AbstractionType> abstraction =
                endpoint_->getBusInterface()->getAbstractionContainingView(activeView);
            if (abstraction && abstraction->getAbstractionRef())
            {
                oldAbsType_ = *abstraction->getAbstractionRef().data();
            }
        }

    }
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::~EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::~EndPointTypesCommand()
{
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::undo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::undo()
{
    QUndoCommand::undo();

    setTypes(oldBusType_, oldAbsType_);

    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::redo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::redo()
{
    QUndoCommand::redo();

    setTypes(newBusType_, newAbsType_);

    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::setTypes()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::setTypes(VLNV const& busType, VLNV const& absType)
{
    // Disconnect the connections.
    if (endpoint_->getBusInterface()->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
    {
        for (GraphicsConnection* conn : endpoint_->getConnections())
        {
            if (conn->endpoint1() != endpoint_)
            {
                conn->endpoint1()->onDisconnect();
            }
            else
            {
                conn->endpoint2()->onDisconnect();
            }
        }
    }

    endpoint_->getBusInterface()->setBusType(busType);

    if (endpoint_->getBusInterface())
    {
        QSharedPointer<AbstractionType> abstraction =
            endpoint_->getBusInterface()->getAbstractionContainingView(activeView_);
        if (!abstraction)
        {
            QSharedPointer<AbstractionType> newAbstraction(new AbstractionType());
            endpoint_->getBusInterface()->getAbstractionTypes()->append(newAbstraction);
            abstraction = newAbstraction;
        }

        QSharedPointer<ConfigurableVLNVReference> newVLNVReference;
        if (abstraction && abstraction->getAbstractionRef())
        {
            newVLNVReference = abstraction->getAbstractionRef();
        }
        else
        {
            newVLNVReference = QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference());
            abstraction->setAbstractionRef(newVLNVReference);
        }

        newVLNVReference->setVLNV(absType);
    }

    endpoint_->setTypeLocked(busType.isValid());

    // Undefined end points of the connections can now be defined.
    if (busType.isValid())
    {
        for (GraphicsConnection* conn : endpoint_->getConnections())
        {
            if (conn->endpoint1() != endpoint_)
            {
                conn->endpoint1()->onConnect(endpoint_);
                conn->endpoint2()->onConnect(conn->endpoint1());
            }
            else
            {
                conn->endpoint2()->onConnect(endpoint_);
                conn->endpoint1()->onConnect(conn->endpoint2());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::EndPointPortMapCommand(ConnectionEndpoint* endpoint,
    QList< QSharedPointer<PortMap> > newPortMaps, QSharedPointer<Component> component,
    QUndoCommand* parent) :
QUndoCommand(parent),
endpoint_(endpoint),
abstraction_(),
oldPortMaps_(),
newPortMaps_(newPortMaps),
component_(component)
{
    QSharedPointer<BusInterface> endPointBus = endpoint->getBusInterface();
    if (endPointBus && endPointBus->getAbstractionTypes() && endPointBus->getAbstractionTypes()->size() > 0)
    {
        abstraction_ = endPointBus->getAbstractionTypes()->first();
        if (abstraction_->getPortMaps())
        {
            for (auto const& oldMap : *abstraction_->getPortMaps())
            {
                oldPortMaps_.append(oldMap);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::~EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::~EndPointPortMapCommand()
{

}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::undo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::undo()
{
    if (endpoint_->isBus())
    {
        component_->getBusInterfaces()->removeOne(endpoint_->getBusInterface());
        abstraction_->getPortMaps()->clear();
        for (auto const& oldMap : oldPortMaps_)
        {
            abstraction_->getPortMaps()->append(oldMap);
        }

        endpoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::redo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::redo()
{
    if (endpoint_->isBus())
    {
        if (!component_->getBusInterfaces()->contains(endpoint_->getBusInterface()))
        {
            component_->getBusInterfaces()->append(endpoint_->getBusInterface());
        }

        abstraction_->getPortMaps()->clear();
        for (auto const& newMap : newPortMaps_)
        {
            abstraction_->getPortMaps()->append(newMap);
        }

        endpoint_->updateInterface();
    }
}
