//-----------------------------------------------------------------------------
// File: MappingComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.9.2011
//
// Description:
// Graphics item for the SW mapping component.
//-----------------------------------------------------------------------------

#include "MappingComponentItem.h"

#include "PlatformComponentItem.h"
#include "PlatformPlaceholderItem.h"
#include "ApplicationItem.h"
#include "EndpointDesignDiagram.h"
#include "EndpointConnection.h"
#include "SystemMoveCommands.h"

#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>
#include <models/businterface.h>
#include <models/cpu.h>

#include <LibraryManager/libraryinterface.h>

#include <QGraphicsScene>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: MappingComponentItem()
//-----------------------------------------------------------------------------
MappingComponentItem::MappingComponentItem(EndpointDesignDiagram* diagram,
                                           LibraryInterface* libInterface,
                                           QSharedPointer<Component> component,
                                           QString const& instanceName,
                                           QString const& displayName,
                                           QString const& description,
                                           QMap<QString, QString> const& configurableElementValues,
                                           unsigned int id) : SWComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, 0),
                                                                              component, instanceName,
                                                                              displayName, description,
                                                                              configurableElementValues, 0),
                                                              portIDFactory_(), diagram_(diagram),
                                                              id_(id), oldColumn_(0), progEntitys_(),
                                                              platformPlaceholder_(0), platformCompItem_(0),
                                                              oldPos_(), conns_(), importedIcon_(0)
{
    if (diagram_ != 0)
    {
        diagram_->addItem(this);
    }

    // Set basic graphics properties.
    setFlag(ItemIsMovable);
    setBrush(QBrush(QColor(0xa5,0xc3,0xef)));

    // Create the platform component placeholder.
    platformPlaceholder_ = new PlatformPlaceholderItem(this);

    // Parse the component's design if the component is valid.
    if (component->getVlnv()->isValid())
    {
        VLNV designVLNV = component->getHierRef("kts_sys_ref");

        if (!designVLNV.isValid())
        {
            designVLNV = component->getHierRef("kts_sw_ref");

            // Add CPU to the component so that it can be previewed correctly.
            QList<QSharedPointer<Cpu> > cpus;
            cpus.append(QSharedPointer<Cpu>(new Cpu()));
            component->setCpus(cpus);
        }

        QSharedPointer<LibraryComponent> libDesComp = libInterface->getModel(designVLNV);
        QSharedPointer<Design> design = libDesComp.staticCast<Design>();
        QMap<QString, ProgramEntityItem*> progEntityMap;
        
        foreach (Design::ComponentInstance const& instance, design->getComponentInstances())
        {
            // Retrieve the component model.
            QSharedPointer<LibraryComponent> libComp = libInterface->getModel(instance.componentRef);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            if (comp == 0)
            {
//                 emit errorMessage(tr("The component %1 was not found in the library").arg(
//                                   instance.componentRef.name()).arg(design->getVlnv()->name()));
                continue;
            }

            // Create the correct graphics item based on the SW type.
            switch (comp->getComponentSWType())
            {
            case KactusAttribute::KTS_SW_ENDPOINTS:
                {
                    ProgramEntityItem* progEntity = new ProgramEntityItem(comp, instance.instanceName,
                                                                          instance.displayName,
                                                                          instance.description,
                                                                          instance.configurableElementValues);
                    progEntity->setImported(instance.imported);

                    if (diagram_ != 0)
                    {
                        diagram_->onComponentInstanceAdded(progEntity);
                        progEntity->setPos(instance.position);
                        progEntity->setEndpointsExpanded(instance.endpointsExpanded);

                        // Set custom endpoint positions.
                        progEntity->setEndpointOrder(instance.portPositions);
                    }

                    connect(progEntity, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

                    addProgramEntity(progEntity);
                    progEntityMap.insert(instance.instanceName, progEntity);
                    break;
                }

            case KactusAttribute::KTS_SW_APPLICATION:
                {
                    // Discard any applications if the mapping component is not a CPU.
                    if (!componentModel()->isCpu())
                    {
                        continue;
                    }

                    // Find the corresponding interconnection to find the linked program entity.
                    int connIndex = 0;

                    for (; connIndex < design->getInterconnections().size(); ++connIndex)
                    {
                        if (design->getInterconnections().at(connIndex).interface2.componentRef ==
                            instance.instanceName)
                        {
                            break;
                        }
                    }

                    if (connIndex == design->getInterconnections().size())
                    {
                        // TODO: Error message to the output window.
                        continue;
                    }

                    Design::Interconnection const& conn = design->getInterconnections().at(connIndex);
                    Q_ASSERT(conn.interface2.componentRef == instance.instanceName);

                    ProgramEntityItem* progEntity = progEntityMap.value(conn.interface1.componentRef);
                    Q_ASSERT(progEntity != 0);
                    
                    ApplicationItem* appItem = new ApplicationItem(comp, instance.instanceName,
                                                                   instance.displayName,
                                                                   instance.description,
                                                                   instance.configurableElementValues,
                                                                   progEntity);
                    appItem->setImported(instance.imported);

                    if (diagram_ != 0)
                    {
                        diagram_->onComponentInstanceAdded(appItem);

                        connect(appItem, SIGNAL(openSource(ProgramEntityItem*)),
                            diagram_, SIGNAL(openSource(ProgramEntityItem*)), Qt::UniqueConnection);
                    }

                    connect(appItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
                    progEntity->setApplication(appItem);
                    break;
                }

            case KactusAttribute::KTS_SW_PLATFORM:
                {
                    PlatformComponentItem* platformCompItem =
                        new PlatformComponentItem(comp, instance.instanceName, instance.displayName,
                                                  instance.description, instance.configurableElementValues,
                                                  this);
                    platformCompItem->setImported(instance.imported);

                    if (diagram_ != 0)
                    {
                        diagram_->onComponentInstanceAdded(platformCompItem);
                    }

                    connect(platformCompItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
                    setPlatformComponent(platformCompItem);
                    break;
                }
            }
        }
    }
    
    // Determine the correct height for the node.
    updateSize();
    updateComponent();
    updateNameLabel(name());
}

//-----------------------------------------------------------------------------
// Function: ~MappingComponentItem()
//-----------------------------------------------------------------------------
MappingComponentItem::~MappingComponentItem()
{
    // Remove the program entities.
    foreach (ProgramEntityItem* item, progEntitys_)
    {
        delete item;
    }

    // Remove this item from the column where it resides.
    SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: setID()
//-----------------------------------------------------------------------------
void MappingComponentItem::setID(unsigned int id)
{
    id_ = id;
    updateNameLabel(name());
}

//-----------------------------------------------------------------------------
// Function: getID()
//-----------------------------------------------------------------------------
unsigned int MappingComponentItem::getID() const
{
    return id_;
}

//-----------------------------------------------------------------------------
// Function: getDiagram()
//-----------------------------------------------------------------------------
EndpointDesignDiagram* MappingComponentItem::getDiagram() const
{
    return diagram_;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void MappingComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<SystemColumn*>(parentItem());

    // Begin the position update for the connections.
    foreach (ProgramEntityItem* progEntity, progEntitys_)
    {
        foreach (EndpointItem* endpoint, progEntity->getEndpoints())
        {
            foreach (EndpointConnection* conn, endpoint->getConnections())
            {
                if (!conns_.contains(conn))
                {
                    conn->beginUpdatePosition();
                    conns_.insert(conn);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void MappingComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<EndpointDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    QGraphicsItem::mouseMoveEvent(event);

    if (oldColumn_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));
        
        SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this, oldColumn_);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void MappingComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new MappingCompMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the interconnections.
        foreach (EndpointConnection* conn, conns_)
        {
            conn->endUpdatePosition(cmd.data());
        }

        conns_.clear();

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onEndpointStackChange()
//-----------------------------------------------------------------------------
void MappingComponentItem::onEndpointStackChange(int height)
{
    setRect(-WIDTH / 2, 0, WIDTH, TOP_MARGIN + height + BOTTOM_MARGIN);

    SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());
    
    if (column != 0)
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: addProgramEntity()
//-----------------------------------------------------------------------------
void MappingComponentItem::addProgramEntity(ProgramEntityItem* item)
{
    item->setFixed(!componentModel()->isCpu());
    item->setParentItem(this);

    progEntitys_.append(item);
    VStackedLayout::updateItemMove(progEntitys_, item, TOP_MARGIN, SPACING);
    VStackedLayout::setItemPos(progEntitys_, item, 0.0, TOP_MARGIN, SPACING);
    updateSize();

    // Create the bus interfaces for the endpoints.
    foreach (EndpointItem* endpoint, item->getEndpoints())
    {
        endpoint->createBusInterface(this);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ()
//-----------------------------------------------------------------------------
void MappingComponentItem::updateSize()
{
    // Update the application item positions.
    VStackedLayout::updateItemPositions(progEntitys_, 0.0, TOP_MARGIN, SPACING);

    // Update the component's size based on the item that is positioned at
    // the lowest level of them all.
    qreal bottom = TOP_MARGIN;

    if (!progEntitys_.empty())
    {
        bottom = progEntitys_.back()->y() + progEntitys_.back()->boundingRect().bottom();
    }

    setRect(-WIDTH / 2, 0, WIDTH, bottom + BOTTOM_MARGIN);

    SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());

    if (column != 0)
    {
        column->updateItemPositions();
    }

    // Update the location of the platform component (and the placeholder).
    platformPlaceholder_->setPos(0.0, bottom + SPACING);

    if (platformCompItem_ != 0)
    {
        platformCompItem_->setPos(platformPlaceholder_->pos());
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void MappingComponentItem::onMoveItem(ProgramEntityItem* item)
{
    setZValue(1001.0);

    // Calculate the height of the application item stack.
    qreal stackHeight = progEntitys_.first()->boundingRect().height();

    for (int i = 1; i < progEntitys_.size(); ++i)
    {
        stackHeight += progEntitys_[i]->boundingRect().height() + SPACING;
    }

    // Restrict to horizontal center and clamp the vertical range.
    qreal y = std::min<qreal>(std::max<qreal>(TOP_MARGIN, item->y()),
                              TOP_MARGIN + stackHeight - item->boundingRect().height());
    item->setPos(snapPointToGrid(0, y));

    VStackedLayout::updateItemMove(progEntitys_, item, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void MappingComponentItem::onReleaseItem(ProgramEntityItem* item)
{
    setZValue(0.0);
    VStackedLayout::setItemPos(progEntitys_, item, 0, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: removeProgramEntity()
//-----------------------------------------------------------------------------
void MappingComponentItem::removeProgramEntity(ProgramEntityItem* item)
{
    // Remove the bus interfaces from the mapping component.
    foreach (EndpointItem* endpoint, item->getEndpoints())
    {
        endpoint->removeBusInterface(this);
    }

    progEntitys_.removeAll(item);
    item->setParentItem(0);
    updateSize();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: setPlatformComponent()
//-----------------------------------------------------------------------------
void MappingComponentItem::setPlatformComponent(PlatformComponentItem* item)
{
    platformCompItem_ = item;

    if (platformCompItem_ != 0)
    {
        platformCompItem_->setParentItem(this);
        platformCompItem_->setPos(platformPlaceholder_->pos());
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getPortIDFactory()
//-----------------------------------------------------------------------------
IDFactory& MappingComponentItem::getPortIDFactory()
{
    return portIDFactory_;
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool MappingComponentItem::save(LibraryInterface* libInterface)
{
    // Create the design.
    QSharedPointer<Design> design(new Design(componentModel()->getHierRef("kts_sys_ref")));

    QList<Design::ComponentInstance> instances;
    QList<Design::Interconnection> interconnections;

    // Add the platform component item to the instance list if set.
    if (platformCompItem_ != 0)
    {
        Design::ComponentInstance instance(platformCompItem_->name(), platformCompItem_->displayName(),
                                           platformCompItem_->description(),
                                           *platformCompItem_->componentModel()->getVlnv(),
                                           platformCompItem_->pos());
        instance.imported = platformCompItem_->isImported();
        instance.configurableElementValues = platformCompItem_->getConfigurableElements();

        instances.append(instance);
    }

    // Add the program entity items to the instance list.
    foreach (ProgramEntityItem* item, progEntitys_)
    {
        Design::ComponentInstance instance(item->name(), item->displayName(), item->description(),
                                           *item->componentModel()->getVlnv(), item->pos());
        instance.imported = item->isImported();
        instance.configurableElementValues = item->getConfigurableElements();
        instance.endpointsExpanded = item->isEndpointsExpanded();

        // Save endpoint positions (actually just the order of the endpoints).
        for (int i = 0; i < item->getEndpoints().size(); ++i)
        {
            instance.portPositions[item->getEndpoints().at(i)->getName()] = QPointF(0, i);
        }

        instances.append(instance);

        // Save the attached application.
        if (item->getApplication() != 0)
        {
            ApplicationItem* appItem = item->getApplication();

            Design::ComponentInstance appInstance(appItem->name(), appItem->displayName(),
                                                  appItem->description(), *appItem->componentModel()->getVlnv(),
                                                  appItem->pos());
            appInstance.imported = appItem->isImported();
            appInstance.configurableElementValues = appItem->getConfigurableElements();

            instances.append(appInstance);

            // Add the applink interconnection.
            Design::Interface iface1(item->name(), "app_link");
            Design::Interface iface2(appItem->name(), "app_link");
            interconnections.append(Design::Interconnection(item->name() + "_link_" + appItem->name(),
                                                            iface1, iface2, QList<QPointF>()));
        }
    }

    design->setComponentInstances(instances);
    design->setInterconnections(interconnections);

    // Save the design and the component.
    libInterface->writeModelToFile(design);
    libInterface->writeModelToFile(componentModel());
    return true;
}

//-----------------------------------------------------------------------------
// Function: getEndpoint()
//-----------------------------------------------------------------------------
EndpointItem* MappingComponentItem::getEndpoint(QString const& fullName)
{
    // Retrieve the program entity name from the full name (the part before _mcapiBus_).
    QStringList names = fullName.split("_mcapiBus_");

    if (names.size() != 2)
    {
        return 0;
    }

    foreach (ProgramEntityItem* item, progEntitys_)
    {
        if (item->name() == names.at(0))
        {
            return item->getEndpoint(names.at(1));
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: updateNameLabel()
//-----------------------------------------------------------------------------
void MappingComponentItem::updateNameLabel(QString const& text)
{
    ComponentItem::updateNameLabel(text + " (ID = " + QString::number(id_) + ")");
}

//-----------------------------------------------------------------------------
// Function: isMapped()
//-----------------------------------------------------------------------------
bool MappingComponentItem::isMapped() const
{
    return getConfigurableElements().contains("kts_hw_ref");
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void MappingComponentItem::updateComponent()
{
    SWComponentItem::updateComponent();

    // Show an icon if this is an imported SW component.
    if (isImported())
    {
        if (importedIcon_ == 0)
        {
            importedIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/imported.png"), this);
            importedIcon_->setToolTip(tr("Auto-synced"));
            importedIcon_->setPos(100, 6);
        }
    }
    else if (importedIcon_ != 0)
    {
        delete importedIcon_;
    }
}
