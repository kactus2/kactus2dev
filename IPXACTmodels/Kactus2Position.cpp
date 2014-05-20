//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for single values.
//-----------------------------------------------------------------------------

#include "Kactus2Position.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Position::Kactus2Position(QPointF position):
    position_(position)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::~Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Position::~Kactus2Position()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::write()
//-----------------------------------------------------------------------------
void Kactus2Position::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("kactus2:position");
    writer.writeAttribute("x", QString::number(int(position_.x())));
    writer.writeAttribute("y", QString::number(int(position_.y())));
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


