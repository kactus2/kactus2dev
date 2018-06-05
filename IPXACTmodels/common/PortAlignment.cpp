//-----------------------------------------------------------------------------
// File: PortAlignment.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2016
//
// Description:
// Implementation of port alignment class.
//-----------------------------------------------------------------------------

#include "PortAlignment.h"

//-----------------------------------------------------------------------------
// Function: PortAlignment::PortAlignment()
//-----------------------------------------------------------------------------
PortAlignment::PortAlignment(int leftAlignment, int rightAlignment):
leftAlignment_(leftAlignment),
    rightAlignment_(rightAlignment),
    validAlignment_(true)
{

}

//-----------------------------------------------------------------------------
// Function: PortAlignment::PortAlignment()
//-----------------------------------------------------------------------------
PortAlignment::PortAlignment(PortAlignment const& other):
leftAlignment_(other.leftAlignment_),
    rightAlignment_(other.rightAlignment_),
    validAlignment_(other.validAlignment_)
{

}

//-----------------------------------------------------------------------------
// Function: PortAlignment::~PortAlignment()
//-----------------------------------------------------------------------------
PortAlignment::~PortAlignment()
{

}

//-----------------------------------------------------------------------------
// Function: PortAlignment::getLeftAlignment()
//-----------------------------------------------------------------------------
int PortAlignment::getLeftAlignment() const
{
    return leftAlignment_;
}

//-----------------------------------------------------------------------------
// Function: PortAlignment::setLeftAlignment()
//-----------------------------------------------------------------------------
void PortAlignment::setLeftAlignment(int newLeftAlignment)
{
    leftAlignment_ = newLeftAlignment;
}

//-----------------------------------------------------------------------------
// Function: PortAlignment::getRightAlignment()
//-----------------------------------------------------------------------------
int PortAlignment::getRightAlignment() const
{
    return rightAlignment_;
}

//-----------------------------------------------------------------------------
// Function: PortAlignment::setRightAlignment()
//-----------------------------------------------------------------------------
void PortAlignment::setRightAlignment(int newRightAlignment)
{
    rightAlignment_ = newRightAlignment;
}

//-----------------------------------------------------------------------------
// Function: PortAlignment::isValid()
//-----------------------------------------------------------------------------
bool PortAlignment::isValid() const
{
    return validAlignment_;
}

//-----------------------------------------------------------------------------
// Function: PortAlignment::setAlignmentValidity()
//-----------------------------------------------------------------------------
void PortAlignment::setAlignmentValidity(bool validity)
{
    validAlignment_ = validity;
}
