//-----------------------------------------------------------------------------
// File: SegmentDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Delegate class for segments within an address space.
//-----------------------------------------------------------------------------

#include "SegmentDelegate.h"
#include "SegmentColumns.h"

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::SegmentDelegate()
//-----------------------------------------------------------------------------
SegmentDelegate::SegmentDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
    QObject* parent)
    : ExpressionDelegate(completionModel, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool SegmentDelegate::columnAcceptsExpression(int column) const
{
    return column == SegmentColumns::OFFSET || column == SegmentColumns::RANGE ||
        column == SegmentColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int SegmentDelegate::descriptionColumn() const
{
    return SegmentColumns::DESCRIPTION;
}

