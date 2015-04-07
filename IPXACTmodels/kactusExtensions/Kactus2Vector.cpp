//-----------------------------------------------------------------------------
// File: Kactus2Vector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// Kactus2 vendor extension for vectors.
//-----------------------------------------------------------------------------

#include "Kactus2Vector.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Vector::Kactus2Vector()
//-----------------------------------------------------------------------------
Kactus2Vector::Kactus2Vector(QString left, QString right):
Kactus2Array(left, right)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Vector::~Kactus2Vector()
//-----------------------------------------------------------------------------
Kactus2Vector::~Kactus2Vector()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Vector::clone()
//-----------------------------------------------------------------------------
Kactus2Vector* Kactus2Vector::clone() const
{
    return new Kactus2Vector(getLeft(), getRight());
}

//-----------------------------------------------------------------------------
// Function: Kactus2Vector::type()
//-----------------------------------------------------------------------------
QString Kactus2Vector::type() const
{
    return QString("kactus2:vector");
}
