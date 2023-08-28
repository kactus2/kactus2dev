//-----------------------------------------------------------------------------
// File: RegionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Delegate class for segments within an address space.
//-----------------------------------------------------------------------------

#include "RegionsDelegate.h"
#include "RegionColumns.h"

//-----------------------------------------------------------------------------
// Function: RegionDelegate::RegionDelegate()
//-----------------------------------------------------------------------------
RegionsDelegate::RegionsDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
    QObject* parent): 
    ExpressionDelegate(completionModel, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: RegionDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool RegionsDelegate::columnAcceptsExpression(int column) const
{
    return column == RegionColumns::OFFSET || column == RegionColumns::RANGE;
}

//-----------------------------------------------------------------------------
// Function: RegionDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int RegionsDelegate::descriptionColumn() const
{
    return RegionColumns::DESCRIPTION;
}

