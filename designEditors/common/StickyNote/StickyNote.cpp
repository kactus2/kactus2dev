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
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

//-----------------------------------------------------------------------------
// Function: StickyNote::StickyNote()
//-----------------------------------------------------------------------------
StickyNote::StickyNote(QSharedPointer<Kactus2Group> extension, QGraphicsItem* parent):
    QGraphicsItemGroup(parent),
    oldPos_(),
    extension_(extension),
    position_(),
    content_(),
    textArea_(0)
{
    setItemOptions();
    createGluedEdge();
    createWritableArea();

    initializeExtensions();

    connect(textArea_, SIGNAL(contentChanged()), this, SLOT(onTextEdited()), Qt::UniqueConnection);
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
    return extension_;
}

//-----------------------------------------------------------------------------
// Function: StickyNote::onTextEdited()
//-----------------------------------------------------------------------------
void StickyNote::onTextEdited()
{
    content_->setValue(textArea_->toPlainText());
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

//-----------------------------------------------------------------------------
// Function: StickyNote::initializeExtensions()
//-----------------------------------------------------------------------------
void StickyNote::initializeExtensions()
{
    initializePosition();
    initializeContent();
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializePosition()
//-----------------------------------------------------------------------------
void StickyNote::initializePosition()
{
    if (extension_->getByType("kactus2:position").isEmpty())
    {
        position_ = QSharedPointer<Kactus2Position>(new Kactus2Position(QPointF()));
        extension_->addToGroup(position_);
    }
    else
    {
        position_ = extension_->getByType("kactus2:position").first().dynamicCast<Kactus2Position>();
    }

    setPos(position_->position());
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializeContent()
//-----------------------------------------------------------------------------
void StickyNote::initializeContent()
{
    if (extension_->getByType("kactus2:content").isEmpty())
    {
        content_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:content", ""));
        extension_->addToGroup(content_);
    }
    else
    {
        content_ = extension_->getByType("kactus2:content").first().dynamicCast<Kactus2Value>();
    }

    textArea_->setPlainText(content_->value());
}
