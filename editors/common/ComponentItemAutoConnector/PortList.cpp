//-----------------------------------------------------------------------------
// File: PortList.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.01.2019
//
// Description:
// Lists component ports as name, direction pairs.
//-----------------------------------------------------------------------------

#include "PortList.h"

#include <common/graphicsItems/ComponentItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: PortList::PortList()
//-----------------------------------------------------------------------------
PortList::PortList(ComponentItem* componentItem, QWidget* parent):
QListWidget(parent),
componentItem_(componentItem)
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);

    setupPortList(componentItem->componentModel());
}

//-----------------------------------------------------------------------------
// Function: PortList::~PortList()
//-----------------------------------------------------------------------------
PortList::~PortList()
{

}

//-----------------------------------------------------------------------------
// Function: PortList::getContainingItem()
//-----------------------------------------------------------------------------
ComponentItem* PortList::getContainingItem() const
{
    return componentItem_;
}

//-----------------------------------------------------------------------------
// Function: PortList::setupPortList()
//-----------------------------------------------------------------------------
void PortList::setupPortList(QSharedPointer<Component> containingComponent)
{
    for (auto currentPort : *containingComponent->getPorts())
    {
        QString iconPath = ":icons/common/graphics/cross.png";
        if (currentPort->getDirection() == DirectionTypes::IN)
        {
            iconPath = ":icons/common/graphics/input.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::OUT)
        {
            iconPath = ":icons/common/graphics/output.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::INOUT)
        {
            iconPath = ":icons/common/graphics/inout.png";
        }
        else if (currentPort->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
        {
            iconPath = ":icons/common/graphics/phantom.png";
        }

        new QListWidgetItem(QIcon(iconPath), currentPort->name(), this);
    }
}

//-----------------------------------------------------------------------------
// Function: PortList::mousePressEvent()
//-----------------------------------------------------------------------------
void PortList::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QMimeData* newMimeData = new QMimeData();
        QListWidgetItem* selectedItem = currentItem();
        newMimeData->setText(selectedItem->text());

        QRect itemRectangle =  fontMetrics().boundingRect(currentItem()->text());
        itemRectangle.setWidth(itemRectangle.width() + 30);

        QPixmap* newPixMap = new QPixmap(itemRectangle.size());
        QPainter* pixMapPainter = new QPainter(newPixMap);
        pixMapPainter->drawText(24, itemRectangle.height() - 2, currentItem()->text());
        pixMapPainter->drawPixmap(2, 0, 17, 17, currentItem()->icon().pixmap(17, 17));

        QDrag* newDrag = new QDrag(this);
        newDrag->setMimeData(newMimeData);
        newDrag->setPixmap(*newPixMap);

        newDrag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
    }
}
