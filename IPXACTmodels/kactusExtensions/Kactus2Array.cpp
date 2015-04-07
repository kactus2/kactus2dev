//-----------------------------------------------------------------------------
// File: Kactus2Array.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.04.2015
//
// Description:
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------

#include "Kactus2Array.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Array::Kactus2Array()
//-----------------------------------------------------------------------------
Kactus2Array::Kactus2Array(QString left, QString right):
left_(left),
right_(right)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::~Kactus2Array()
//-----------------------------------------------------------------------------
Kactus2Array::~Kactus2Array()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::clone()
//-----------------------------------------------------------------------------
Kactus2Array* Kactus2Array::clone() const
{
    return new Kactus2Array(left_, right_);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::type()
//-----------------------------------------------------------------------------
QString Kactus2Array::type() const
{
    return QString("kactus2:array");
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::write()
//-----------------------------------------------------------------------------
void Kactus2Array::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement(type());
    writer.writeTextElement("kactus2:left", left_);
    writer.writeTextElement("kactus2:right", right_);
    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::getLeft()
//-----------------------------------------------------------------------------
QString Kactus2Array::getLeft() const
{
    return left_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::getRight()
//-----------------------------------------------------------------------------
QString Kactus2Array::getRight() const
{
    return right_;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::setLeft()
//-----------------------------------------------------------------------------
void Kactus2Array::setLeft(QString const& newLeft)
{
    left_ = newLeft;
}

//-----------------------------------------------------------------------------
// Function: Kactus2Array::setRight()
//-----------------------------------------------------------------------------
void Kactus2Array::setRight(QString const& newright)
{
    right_ = newright;
}
