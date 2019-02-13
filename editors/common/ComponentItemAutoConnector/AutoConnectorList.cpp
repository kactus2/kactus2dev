//-----------------------------------------------------------------------------
// File: AutoConnectorList.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Lists items for auto connector.
//-----------------------------------------------------------------------------

#include "AutoConnectorList.h"

#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: AutoConnectorList::AutoConnectorList()
//-----------------------------------------------------------------------------
AutoConnectorList::AutoConnectorList(QWidget* parent):
QListWidget(parent)
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorList::~AutoConnectorList()
//-----------------------------------------------------------------------------
AutoConnectorList::~AutoConnectorList()
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorList::mousePressEvent()
//-----------------------------------------------------------------------------
void AutoConnectorList::mousePressEvent(QMouseEvent *event)
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
