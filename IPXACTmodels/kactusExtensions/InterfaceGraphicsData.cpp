//-----------------------------------------------------------------------------
// File: InterfaceGraphicsData.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2016
//
// Description:
// Interface graphics class for defining properties of graphical interfaces in designs.
//-----------------------------------------------------------------------------

#include "InterfaceGraphicsData.h"

#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::InterfaceGraphicsData()
//-----------------------------------------------------------------------------
InterfaceGraphicsData::InterfaceGraphicsData(QString const& name, QPointF position, QVector2D direction):
name_(name),
positionExtension_(new Kactus2Position(position)),
direction_(direction)
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::InterfaceGraphics()
//-----------------------------------------------------------------------------
InterfaceGraphicsData::InterfaceGraphicsData(const InterfaceGraphicsData& other):
name_(other.name_),
positionExtension_(other.positionExtension_->clone()),
direction_(other.direction_)
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::InterfaceGraphicsData()
//-----------------------------------------------------------------------------
InterfaceGraphicsData::InterfaceGraphicsData(QDomNode& node):
name_(),
positionExtension_(),
direction_()
{
    QDomElement nameElement = node.firstChildElement("kactus2:name");
    if (!nameElement.isNull())
    {
        setName(nameElement.nodeValue());
    }

    QDomElement positionElement = node.firstChildElement("kactus2:position");
    if (!positionElement.isNull())
    {
        int positionX = positionElement.attribute("x").toInt();
        int positionY = positionElement.attribute("y").toInt();

        QPointF loadPosition(positionX, positionY);
        positionExtension_ = QSharedPointer<Kactus2Position> (new Kactus2Position(loadPosition));
    }

    QDomElement directionElement = node.firstChildElement("kactus2:direction");
    if (!directionElement.isNull())
    {
        int directionX = directionElement.attribute("x").toInt();
        int directionY = directionElement.attribute("y").toInt();

        QVector2D loadDirection(directionX, directionY);
        setDirection(loadDirection);
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::~InterfaceGraphicsData()
//-----------------------------------------------------------------------------
InterfaceGraphicsData::~InterfaceGraphicsData()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::clone()
//-----------------------------------------------------------------------------
InterfaceGraphicsData* InterfaceGraphicsData::clone() const
{
    return new InterfaceGraphicsData(*this);
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::operator=()
//-----------------------------------------------------------------------------
InterfaceGraphicsData& InterfaceGraphicsData::operator=(InterfaceGraphicsData const& other)
{
    if (&other != this)
    {
        name_ = other.name_;
        direction_ = other.direction_;

        positionExtension_.clear();
        positionExtension_ = QSharedPointer<Kactus2Position>(other.positionExtension_->clone());
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::type()
//-----------------------------------------------------------------------------
QString InterfaceGraphicsData::type() const
{
    return QString("kactus2:interfaceGraphics");
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::write()
//-----------------------------------------------------------------------------
void InterfaceGraphicsData::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(type());

    writer.writeTextElement("kactus2:name", name_);

    positionExtension_->write(writer);

    writer.writeEmptyElement("kactus2:direction");
    writer.writeAttribute("x", QString::number(direction_.x()));
    writer.writeAttribute("y", QString::number(direction_.y()));

    writer.writeEndElement(); // kactus2:interfaceGraphics
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::setName()
//-----------------------------------------------------------------------------
void InterfaceGraphicsData::setName(QString const& newName)
{
    name_ = newName;
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::getName()
//-----------------------------------------------------------------------------
QString InterfaceGraphicsData::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::setPosition()
//-----------------------------------------------------------------------------
void InterfaceGraphicsData::setPosition(QPointF const& newPosition)
{
    positionExtension_->setPosition(newPosition);
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::getPosition()
//-----------------------------------------------------------------------------
QPointF InterfaceGraphicsData::getPosition() const
{
    return positionExtension_->position();
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::hasPosition()
//-----------------------------------------------------------------------------
bool InterfaceGraphicsData::hasPosition() const
{
    return positionExtension_ && !positionExtension_->position().isNull();
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::setDirection()
//-----------------------------------------------------------------------------
void InterfaceGraphicsData::setDirection(QVector2D const& newDirection)
{
    direction_ = newDirection;
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::getDirection()
//-----------------------------------------------------------------------------
QVector2D InterfaceGraphicsData::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceGraphicsData::hasDirection()
//-----------------------------------------------------------------------------
bool InterfaceGraphicsData::hasDirection() const
{
    return !direction_.isNull();
}
