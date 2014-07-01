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
#include "StickyNoteEditCommand.h"

#include <common/GenericEditProvider.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/Association/Association.h>

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>
#include <IPXACTmodels/XmlUtils.h>

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
    extension_(),
    positionExtension_(),
    contentExtension_(),
    associationExtensions_(),
    timestampExtension_(),
    textArea_(0),
    timeLabel_(0),
    associationButton_(0)
{
    initializeExtensions();

    setItemOptions();
    createGluedEdge();
    createWritableArea();
    createAssociationButton();    

    connect(textArea_, SIGNAL(contentChanged()), this, SLOT(onTextEdited()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignLabel::~DesignLabel()
//-----------------------------------------------------------------------------
StickyNote::~StickyNote()
{

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
// Function: StickyNote::parseValuesFrom()
//-----------------------------------------------------------------------------
void StickyNote::parseValuesFrom(QDomNode const &node)
{
    int childCount = node.childNodes().count();
    for (int i = 0; i < childCount; ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.nodeName() == positionExtension_->type())
        {
            QPointF position = XmlUtils::parsePoint(childNode);
            positionExtension_->setPosition(position);
            setPos(position);
        }
        else if (childNode.nodeName() == contentExtension_->type())
        {
            QString content = childNode.childNodes().at(0).nodeValue();
            setText(content);
        }
        else if (childNode.nodeName() == associationExtensions_->type())
        {            
            int associationCount = childNode.childNodes().count();
            for(int j = 0; j < associationCount; j++)
            {
                QDomNode assocationNode = childNode.childNodes().at(j);

                QPointF position = XmlUtils::parsePoint(assocationNode);
                QSharedPointer<Kactus2Position> associationEnd(new Kactus2Position(position));

                associationExtensions_->addToGroup(associationEnd);
            }
        }
        else if (childNode.nodeName() == timestampExtension_->type())
        {
            QString timestamp = childNode.childNodes().at(0).nodeValue();
            setTimestamp(timestamp);
        }
        else
        {
            extension_->addToGroup(XmlUtils::createVendorExtensionFromNode(childNode));
        }
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
    QString timestamp = QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);    
    
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
    QColor topColor = QColor("lemonChiffon").darker(103);

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
    extension_ = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:note"));

    positionExtension_ = QSharedPointer<Kactus2Position>(new Kactus2Position(QPointF()));
    extension_->addToGroup(positionExtension_);

    contentExtension_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:content", ""));
    extension_->addToGroup(contentExtension_);

    associationExtensions_ = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:associations"));
    extension_->addToGroup(associationExtensions_);

    timestampExtension_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:timestamp", ""));
    extension_->addToGroup(timestampExtension_);
}

//-----------------------------------------------------------------------------
// Function: StickyNote::clickedAssociationButton()
//-----------------------------------------------------------------------------
bool StickyNote::hitsAssociationButton(QPointF const& clickPosition) const
{
    QPolygonF buttonArea = mapFromItem(associationButton_, associationButton_->boundingRect());
    return buttonArea.containsPoint(clickPosition, Qt::OddEvenFill);
}
