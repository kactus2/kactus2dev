//-----------------------------------------------------------------------------
// File: StickyNote.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Sticky note for designs.
//-----------------------------------------------------------------------------

#include "StickyNote.h"

#include "ColorFillTextItem.h"
#include "StickyNoteMoveCommand.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSharedPointer>

#include <common/GenericEditProvider.h>

#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

//-----------------------------------------------------------------------------
// Function: StickyNote::StickyNote()
//-----------------------------------------------------------------------------
StickyNote::StickyNote(QSharedPointer<VendorExtension> extension, QGraphicsItem* parent):
    QGraphicsItemGroup(parent),
    oldPos_(),
    position_(extension.dynamicCast<Kactus2Position>()),
    textArea_(0)
{
    setItemOptions();
    createGluedEdge();
    createWritableArea();

    connect(textArea_, SIGNAL(contentChanged()), this, SLOT(onTextEdited()), Qt::UniqueConnection);

    setPos(position_->position());
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::~DesignLabel()
//-----------------------------------------------------------------------------
StickyNote::~StickyNote()
{

}

//-----------------------------------------------------------------------------
// Function: StickyNote::mousePressEvent()
//-----------------------------------------------------------------------------
void StickyNote::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    oldPos_ = pos();

    QGraphicsItemGroup::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::mouseMoveEvent()
//-----------------------------------------------------------------------------
void StickyNote::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0 || diagram->isProtected())
    {
        return;
    } 

    QGraphicsItemGroup::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void StickyNote::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    QGraphicsItemGroup::mouseReleaseEvent(event);

    if (pos() != oldPos_)
    {
        QSharedPointer<StickyNoteMoveCommand> moveCommand(new StickyNoteMoveCommand(this, oldPos_));
        diagram->getEditProvider().addCommand(moveCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: StickyNote::beginEditing()
//-----------------------------------------------------------------------------
void StickyNote::beginEditing()
{
    textArea_->setFocus();
}

//-----------------------------------------------------------------------------
// Function: StickyNote::itemChange()
//-----------------------------------------------------------------------------
QVariant StickyNote::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        position_->setPosition(value.toPointF());
    }

    return QGraphicsItemGroup::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::getVendorExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> StickyNote::getVendorExtension() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: StickyNote::onTextEdited()
//-----------------------------------------------------------------------------
void StickyNote::onTextEdited()
{

}

//-----------------------------------------------------------------------------
// Function: StickyNote::setItemOptions()
//-----------------------------------------------------------------------------
void StickyNote::setItemOptions()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);

    setHandlesChildEvents(false);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::createGluedEdge()
//-----------------------------------------------------------------------------
void StickyNote::createGluedEdge()
{
    QColor topColor = QColor("lemonChiffon").darker(103);

    QGraphicsRectItem* glueEdge = new QGraphicsRectItem(0, 0 , DEFAULT_WIDTH, TOP_OFFSET);
    QPen outlinePen;
    outlinePen.setStyle(Qt::NoPen);
    glueEdge->setPen(outlinePen);
    glueEdge->setBrush(QBrush(topColor));

    addToGroup(glueEdge);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::createWritableArea()
//-----------------------------------------------------------------------------
void StickyNote::createWritableArea()
{
    textArea_ = new ColorFillTextItem();
    textArea_->setFill(QColor("lemonChiffon"));
    textArea_->setTextWidth(DEFAULT_WIDTH);
    textArea_->setPos(0, TOP_OFFSET);

    addToGroup(textArea_);
}
