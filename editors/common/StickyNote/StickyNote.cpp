//-----------------------------------------------------------------------------
// File: StickyNote.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 5.2.2014
//
// Description:
// Sticky note for designs.
//-----------------------------------------------------------------------------

#include "StickyNote.h"

#include "ColorFillTextItem.h"
#include "StickyNoteMoveCommand.h"
#include "StickyNoteEditCommand.h"

#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/Association/Association.h>

#include <IPXACTmodels/utilities/XmlUtils.h>
#include <IPXACTmodels/common/VendorExtension.h>
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
#include <QDateTime>


//-----------------------------------------------------------------------------
// Function: StickyNote::StickyNote()
//-----------------------------------------------------------------------------
StickyNote::StickyNote(QGraphicsItem* parent):
    QGraphicsItemGroup(parent),
    Associable(),
    oldPos_(),
    extension_(new Kactus2Group(QStringLiteral("kactus2:note"))),
    positionExtension_(new Kactus2Position(QPointF())),
    contentExtension_(new Kactus2Value(QStringLiteral("kactus2:content"), QString())),
    associationExtensions_(new Kactus2Group(QStringLiteral("kactus2:associations"))),
    timestampExtension_(new Kactus2Value(QStringLiteral("kactus2:timestamp"), QString())),
    textArea_(0),
    timeLabel_(0),
    associationButton_(0)
{
    extension_->addToGroup(positionExtension_);
    extension_->addToGroup(contentExtension_);
    extension_->addToGroup(associationExtensions_);    
    extension_->addToGroup(timestampExtension_);

    setItemOptions();
    createGluedEdge();
    createWritableArea();
    createAssociationButton();    

    setTimestamp(getFormattedTimestamp());    

    connect(textArea_, SIGNAL(contentChanged()), this, SLOT(onTextEdited()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::beginEditing()
//-----------------------------------------------------------------------------
void StickyNote::beginEditing()
{
    textArea_->setFocus();
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
    QGraphicsItemGroup::mouseReleaseEvent(event);

    if (positionChanged())
    {
        StickyNoteMoveCommand* moveCommand(new StickyNoteMoveCommand(this, oldPos_));
        emit modified(moveCommand);
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
// Function: StickyNote::getAssociationExtensions()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> StickyNote::getAssociationExtensions() const
{
    return associationExtensions_;
}

//-----------------------------------------------------------------------------
// Function: StickyNote::setVendorExtension()
//-----------------------------------------------------------------------------
void StickyNote::setVendorExtension(QSharedPointer<Kactus2Group> extension)
{
    extension_ = extension;

    if (extension->getByType(QStringLiteral("kactus2:position")).isEmpty() == false)
    {
        positionExtension_ =
            extension->getByType(QStringLiteral("kactus2:position")).first().dynamicCast<Kactus2Position>();
        setPos(positionExtension_->position());
    }
    else
    {
        extension_->addToGroup(positionExtension_);
    }
    
    if (extension->getByType(QStringLiteral("kactus2:content")).isEmpty() == false)
    {
        contentExtension_ =
            extension->getByType(QStringLiteral("kactus2:content")).first().dynamicCast<Kactus2Value>();
        setText(contentExtension_->value());
    }
    else
    {
        extension_->addToGroup(contentExtension_);
    }

    if (extension->getByType(QStringLiteral("kactus2:associations")).isEmpty() == false)
    {
        associationExtensions_ =
            extension->getByType(QStringLiteral("kactus2:associations")).first().dynamicCast<Kactus2Group>();
     }
    else
    {
        extension_->addToGroup(associationExtensions_);
    }

    if (extension->getByType(QStringLiteral("kactus2:timestamp")).isEmpty() == false)
    {
        timestampExtension_ =
            extension->getByType(QStringLiteral("kactus2:timestamp")).first().dynamicCast<Kactus2Value>();
        setTimestamp(timestampExtension_->value());
    }
    else
    {
        extension_->addToGroup(timestampExtension_);
    }
}

//-----------------------------------------------------------------------------
// Function: StickyNote::setText()
//-----------------------------------------------------------------------------
void StickyNote::setText(QString const& text)
{
    contentExtension_->setValue(text);
    textArea_->setPlainText(text);   
}

//-----------------------------------------------------------------------------
// Function: StickyNote::setTimestamp()
//-----------------------------------------------------------------------------
void StickyNote::setTimestamp(QString const& timestamp)
{
    timestampExtension_->setValue(timestamp);
    timeLabel_->setText(timestamp);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::onTextEdited()
//-----------------------------------------------------------------------------
void StickyNote::onTextEdited()
{
    QString timestamp = getFormattedTimestamp();
    
    StickyNoteEditCommand* command = new StickyNoteEditCommand(this, 
        textArea_->toPlainText(), contentExtension_->value(), timestamp, timestampExtension_->value());
    
    command->redo();

    emit modified(command);    
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
    QColor topColor = KactusColors::STICKY_NOTE_BACKGROUND.darker(103);

    QGraphicsRectItem* glueEdge = new QGraphicsRectItem(0, 0, DEFAULT_WIDTH, TOP_OFFSET);
    QPen outlinePen(Qt::black, 0, Qt::NoPen);    
    glueEdge->setPen(outlinePen);
    glueEdge->setBrush(topColor);

    addToGroup(glueEdge);

    timeLabel_ = new QGraphicsSimpleTextItem();
    timeLabel_->setBrush(Qt::gray);
    timeLabel_->setPos(5, TOP_OFFSET/2 - QFontMetrics(timeLabel_->font()).height()/2);
    addToGroup(timeLabel_);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::createWritableArea()
//-----------------------------------------------------------------------------
void StickyNote::createWritableArea()
{
    textArea_ = new ColorFillTextItem();
    textArea_->setFill(KactusColors::STICKY_NOTE_BACKGROUND);
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
// Function: StickyNote::clickedAssociationButton()
//-----------------------------------------------------------------------------
bool StickyNote::hitsAssociationButton(QPointF const& clickPosition) const
{
    QPolygonF buttonArea = mapFromItem(associationButton_, associationButton_->boundingRect());
    return buttonArea.containsPoint(clickPosition, Qt::OddEvenFill);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::getFormattedTimestamp()
//-----------------------------------------------------------------------------
QString StickyNote::getFormattedTimestamp()
{
    return QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
}
