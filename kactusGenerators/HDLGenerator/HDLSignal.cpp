//-----------------------------------------------------------------------------
// File: HDLSignal.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDL signal/wire implementations.
//-----------------------------------------------------------------------------

#include "HDLSignal.h"

//-----------------------------------------------------------------------------
// Function: HDLSignal::HDLSignal()
//-----------------------------------------------------------------------------
HDLSignal::HDLSignal( QObject* parent, 
					   const QString& name,
					   const QString& signalType,
					   int leftBound, 
					   int rightBound, 
					   const QString& description,
					   const QString& defaultValue):
HDLObject(parent, name, signalType, defaultValue, description),
left_(leftBound),
right_(rightBound)
{

}

//-----------------------------------------------------------------------------
// Function: HDLSignal::~HDLSignal()
//-----------------------------------------------------------------------------
HDLSignal::~HDLSignal()
{

}

//-----------------------------------------------------------------------------
// Function: HDLSignal::left()
//-----------------------------------------------------------------------------
int HDLSignal::left() const
{
    return left_;
}

//-----------------------------------------------------------------------------
// Function: HDLSignal::setLeft()
//-----------------------------------------------------------------------------
void HDLSignal::setLeft(int left)
{
    left_ = left;
}

//-----------------------------------------------------------------------------
// Function: HDLSignal::right()
//-----------------------------------------------------------------------------
int HDLSignal::right() const
{
    return right_;
}

//-----------------------------------------------------------------------------
// Function: HDLSignal::setRight()
//-----------------------------------------------------------------------------
void HDLSignal::setRight(int right)
{
    right_ = right;
}

//-----------------------------------------------------------------------------
// Function: HDLSignal::setBounds()
//-----------------------------------------------------------------------------
void HDLSignal::setBounds( int left, int right )
{
	setLeft(left);
	setRight(right);
}
