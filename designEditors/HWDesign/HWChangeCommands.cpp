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
#include "HWMoveCommands.h"
#include "HWAddCommands.h"
#include "HWConnection.h"
#include "BusPortItem.h"
#include "AdHocPortItem.h"
#include "AdHocInterfaceItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"
#include "columnview/HWColumn.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/DesignWidget.h>
#include <designEditors/common/Association/AssociationChangeEndpointCommand.h>
#include <designEditors/common/ConfigurationEditor/activeviewmodel.h>

#include <designEditors/HWDesign/AdHocConnectionItem.h>
#include <designEditors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <designEditors/HWDesign/undoCommands/ComponentDeleteCommand.h>

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
// Function: ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::ComponentChangeNameCommand(ComponentItem* component, QString const& newName,
    QSharedPointer<Design> design, QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
oldName_(component->name()),
newName_(newName),
containingDesign_(design)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::~ComponentChangeNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::undo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::undo()
{
    component_->setName(oldName_);

    HWComponentItem* hwItem = dynamic_cast<HWComponentItem*>(component_);
    if (hwItem)
    {
        changeAdHocTieOffConnectionReferences(newName_, oldName_);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand::redo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::redo()
{
    component_->setName(newName_);

    HWComponentItem* hwItem = dynamic_cast<HWComponentItem*>(component_);
    if (hwItem)
    {
        changeAdHocTieOffConnectionReferences(oldName_, newName_);
    }
}

//-----------------------------------------------------------------------------
// Function: HWChangeCommands::changeAdHocTieOffConnectionReferences()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::changeAdHocTieOffConnectionReferences(QString const& oldReference,
    QString const& newReference)
{
    if (containingDesign_)
    {
        foreach (QSharedPointer<AdHocConnection> connection, *containingDesign_->getAdHocConnections())
        {
            if (!connection->getTiedValue().isEmpty())
            {
                foreach (QSharedPointer<PortReference> internalReference, *connection->getInternalPortReferences())
                {
                    if (internalReference->getComponentRef().compare(oldReference) == 0)
                    {
                        internalReference->setComponentRef(newReference);
                        changeAdHocConnectionDefaultName(connection, internalReference->getPortRef(), oldReference,
                            newReference);
                    }
                }
                foreach (QSharedPointer<PortReference> externalReference, *connection->getExternalPortReferences())
                {
                    if (externalReference->getComponentRef().compare(oldReference) == 0)
                    {
                        externalReference->setComponentRef(newReference);
                        changeAdHocConnectionDefaultName(connection, externalReference->getPortRef(), oldReference,
                            newReference);
                    }
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
    vlnv_(vlnv),
    endpointLockedStates_()
{
    // Save the locked states of each endpoint.
    foreach (QGraphicsItem* item, componentItem_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpointLockedStates_.insert(endpoint, endpoint->isTypeLocked());
        }
    }
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
    diagram_->getParent()->removeRelatedVLNV(vlnv_);

    componentItem_->setDraft();
    componentItem_->getComponentInstance()->getComponentRef()->setVLNV(VLNV());
    componentItem_->componentModel()->setVlnv(VLNV());    

    // Mark all endpoints as temporary.
    foreach (QGraphicsItem* item, componentItem_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpoint->setTemporary(true);
            endpoint->setTypeLocked(endpointLockedStates_.value(endpoint));
        }
    }

    componentItem_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand::redo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::redo()
{
    componentItem_->componentModel()->setVlnv(vlnv_);
    componentItem_->getComponentInstance()->getComponentRef()->setVLNV(vlnv_);
    componentItem_->setPackaged();

    diagram_->getParent()->addRelatedVLNV(vlnv_);

    // Mark all endpoints as non-temporary.
    foreach (QGraphicsItem* item, componentItem_->childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpoint->setTemporary(false);
            endpoint->setTypeLocked(true);
        }
    }

    componentItem_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::EndpointChangeCommand()
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
newDescription_(newDescription)
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
    endpoint_->getBusInterface()->setInterfaceMode(oldMode_);
    endpoint_->setDescription(oldDescription_);
    endpoint_->setName(oldName_);
    endpoint_->revalidateConnections();
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::redo()
{
    endpoint_->getBusInterface()->setInterfaceMode(newMode_);
    endpoint_->setDescription(newDescription_);
    endpoint_->setName(newName_);
    endpoint_->revalidateConnections();
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::EndpointNameChangeCommand()
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
// Function: EndpointNameChangeCommand::~EndpointNameChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::~EndpointNameChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::undo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::undo()
{
    endpoint_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: EndpointNameChangeCommand::redo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::redo()
{
    endpoint_->setName(newName_);
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
        newAbsType_ = *endpoint_->getBusInterface()->getAbstractionTypes()->first()->getAbstractionRef();
        newMode_ = endpoint_->getBusInterface()->getInterfaceMode();
        newName_ = endpoint_->getBusInterface()->name();
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
    if (endpoint_->getBusInterface() != 0 && oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(oldName_);
    }

    endpoint_->setTypes(oldBusType_, oldAbsType_, oldMode_);
    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand::redo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::redo()
{
    if (oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(newName_);
    }

    endpoint_->setTypes(newBusType_, newAbsType_, newMode_);
    endpoint_->updateInterface();

    // Set interface modes for the other end points.
    QMap<ConnectionEndpoint*, General::InterfaceMode>::iterator cur = connModes_.begin();

    while (cur != connModes_.end())
    {
        cur.key()->getBusInterface()->setInterfaceMode(cur.value());
        cur.key()->updateInterface();
        cur++;
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::EndPointPortMapCommand(HWConnectionEndpoint* endpoint,
                                               QList< QSharedPointer<PortMap> > newPortMaps,
                                               QUndoCommand* parent)
    : QUndoCommand(parent), endpoint_(endpoint),
      oldPortMaps_(*endpoint->getBusInterface()->getPortMaps()),
      newPortMaps_(newPortMaps)
{
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
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->getPortMaps()->clear();
        endpoint_->getBusInterface()->getPortMaps()->append(oldPortMaps_);
        endpoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand::redo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::redo()
{
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->getPortMaps()->clear();
        endpoint_->getBusInterface()->getPortMaps()->append(newPortMaps_);
        endpoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentConfElementChangeCommand::ComponentConfElementChangeCommand()
//-----------------------------------------------------------------------------
ComponentConfElementChangeCommand::ComponentConfElementChangeCommand(
    QSharedPointer<ComponentInstance> componentInstance, const QMap<QString, QString>& newConfElements,
    QUndoCommand* parent /* = 0 */):
QUndoCommand(parent),
componentInstance_(componentInstance),
oldConfElements_(),
newConfElements_(newConfElements)
{
    foreach (QSharedPointer<ConfigurableElementValue> element, *componentInstance_->getConfigurableElementValues())
    {
        oldConfElements_.insert(element->getReferenceId(), element->getConfigurableValue());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentConfElementChangeCommand::~ComponentConfElementChangeCommand()
//-----------------------------------------------------------------------------
ComponentConfElementChangeCommand::~ComponentConfElementChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentConfElementChangeCommand::undo()
//-----------------------------------------------------------------------------
void ComponentConfElementChangeCommand::undo()
{
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > currentConfigurables =
        componentInstance_->getConfigurableElementValues();
    QStringList configurableIDs;
    foreach (QSharedPointer<ConfigurableElementValue> element, *currentConfigurables)
    {
        if (oldConfElements_.contains(element->getReferenceId()))
        {
            element->setConfigurableValue(oldConfElements_.value(element->getReferenceId()));
            configurableIDs.append(element->getReferenceId());
        }
        else
        {
            currentConfigurables->removeAll(element);
        }
    }

    if (oldConfElements_.size() > currentConfigurables->size())
    {
        QMapIterator<QString, QString> elementIterator(oldConfElements_);
        while (elementIterator.hasNext())
        {
            elementIterator.next();

            if (!configurableIDs.contains(elementIterator.key()))
            {
                QSharedPointer<ConfigurableElementValue> newElement (
                    new ConfigurableElementValue(elementIterator.value(), elementIterator.key()));
                componentInstance_->getConfigurableElementValues()->append(newElement);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentConfElementChangeCommand::redo()
//-----------------------------------------------------------------------------
void ComponentConfElementChangeCommand::redo()
{
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > currentConfigurableElements =
        componentInstance_->getConfigurableElementValues();
    if (newConfElements_.size() < currentConfigurableElements->size())
    {
        foreach (QSharedPointer<ConfigurableElementValue> element, *currentConfigurableElements)
        {
            if (!newConfElements_.contains(element->getReferenceId()))
            {
                componentInstance_->getConfigurableElementValues()->removeAll(element);
            }
        }
    }

    QMapIterator<QString, QString> elementIterator(newConfElements_);
    while (elementIterator.hasNext())
    {
        elementIterator.next();
        bool elementExists = false;
        foreach (QSharedPointer<ConfigurableElementValue> element, *currentConfigurableElements)
        {
            if (elementIterator.key() == element->getReferenceId())
            {
                element->setConfigurableValue(elementIterator.value());
                elementExists = true;
                break;
            }
        }

        if (!elementExists)
        {
            QSharedPointer<ConfigurableElementValue> newElement (
                new ConfigurableElementValue(elementIterator.value(), elementIterator.key()));
            componentInstance_->getConfigurableElementValues()->append(newElement);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::AdHocBoundsChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::AdHocBoundsChangeCommand(AdHocConnectionItem* connection,
                                                   bool right, int endpointIndex,
                                                   QString const& oldValue, QString const& newValue, 
                                                   QUndoCommand* parent) : QUndoCommand(parent),
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
