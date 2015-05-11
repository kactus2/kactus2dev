//-----------------------------------------------------------------------------
// File: SegmentDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Delegate class for segments within an address space.
//-----------------------------------------------------------------------------

#include "SegmentDelegate.h"

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::SegmentDelegate()
//-----------------------------------------------------------------------------
SegmentDelegate::SegmentDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder, 
    QObject* parent)
    : ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::~SegmentDelegate()
//-----------------------------------------------------------------------------
SegmentDelegate::~SegmentDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: SegmentDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool SegmentDelegate::columnAcceptsExpression(int column) const
{
    return column == 1 || column == 2;
}
