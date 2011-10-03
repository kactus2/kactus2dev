/* 
 *
 * 		filename: diagramcomponent.cpp
 */

#include "diagramcomponent.h"
#include "diagramport.h"
#include "diagraminterconnection.h"
#include "blockdiagram.h"
#include "DiagramMoveCommands.h"

#include "columnview/DiagramColumn.h"

#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/layouts/VCollisionLayout.h>

#include <models/component.h>
#include <models/businterface.h>
#include <models/model.h>
#include <models/modelparameter.h>
#include <models/view.h>

#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QTextDocument>

#include <QDebug>

DiagramComponent::DiagramComponent(LibraryInterface* lh_, 
								   QSharedPointer<Component> component,
                                   const QString &instanceName, 
								   const QString &displayName,
                                   const QString &description,
                                   const QMap<QString, QString> &configurableElementValues,
                                   QGraphicsItem *parent):
QGraphicsRectItem(parent), 
lh_(lh_), component_(component),
nameLabel_(0), hierIcon_(0),
oldColumn_(0),
leftPorts_(), rightPorts_(), 
connUpdateDisabled_(false),
displayName_(displayName),
description_(description),
configurableValues_(configurableElementValues)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);

	setObjectName(instanceName);

	Q_ASSERT_X(component, "DiagramComponent constructor",
		"Null component-pointer given as parameter");

    QList<QSharedPointer<BusInterface> > busInterfaces
            = component->getBusInterfaces().values();

    int portSpacing = 3*GridSize;
    int portCountLeft = busInterfaces.size()/2.0 + .5;
    setRect(-GridSize*8, 0, GridSize * 8 * 2, 
            6 * GridSize + portSpacing * std::max(portCountLeft - 1, 0));

    bool right = false;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;

    foreach (QSharedPointer<BusInterface> busif, busInterfaces) {
        DiagramPort *port = new DiagramPort(busif, lh_, this);

        // If this is a platform component, place master bus interfaces to the right
        // and slave bus interfaces to the left.
        if (component_->getComponentImplementation() == KactusAttribute::KTS_SW &&
            component_->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM)
        {
            if (busif->getInterfaceMode() == General::MASTER)
            {
                right = true;
            }
            else
            {
                right = false;
            }
        }

        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right) {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        } else {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }

    updateSize();

    // Create the name label.
	nameLabel_ = new QGraphicsTextItem(instanceName, this);
    nameLabel_->setHtml("<center>"+instanceName+"</center>");
    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);
    nameLabel_->setFont(font);

    nameLabel_->setTextWidth(rect().width());
    nameLabel_->setPos(-nameLabel_->boundingRect().width()/2, GridSize);

    updateComponent();

    connect(nameLabel_->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~DiagramComponent()
//-----------------------------------------------------------------------------
DiagramComponent::~DiagramComponent()
{
    // Remove all interconnections.
    foreach (QGraphicsItem *item, childItems()) {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
        foreach (DiagramInterconnection *interconn, diagramPort->getInterconnections()) {
            delete interconn;
        }
    }

    // Remove this item from the column where it resides.
    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }

	emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: getBusPort()
//-----------------------------------------------------------------------------
DiagramPort *DiagramComponent::getBusPort(const QString &name)
{
    foreach (QGraphicsItem *item, QGraphicsRectItem::children()) {
        if (item->type() == DiagramPort::Type) {
            DiagramPort *busPort = qgraphicsitem_cast<DiagramPort *>(item);
            if (busPort->name() == name)
                return busPort;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString DiagramComponent::name()
{
    return nameLabel_->toPlainText();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void DiagramComponent::setName(QString name)
{
	QString oldName = nameLabel_->toPlainText();
	BlockDiagram* diagram = static_cast<BlockDiagram*>(scene());
	diagram->updateInstanceName(oldName, name);
    nameLabel_->setHtml("<center>"+name+"</center>");

    emit contentChanged();
	emit nameChanged(name, oldName);
}

QString DiagramComponent::displayName() const {
	return displayName_;
}

void DiagramComponent::setDisplayName( const QString& displayName ) {
	displayName_ = displayName;
	emit contentChanged();
	emit displayNameChanged(displayName_);
}

QString DiagramComponent::description() const {
	return description_;
}

void DiagramComponent::setDescription( const QString& description ) {
	description_ = description;
	emit contentChanged();
	emit descriptionChanged(description_);
}

//-----------------------------------------------------------------------------
// Function: componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DiagramComponent::componentModel()
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramComponent::itemChange(GraphicsItemChange change,
                                      const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();
        return snapPointToGrid(newPos);
    }
    else if (change == ItemPositionHasChanged)
    {
        emit contentChanged();
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Disable connection updates so that Qt does not update the connections
    // before the possible column change has been done.
    connUpdateDisabled_ = true;

    QGraphicsRectItem::mouseMoveEvent(event);
    
    if (oldColumn_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this, oldColumn_);
    }

    connUpdateDisabled_ = false;

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
        foreach (DiagramInterconnection *interconn, diagramPort->getInterconnections())
        {
            interconn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the interconnections.
        foreach (QGraphicsItem *item, childItems())
        {
            if (item->type() != DiagramPort::Type)
                continue;

            DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
            foreach (DiagramInterconnection *conn, diagramPort->getInterconnections())
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            column->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<DiagramColumn*>(parentItem());

    // Begin the position update for the interconnections.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
        foreach (DiagramInterconnection *conn, diagramPort->getInterconnections())
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void DiagramComponent::onAddPort(DiagramPort* port, bool right)
{
    if (right)
    {
        rightPorts_.append(port);
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        leftPorts_.append(port);
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void DiagramComponent::onMovePort(DiagramPort* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), std::max(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));
    
    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0.0)
    {
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
DiagramPort* DiagramComponent::addPort(QPointF const& pos)
{
    // Determine a unique name for the bus interface.
    QString name = "bus";
    unsigned int count = 0;

    while (component_->getBusInterface(name) != 0)
    {
        ++count;
        name = "bus_" + QString::number(count);
    }

    // Create an empty bus interface and add it to the component model.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(name);
    busIf->setInterfaceMode(General::MASTER);
    component_->addBusInterface(busIf);

    // Create the visualization for the bus interface.
    DiagramPort* port = new DiagramPort(busIf, lh_, this);
    port->setPos(mapFromScene(pos));
    onAddPort(port, mapFromScene(pos).x() >= 0);

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
void DiagramComponent::addPort(DiagramPort* port)
{
    port->setParentItem(this);

    // Add the bus interface to the component.
    component_->addBusInterface(port->getBusInterface());

    // Make preparations.
    onAddPort(port, port->x() >= 0);

    // Update the component size.
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: updateSize()
//-----------------------------------------------------------------------------
void DiagramComponent::updateSize()
{
    // Update the component's size based on the port that is positioned at
    // the lowest level of them all.
    qreal maxY = 4 * GridSize;

    if (!leftPorts_.empty())
    {
        maxY = leftPorts_.back()->y();
    }

    if (!rightPorts_.empty())
    {
        maxY = std::max(maxY, rightPorts_.back()->y());
    }

    setRect(-GridSize * 8, 0, GridSize * 8 * 2, maxY + 2 * GridSize);

    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->updateItemPositions();
    }
}

QStringList DiagramComponent::getViews() const {

	return component_->getViewNames();
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void DiagramComponent::updateComponent()
{
    VLNV* vlnv = component_->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        if (component_->isBus())
            setBrush(QBrush(QColor(0xce,0xdf,0xff))); 
        else
            setBrush(QBrush(QColor(0xa5,0xc3,0xef)));
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (component_->getModel()->hasHierView() && hierIcon_ == 0)
    {
        hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/hierarchy.png"), this);
        hierIcon_->setPos(58, 6);
    }
    else if (hierIcon_ != 0)
    {
        delete hierIcon_;
    }

    // Set the tooltip.
    if (vlnv->isValid())
    {
        setToolTip("Vendor : " + vlnv->getVendor() + "\n"
            + "Library : " + vlnv->getLibrary() + "\n"
            + "Name : " + vlnv->getName() + "\n"
            + "Version : " + vlnv->getVersion());
    }
    else
    {
        setToolTip("Unpackaged component. No VLNV assigned!");
    }
}

//-----------------------------------------------------------------------------
// Function: removePort()
//-----------------------------------------------------------------------------
void DiagramComponent::removePort(DiagramPort* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    updateSize();
    
    component_->removeBusInterface(port->getBusInterface().data());
}

//-----------------------------------------------------------------------------
// Function: isConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
bool DiagramComponent::isConnectionUpdateDisabled() const
{
    return connUpdateDisabled_;
}

QMap<QString, QString>& DiagramComponent::getConfigurableElements() {
	return configurableValues_;
}

void DiagramComponent::setConfigurableElements( const QMap<QString, QString>& confElements ) {
	configurableValues_ = confElements;
	emit contentChanged();
	emit confElementsChanged(configurableValues_);
}
