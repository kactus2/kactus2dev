//-----------------------------------------------------------------------------
// File: EndpointStack.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// Endpoint stack graphics item for managing endpoints.
//-----------------------------------------------------------------------------

#include "EndpointStack.h"

#include "SystemAddCommands.h"
#include "EndpointDesignDiagram.h"
#include "EndpointItem.h"
#include "EndpointEditDialog.h"
#include "ProgramEntityItem.h"
#include "MappingComponentItem.h"

#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/layouts/VStackedLayout.h>
#include <common/graphicsItems/GraphicsRectButton.h>

#include <models/businterface.h>
#include <models/component.h>
#include <models/model.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include "models/modelparameter.h"

//-----------------------------------------------------------------------------
// Function: EndpointStack()
//-----------------------------------------------------------------------------
EndpointStack::EndpointStack(ProgramEntityItem* parentNode) : QGraphicsRectItem(parentNode),
                                                              parentProgEntity_(parentNode),
                                                              headerLabel_(0), btnShowHide_(0),
                                                              noneRect_(0), noneLabel_(0),
                                                              btnAdd_(0), endpoints_(),
                                                              expanded_(true), editable_(true)
{
    // Create the header rectangle.
    setBrush(QBrush(QColor(142, 200, 255)));
    setRect(-EndpointItem::WIDTH / 2, 0, EndpointItem::WIDTH, HEIGHT);

    // Create the header label.
    headerLabel_ = new QGraphicsTextItem(this);
    headerLabel_->setHtml(tr("<center>Endpoints</center>"));
    headerLabel_->setTextWidth(EndpointItem::WIDTH);
    headerLabel_->setPos(-headerLabel_->textWidth() / 2.0, 0);

    QFont font = headerLabel_->font();
    font.setBold(true);
    headerLabel_->setFont(font);

    // Create the show/hide button.
    if (expanded_)
    {
        btnShowHide_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/collapse.png"), this);
    }
    else
    {
        btnShowHide_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/expand.png"), this);
    }

    btnShowHide_->setPos(-EndpointItem::WIDTH / 2 + 1, 0);

    // Create the none rectangle and label.
    noneRect_ = new QGraphicsRectItem(-EndpointItem::WIDTH / 2, 0, EndpointItem::WIDTH, HEIGHT, this);
    noneRect_->setPos(0, MIN_Y_PLACEMENT);
    noneRect_->setBrush(QBrush(QColor(215, 249, 217)));
    noneRect_->setVisible(expanded_);

    noneLabel_ = new QGraphicsTextItem(noneRect_);
    noneLabel_->setHtml(tr("<center>None</center>"));
    noneLabel_->setTextWidth(EndpointItem::WIDTH);
    noneLabel_->setPos(-noneLabel_->textWidth() / 2.0, 0);

    // Create the add button with an icon.
    btnAdd_ = new GraphicsRectButton(this);
    btnAdd_->setBrush(QBrush(QColor(215, 249, 217)));
    btnAdd_->setRect(EndpointItem::WIDTH / 2 - HEIGHT, 0, HEIGHT, HEIGHT);
    btnAdd_->setPos(0, MIN_Y_PLACEMENT + HEIGHT);
    btnAdd_->setVisible(expanded_ && editable_);
    connect(btnAdd_, SIGNAL(clicked()), this, SLOT(addEndpoint()));

    QPixmap pixmap(":icons/graphics/add.png");
    QGraphicsPixmapItem* addIcon = new QGraphicsPixmapItem(pixmap.scaled(18, 18), btnAdd_);
    addIcon->setPos(EndpointItem::WIDTH / 2 - HEIGHT / 2 - 8, 1);
}

//-----------------------------------------------------------------------------
// Function: ~EndpointStack()
//-----------------------------------------------------------------------------
EndpointStack::~EndpointStack()
{
    // Free all endpoint ids.
    foreach (EndpointItem* endpoint, endpoints_)
    {
        if (endpoint->getPortID() >= 0)
        {
            parentProgEntity_->getMappingComponent()->getPortIDFactory().freeID(endpoint->getPortID());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::addEndpoint(QString const& name, MCAPIEndpointDirection dir, MCAPIDataType type)
{
    EndpointItem* endpoint = new EndpointItem(parentProgEntity_, name, dir, type);
    connect(endpoint, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    endpoint->setPos(0, -endpoint->boundingRect().top() + MIN_Y_PLACEMENT +
                     endpoints_.size() * EndpointItem::HEIGHT);

    addEndpoint(endpoint);
}

//-----------------------------------------------------------------------------
// Function: addEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::addEndpoint(EndpointItem* endpoint)
{
    endpoint->setParentItem(this);
    endpoint->setVisible(expanded_);

    VStackedLayout::updateItemMove(endpoints_, endpoint, MIN_Y_PLACEMENT, SPACING);
    VStackedLayout::updateItemPositions(endpoints_, 0, MIN_Y_PLACEMENT, SPACING);

    btnAdd_->setPos(0, MIN_Y_PLACEMENT + endpoints_.size() * EndpointItem::HEIGHT);
    noneRect_->setVisible(false);

    // Add the endpoint to the parent program entity's model parameters if the program entity is unpackaged.
    if (!parentProgEntity_->componentModel()->getVlnv()->isValid())
    {
        QSharedPointer<Port> port(new Port());
        port->setName(endpoint->getName());
        port->setTypeName(valueToString(endpoint->getConnectionType()));
        port->setDirection(General::str2Direction(valueToString(endpoint->getMCAPIDirection()), General::IN));

        parentProgEntity_->componentModel()->addPort(port);
    }

    endpoint->createBusInterface(parentProgEntity_->getMappingComponent());
    emit visibleHeightChanged(getVisibleHeight());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: removeEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::removeEndpoint(EndpointItem* endpoint)
{
    // Remove the endpoint and update endpoint positions.
    if (endpoints_.contains(endpoint))
    {
        // Free up the port ID.
        parentProgEntity_->getMappingComponent()->getPortIDFactory().freeID(endpoint->getPortID());

        endpoints_.removeAll(endpoint);
        VStackedLayout::updateItemPositions(endpoints_, 0, MIN_Y_PLACEMENT, SPACING);

        // Update the none rectangle and add button accordingly.
        if (endpoints_.empty())
        {
            noneRect_->setVisible(true);
            btnAdd_->setPos(0, 2 * MIN_Y_PLACEMENT);
        }
        else
        {
            btnAdd_->setPos(0, MIN_Y_PLACEMENT + endpoints_.size() * EndpointItem::HEIGHT);
        }

        // Remove the endpoint from the parent program entity's model parameters.
        if (!parentProgEntity_->componentModel()->getVlnv()->isValid())
        {
            parentProgEntity_->componentModel()->getModel()->removePort(endpoint->getName());
        }

        // Remove the bus interface from the mapping component.
        endpoint->removeBusInterface(parentProgEntity_->getMappingComponent());

        // Inform others about the height change.
        emit visibleHeightChanged(getVisibleHeight());
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::onMoveEndpoint(EndpointItem* endpoint)
{
    setZValue(1001.0);

    // Restrict to horizontal center and clamp the vertical range.
    qreal y = std::min<qreal>(std::max(MIN_Y_PLACEMENT - endpoint->boundingRect().top(), endpoint->y()),
                              MIN_Y_PLACEMENT - endpoint->boundingRect().top() +
                              (endpoints_.size() - 1) * EndpointItem::HEIGHT);
    endpoint->setPos(snapPointToGrid(0, y));

    VStackedLayout::updateItemMove(endpoints_, endpoint, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: onReleaseEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::onReleaseEndpoint(EndpointItem* endpoint)
{
    setZValue(0.0);
    VStackedLayout::setItemPos(endpoints_, endpoint, 0, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: addEndpoint()
//-----------------------------------------------------------------------------
void EndpointStack::addEndpoint()
{
    // Show a dialog for adding an endpoint.
    EndpointEditDialog dialog((QWidget*)scene()->parent());

    if (dialog.exec() == QDialog::Accepted)
    {
        EndpointItem* endpoint = new EndpointItem(parentProgEntity_, dialog.getName(), dialog.getType(),
                                                  dialog.getConnectionType());
        connect(endpoint, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

        endpoint->setPos(0, -endpoint->boundingRect().top() + MIN_Y_PLACEMENT +
                         endpoints_.size() * EndpointItem::HEIGHT);

        // Create an undo command and execute it.
        QSharedPointer<QUndoCommand> cmd(new EndpointAddCommand(this, endpoint));
        static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void EndpointStack::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setExpanded(!isExpanded());
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: getVisibleHeight()
//-----------------------------------------------------------------------------
int EndpointStack::getVisibleHeight() const
{
    if (!expanded_)
    {
        return MIN_Y_PLACEMENT;
    }

    if (endpoints_.empty())
    {
        if (editable_)
        {
            return 3 * MIN_Y_PLACEMENT;
        }
        else
        {
            return 2 * MIN_Y_PLACEMENT;
        }
    }

    if (editable_)
    {
        return (2 * MIN_Y_PLACEMENT + endpoints_.size() * EndpointItem::HEIGHT);
    }
    else
    {
        return (MIN_Y_PLACEMENT + endpoints_.size() * EndpointItem::HEIGHT);
    }
}

//-----------------------------------------------------------------------------
// Function: getEndpoints()
//-----------------------------------------------------------------------------
QList<EndpointItem*> const& EndpointStack::getEndpoints() const
{
    return endpoints_;
}

//-----------------------------------------------------------------------------
// Function: setEditable()
//-----------------------------------------------------------------------------
void EndpointStack::setEditable(bool editable)
{
    editable_ = editable;
    btnAdd_->setVisible(expanded_ && editable_);
}

//-----------------------------------------------------------------------------
// Function: setEndpointOrder()
//-----------------------------------------------------------------------------
void EndpointStack::setEndpointOrder(QMap<QString, QPointF> const& positions)
{
    QMapIterator<QString, QPointF> itrPos(positions);

    // Correct the ordering of the endpoints in the container.
    while (itrPos.hasNext())
    {
        itrPos.next();
        QString const& name = itrPos.key();

        for (int i = 0; i < endpoints_.size(); ++i)
        {
            if (endpoints_.at(i)->getName() == name)
            {
                int index = int(itrPos.value().y());

                if (index < endpoints_.size())
                {
                    endpoints_.swap(i, index);
                }
                break;
            }
        }
    }

    // Update the actual positions.
    updateEndpointPositions();
}

//-----------------------------------------------------------------------------
// Function: updateEndpointPositions()
//-----------------------------------------------------------------------------
void EndpointStack::updateEndpointPositions()
{
    if (expanded_)
    {
        VStackedLayout::updateItemPositions(endpoints_, 0, MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        for (int i = 0; i < endpoints_.size(); ++i)
        {
            endpoints_.at(i)->setPos(0.0, HEIGHT / 2);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: isExpanded()
//-----------------------------------------------------------------------------
bool EndpointStack::isExpanded() const
{
    return expanded_;
}

//-----------------------------------------------------------------------------
// Function: setExpanded()
//-----------------------------------------------------------------------------
void EndpointStack::setExpanded(bool expanded)
{
    expanded_ = expanded;

    // Show/hide endpoints based on the current mode.
    for (int i = 0; i < endpoints_.size(); ++i)
    {
        endpoints_.at(i)->setVisible(expanded_);
    }

    updateEndpointPositions();

    noneRect_->setVisible(expanded_ && endpoints_.empty());
    btnAdd_->setVisible(expanded_ && editable_);

    // Update the button image.
    if (expanded_)
    {
        btnShowHide_->setPixmap(QPixmap(":icons/graphics/collapse.png"));
    }
    else
    {
        btnShowHide_->setPixmap(QPixmap(":icons/graphics/expand.png"));
    }

    emit visibleHeightChanged(getVisibleHeight());
}

//-----------------------------------------------------------------------------
// Function: isEditable()
//-----------------------------------------------------------------------------
bool EndpointStack::isEditable() const
{
    return editable_;
}
