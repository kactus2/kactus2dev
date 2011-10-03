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

#include <common/DiagramUtil.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>
#include <models/businterface.h>

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
                                           unsigned int id) : QGraphicsRectItem(0, diagram),
                                                              portIDFactory_(), diagram_(diagram),
                                                              component_(component), name_(instanceName),
                                                              id_(id), nameLabel_(0), oldColumn_(0),
                                                              progEntityItems_(), platformPlaceholder_(0),
                                                              platformCompItem_(0)
{
    Q_ASSERT(diagram_ != 0);
    Q_ASSERT(component_ != 0);

    // Set basic graphics properties.
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setBrush(QBrush(QColor(0xa5,0xc3,0xef)));

    // Create the name label.
    nameLabel_ = new QGraphicsTextItem(this);
    nameLabel_->setTextWidth(WIDTH);
    nameLabel_->setPos(-nameLabel_->textWidth() / 2.0, 10);

    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);
    nameLabel_->setFont(font);

    // Create the platform component placeholder.
    platformPlaceholder_ = new PlatformPlaceholderItem(this);

    // Parse the component's design if the component is valid.
    if (component->getVlnv()->isValid())
    {
        QSharedPointer<LibraryComponent> libDesComp = libInterface->getModel(component->getHierRef("kts_sys_ref"));
        QSharedPointer<Design> design = libDesComp.staticCast<Design>();
        QMap<QString, ProgramEntityItem*> progEntityMap;
        unsigned int connIndex = 0;

        foreach (Design::ComponentInstance instance, design->getComponentInstances())
        {
            // Retrieve the component model.
            QSharedPointer<LibraryComponent> libComp = libInterface->getModel(instance.componentRef);
            QSharedPointer<Component> comp = libComp.staticCast<Component>();

            if (comp == 0)
            {
//                 emit errorMessage(tr("The component %1 was not found in the library").arg(
//                                   instance.componentRef.getName()).arg(design->getVlnv()->getName()));
                continue;
            }

            // Create the correct graphics item based on the SW type.
            switch (comp->getComponentSWType())
            {
            case KactusAttribute::KTS_SW_ENDPOINTS:
                {
                    ProgramEntityItem* progEntityItem = new ProgramEntityItem(comp, instance.instanceName,
                                                                              instance.displayName,
                                                                              instance.description,
                                                                              instance.configurableElementValues,
                                                                              this);
                    connect(progEntityItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
                    progEntityItem->setPos(instance.position);

                    // TODO: Set custom endpoint positions.

                    addProgramEntity(progEntityItem);
                    progEntityMap.insert(instance.instanceName, progEntityItem);
                    break;
                }

            case KactusAttribute::KTS_SW_APPLICATION:
                {
                    // Find the correct program entity item based on the interconnection.
                    Design::Interconnection const& conn = design->getInterconnections().at(connIndex);
                    Q_ASSERT(conn.interface2.componentRef == instance.instanceName);
                    ++connIndex;

                    ProgramEntityItem* progEntityItem = progEntityMap.value(conn.interface1.componentRef);
                    Q_ASSERT(progEntityItem != 0);
                    
                    ApplicationItem* appItem = new ApplicationItem(comp, instance.instanceName,
                                                                   instance.displayName,
                                                                   instance.description,
                                                                   instance.configurableElementValues,
                                                                   progEntityItem);
                    connect(appItem, SIGNAL(openSource(ProgramEntityItem*)),
                            static_cast<EndpointDesignDiagram*>(scene()),
                            SIGNAL(openSource(ProgramEntityItem*)), Qt::UniqueConnection);

                    connect(appItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
                    progEntityItem->setApplication(appItem);
                    break;
                }

            case KactusAttribute::KTS_SW_PLATFORM:
                {
                    PlatformComponentItem* platformCompItem =
                        new PlatformComponentItem(comp, instance.instanceName, instance.displayName,
                                                  instance.description, instance.configurableElementValues,
                                                  this);
                    connect(platformCompItem, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));
                    setPlatformComponent(platformCompItem);
                    break;
                }
            }
        }
    }
    
    // Determine the correct height for the node.
    updateSize();
    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: ~MappingComponentItem()
//-----------------------------------------------------------------------------
MappingComponentItem::~MappingComponentItem()
{
    // Remove this item from the column where it resides.
    SystemColumn* column = dynamic_cast<SystemColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void MappingComponentItem::setName(QString const& name)
{
    name_ = name;
    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: setID()
//-----------------------------------------------------------------------------
void MappingComponentItem::setID(unsigned int id)
{
    id_ = id;
    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& MappingComponentItem::getName() const
{
    return name_;
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
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant MappingComponentItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            // Snap to grid.
            return snapPointToGrid(value.toPointF());
        }
        
    case ItemPositionHasChanged:
        {
            emit contentChanged();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void MappingComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<SystemColumn*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void MappingComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
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
    }
}

//-----------------------------------------------------------------------------
// Function: updateNameLabel()
//-----------------------------------------------------------------------------
void MappingComponentItem::updateNameLabel()
{
    nameLabel_->setHtml("<center>" + name_ + " (ID = " + QString::number(id_) + ")</center>");
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
// Function: getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> MappingComponentItem::getComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: addProgramEntity()
//-----------------------------------------------------------------------------
ProgramEntityItem* MappingComponentItem::addProgramEntity(QString const& name, QPointF const& pos)
{
    // Create a new MCAPI endpoint component.
    QSharedPointer<Component> component(new Component(VLNV()));
    component->setComponentImplementation(KactusAttribute::KTS_SW);
    component->setComponentSWType(KactusAttribute::KTS_SW_ENDPOINTS);

    // Add the fixed bus interface to the component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName("app_link");
    busIf->setInterfaceMode(General::SLAVE);
    busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "app_link", "1.0"));

    ProgramEntityItem* item = new ProgramEntityItem(component, name, QString(), QString(),
                                                    QMap<QString, QString>(), this);
    item->setPos(mapFromScene(pos));

    addProgramEntity(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: addProgramEntity()
//-----------------------------------------------------------------------------
void MappingComponentItem::addProgramEntity(ProgramEntityItem* item)
{
    progEntityItems_.append(item);
    VStackedLayout::updateItemMove(progEntityItems_, item, TOP_MARGIN, SPACING);
    VStackedLayout::setItemPos(progEntityItems_, item, 0.0, TOP_MARGIN, SPACING);
    updateSize();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ()
//-----------------------------------------------------------------------------
void MappingComponentItem::updateSize()
{
    // Update the application item positions.
    VStackedLayout::updateItemPositions(progEntityItems_, 0.0, TOP_MARGIN, SPACING);

    // Update the component's size based on the item that is positioned at
    // the lowest level of them all.
    qreal bottom = TOP_MARGIN;

    if (!progEntityItems_.empty())
    {
        bottom = progEntityItems_.back()->y() + progEntityItems_.back()->boundingRect().bottom();
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
    qreal stackHeight = progEntityItems_.first()->boundingRect().height();

    for (int i = 1; i < progEntityItems_.size(); ++i)
    {
        stackHeight += progEntityItems_[i]->boundingRect().height() + SPACING;
    }

    // Restrict to horizontal center and clamp the vertical range.
    qreal y = std::min<qreal>(std::max<qreal>(TOP_MARGIN, item->y()),
                              TOP_MARGIN + stackHeight - item->boundingRect().height());
    item->setPos(snapPointToGrid(0, y));

    VStackedLayout::updateItemMove(progEntityItems_, item, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void MappingComponentItem::onReleaseItem(ProgramEntityItem* item)
{
    setZValue(0.0);
    VStackedLayout::setItemPos(progEntityItems_, item, 0, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: removeProgramEntity()
//-----------------------------------------------------------------------------
void MappingComponentItem::removeProgramEntity(ProgramEntityItem* item)
{
    progEntityItems_.removeAll(item);
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
    QSharedPointer<Design> design(new Design(component_->getHierRef("kts_sys_ref")));

    QList<Design::ComponentInstance> instances;
    QList<Design::Interconnection> interconnections;

    // Add the platform component item to the instance list if set.
    if (platformCompItem_ != 0)
    {
        Design::ComponentInstance instance(platformCompItem_->name(), platformCompItem_->displayName(),
                                           platformCompItem_->description(),
                                           *platformCompItem_->componentModel()->getVlnv(),
                                           platformCompItem_->pos());
        instance.configurableElementValues = platformCompItem_->getConfigurableElements();

        instances.append(instance);
    }

    // Add the program entity items to the instance list.
    foreach (ProgramEntityItem* item, progEntityItems_)
    {
        Design::ComponentInstance instance(item->name(), item->displayName(), item->description(),
                                           *item->componentModel()->getVlnv(), item->pos());
        instance.configurableElementValues = item->getConfigurableElements();

        // Save endpoint positions.
        foreach (EndpointItem* endpoint, item->getEndpoints())
        {
            instance.portPositions[endpoint->getName()] = endpoint->pos();
        }

        instances.append(instance);

        // Save the attached application.
        if (item->getApplication() != 0)
        {
            ApplicationItem* appItem = item->getApplication();

            Design::ComponentInstance appInstance(appItem->name(), appItem->displayName(),
                                                  appItem->description(), *appItem->componentModel()->getVlnv(),
                                                  appItem->pos());
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
    libInterface->writeModelToFile(component_);
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

    foreach (ProgramEntityItem* item, progEntityItems_)
    {
        if (item->name() == names.at(0))
        {
            return item->getEndpoint(names.at(1));
        }
    }

    return 0;
}
