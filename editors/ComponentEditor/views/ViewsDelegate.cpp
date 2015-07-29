//-----------------------------------------------------------------------------
// File: ViewsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.07.2015
//
// Description:
// The delegate that provides editors to edit the views summary.
//-----------------------------------------------------------------------------

#include "ViewsDelegate.h"

//-----------------------------------------------------------------------------
// Function: ViewsDelegate::ViewsDelegate()
//-----------------------------------------------------------------------------
ViewsDelegate::ViewsDelegate(QObject* parent): MultilineDescriptionDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ViewsDelegate::~ViewsDelegate()
//-----------------------------------------------------------------------------
ViewsDelegate::~ViewsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ViewsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ViewsDelegate::descriptionColumn() const
{
    return 2;
}
