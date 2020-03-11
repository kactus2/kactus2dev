//-----------------------------------------------------------------------------
// File: SimpleNameGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.02.2020
//
// Description:
// Simplified name group data model.
//-----------------------------------------------------------------------------

#include "SimpleNameGroup.h"

//-----------------------------------------------------------------------------
// Function: SimpleNameGroup::SimpleNameGroup()
//-----------------------------------------------------------------------------
SimpleNameGroup::SimpleNameGroup(QString const& name, QString const& description):
name_(name),
description_(description)
{

}

//-----------------------------------------------------------------------------
// Function: SimpleNameGroup::getName()
//-----------------------------------------------------------------------------
QString SimpleNameGroup::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: SimpleNameGroup::getDescription()
//-----------------------------------------------------------------------------
QString SimpleNameGroup::getDescription() const
{
    return description_;
}
