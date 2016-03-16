//-----------------------------------------------------------------------------
// File: DesignInstantiationsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.01.2016
//
// Description:
// The delegate that provides editors to edit the design instantiations summary.
//-----------------------------------------------------------------------------

#include "DesignInstantiationsDelegate.h"
#include "DesignInstantiationColumns.h"

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsDelegate::DesignInstantiationsDelegate()
//-----------------------------------------------------------------------------
DesignInstantiationsDelegate::DesignInstantiationsDelegate(QObject* parent): MultilineDescriptionDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsDelegate::~DesignInstantiationsDelegate()
//-----------------------------------------------------------------------------
DesignInstantiationsDelegate::~DesignInstantiationsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: DesignInstantiationsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int DesignInstantiationsDelegate::descriptionColumn() const
{
    return DesignInstantiationsColumns::DESCRIPTION;
}
