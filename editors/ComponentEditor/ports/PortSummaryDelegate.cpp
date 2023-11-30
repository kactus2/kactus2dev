//-----------------------------------------------------------------------------
// File: PortSummaryDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.11.2023
//
// Description:
// Delegate for port summary model.
//-----------------------------------------------------------------------------

#include "PortSummaryDelegate.h"
#include "PortSummaryColumns.h"

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PortSummaryDelegate::columnAcceptsExpression(int column) const
{
    return PortsDelegate::columnAcceptsExpression(column);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::descriptionColumn() const
{
    return PortSummaryColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::nameColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::nameColumn() const
{
    return PortSummaryColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::typeColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::typeColumn() const
{
    return PortSummaryColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::tagColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::tagColumn() const
{
    return PortSummaryColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::adHocColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::adHocColumn() const
{
    return PortSummaryColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::arrayLeftColumn() const
{
    return PortSummaryColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int PortSummaryDelegate::arrayRightColumn() const
{
    return PortSummaryColumns::ARRAY_RIGHT;
}
