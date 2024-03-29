//-----------------------------------------------------------------------------
// File: Kactus2Position.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for position.
//-----------------------------------------------------------------------------

#include "Kactus2Position.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: Kactus2Position::Kactus2Position()
//-----------------------------------------------------------------------------
Kactus2Position::Kactus2Position(QPointF position):
    position_(position)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Position::~Kactus2Position()
//-----------------------------------------------------------------------------
Kactus2Position::~Kactus2Position()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Position::clone()
//-----------------------------------------------------------------------------
Kactus2Position* Kactus2Position::clone() const
{
    return new Kactus2Position(position_);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Position::type()
//-----------------------------------------------------------------------------
QString Kactus2Position::type() const
{
    return QStringLiteral("kactus2:position");
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::write()
//-----------------------------------------------------------------------------
void Kactus2Position::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(type());
    writer.writeAttribute(QStringLiteral("x"), QString::number(int(position_.x())));
    writer.writeAttribute(QStringLiteral("y"), QString::number(int(position_.y())));
}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::parsePosition()
//-----------------------------------------------------------------------------
void Kactus2Position::parsePosition(QDomNode const& positionNode)
{
    position_ = CommonItemsReader::parsePoint(positionNode);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Position::position()
//-----------------------------------------------------------------------------
QPointF Kactus2Position::position() const
{
    return position_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Position::setPosition()
//-----------------------------------------------------------------------------
void Kactus2Position::setPosition(QPointF const& pos)
{
    position_ = pos;
}
