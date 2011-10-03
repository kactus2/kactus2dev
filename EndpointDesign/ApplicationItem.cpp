//-----------------------------------------------------------------------------
// File: ApplicationItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.9.2011
//
// Description:
// Graphics item for representing SW application components.
//-----------------------------------------------------------------------------

#include "ApplicationItem.h"

#include "EndpointDesignDiagram.h"

#include <models/component.h>
#include <models/model.h>

#include <QBrush>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: ApplicationItem()
//-----------------------------------------------------------------------------
ApplicationItem::ApplicationItem(QSharedPointer<Component> component,
                                 QString const& instanceName,
                                 QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent) :
SWComponentItem(QRectF(-100, 0, 200, 40), component, instanceName, displayName,
                description, configurableElementValues, parent)
{
    Q_ASSERT_X(component, "ApplicationItem constructor",
        "Null component-pointer given as parameter");

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ~ApplicationItem()
//-----------------------------------------------------------------------------
ApplicationItem::~ApplicationItem()
{
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void ApplicationItem::updateComponent()
{
    SWComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        setBrush(QBrush(QColor(211, 236, 255)));
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void ApplicationItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem::contextMenuEvent(event);

    QMenu menu;
    menu.addAction("Open Source...", this, SLOT(openSource()));

    menu.exec(event->screenPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: openSource()
//-----------------------------------------------------------------------------
void ApplicationItem::openSource()
{
    emit openSource(static_cast<ProgramEntityItem*>(parentItem()));
}
