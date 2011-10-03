//-----------------------------------------------------------------------------
// File: AppPlaceholderItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.3.2011
//
// Description:
// Application placeholder graphics item.
//-----------------------------------------------------------------------------

#include "AppPlaceholderItem.h"

#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>

#include "ProgramEntityItem.h"
#include "EndpointDesignDiagram.h"

#include <common/diagramgrid.h>
#include <mainwindow/mainwindow.h>

//-----------------------------------------------------------------------------
// Function: AppPlaceholderItem()
//-----------------------------------------------------------------------------
AppPlaceholderItem::AppPlaceholderItem(ProgramEntityItem* parent) : QGraphicsRectItem(parent),
                                                                    parent_(parent)
{
    Q_ASSERT(parent_ != 0);

    //setFlag(ItemIsSelectable);
    setRect(-WIDTH / 2, 0, WIDTH, HEIGHT);
    setPen(QPen(Qt::black, 0, Qt::DashLine));

    QGraphicsTextItem* label = new QGraphicsTextItem(this);
    label->setHtml("<center>Drag an <b>Application</b> from the library OR double-click to create new</center>");
    label->setTextWidth(WIDTH);
    label->setPos(-0.5 * label->boundingRect().width(), 4);
}

//-----------------------------------------------------------------------------
// Function: ~AppPlaceholderItem()
//-----------------------------------------------------------------------------
AppPlaceholderItem::~AppPlaceholderItem()
{
}

//-----------------------------------------------------------------------------
// Function: type()
//-----------------------------------------------------------------------------
int AppPlaceholderItem::type() const
{
    return Type;
}
