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

#include <common/GenericEditProvider.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/Association.h>

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPolygonF>
#include <QStyleOptionGraphicsItem>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: StickyNote::StickyNote()
//-----------------------------------------------------------------------------
StickyNote::StickyNote(QSharedPointer<Kactus2Group> extension, QGraphicsItem* parent):
    QGraphicsItemGroup(parent),
    Associable(),
    oldPos_(),
    extension_(extension),
    positionExtension_(),
    contentExtension_(),
    associationExtensions_(),
    textArea_(0),
    associationButton_(0)
{
    setItemOptions();
    createGluedEdge();
    createWritableArea();
    createAssociationButton();

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

    if (positionChanged())
    {
        QSharedPointer<StickyNoteMoveCommand> moveCommand(new StickyNoteMoveCommand(this, oldPos_));
        diagram->getEditProvider().addCommand(moveCommand);
    }
    else if (hitsAssociationButton(event->pos()))
    {
        emit beginAssociation(this);
    }
}

//-----------------------------------------------------------------------------
// Function: StickyNote::positionChanged()
//-----------------------------------------------------------------------------
bool StickyNote::positionChanged()
{
    return pos() != oldPos_;
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
        // Assert that the new position does not overlap the column headers.
        QPointF newPosition = snapPointToGrid(value.toPointF());
        newPosition.setY(qMax(newPosition.y(), 31.0));

        positionExtension_->setPosition(newPosition);
        return newPosition;
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
// Function: StickyNote::addAssociation()
//-----------------------------------------------------------------------------
void StickyNote::addAssociation(Association* association)
{
    Associable::addAssociation(association);
    associationExtensions_->addToGroup(association->getEndpointExtension());
}

//-----------------------------------------------------------------------------
// Function: StickyNote::removeAssociation()
//-----------------------------------------------------------------------------
void StickyNote::removeAssociation(Association* association)
{
    Associable::removeAssociation(association);
    associationExtensions_->removeFromGroup(association->getEndpointExtension());
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::connectionPoint()
//-----------------------------------------------------------------------------
QPointF StickyNote::connectionPoint(QPointF const&) const
{
    return mapToScene(boundingRect().center());
}

//-----------------------------------------------------------------------------
// Function: StickyNote::onTextEdited()
//-----------------------------------------------------------------------------
void StickyNote::onTextEdited()
{
    contentExtension_->setValue(textArea_->toPlainText());
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

    QGraphicsRectItem* glueEdge = new QGraphicsRectItem(0, 0, DEFAULT_WIDTH, TOP_OFFSET);
    QPen outlinePen(Qt::black, 0, Qt::NoPen);    
    glueEdge->setPen(outlinePen);
    glueEdge->setBrush(topColor);

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
// Function: StickyNote::createAssociationButton()
//-----------------------------------------------------------------------------
void StickyNote::createAssociationButton()
{
    QPixmap pixmap(":/icons/common/graphics/pin--plus.png");
    associationButton_ = new QGraphicsPixmapItem(pixmap, this);
    associationButton_->setToolTip(QObject::tr("Click to add a new association to a component."));
    associationButton_->setPos(DEFAULT_WIDTH - associationButton_->boundingRect().width(), 0);
    associationButton_->setZValue(1.0);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializeExtensions()
//-----------------------------------------------------------------------------
void StickyNote::initializeExtensions()
{
    initializePosition();
    initializeContent();
    initializeAssociations();
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializePosition()
//-----------------------------------------------------------------------------
void StickyNote::initializePosition()
{
    QList<QSharedPointer<VendorExtension> > positionExtensions = extension_->getByType("kactus2:position");

    if (positionExtensions.isEmpty())
    {
        positionExtension_ = QSharedPointer<Kactus2Position>(new Kactus2Position(QPointF()));
        extension_->addToGroup(positionExtension_);
    }
    else
    {
        positionExtension_ = positionExtensions.first().dynamicCast<Kactus2Position>();
    }

    setPos(positionExtension_->position());
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializeContent()
//-----------------------------------------------------------------------------
void StickyNote::initializeContent()
{
    QList<QSharedPointer<VendorExtension> > contentExtensions = extension_->getByType("kactus2:content");

    if (contentExtensions.isEmpty())
    {
        contentExtension_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:content", ""));
        extension_->addToGroup(contentExtension_);
    }
    else
    {
        contentExtension_ = contentExtensions.first().dynamicCast<Kactus2Value>();
    }

    textArea_->setPlainText(contentExtension_->value());
}

//-----------------------------------------------------------------------------
// Function: StickyNote::initializeAssociations()
//-----------------------------------------------------------------------------
void StickyNote::initializeAssociations()
{
    QList<QSharedPointer<VendorExtension> > existingExtensions = extension_->getByType("kactus2:associations");

    if (existingExtensions.isEmpty())
    {
        associationExtensions_ = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:associations"));
        extension_->addToGroup(associationExtensions_);
    }
    else
    {
        associationExtensions_ = existingExtensions.first().dynamicCast<Kactus2Group>();
    }
}

//-----------------------------------------------------------------------------
// Function: StickyNote::clickedAssociationButton()
//-----------------------------------------------------------------------------
bool StickyNote::hitsAssociationButton(QPointF const& clickPosition) const
{
    QPolygonF buttonArea = mapFromItem(associationButton_, associationButton_->boundingRect());
    return buttonArea.containsPoint(clickPosition, Qt::OddEvenFill);
}
