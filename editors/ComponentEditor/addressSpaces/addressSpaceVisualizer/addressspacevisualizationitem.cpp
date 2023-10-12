//-----------------------------------------------------------------------------
// File: addressspacevisualizationitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.12.2012
//
// Description:
// The base class to visualize objects in address space editor.
//-----------------------------------------------------------------------------

#include "addressspacevisualizationitem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <common/KactusColors.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizationItem()
//-----------------------------------------------------------------------------
AddressSpaceVisualizationItem::AddressSpaceVisualizationItem(QString const& addressSpaceWidth,
                                                             QSharedPointer<ExpressionParser> expressionParser,
															 QGraphicsItem* parent):
AddressableItem(parent),
expressionParser_(expressionParser),
addressSpaceWidth_(addressSpaceWidth)
{
    setRect(0, 0, VisualizerItem::DEFAULT_WIDTH, AddressSpaceVisualizationItem::SEGMENT_HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: getBitWidth()
//-----------------------------------------------------------------------------
int AddressSpaceVisualizationItem::getBitWidth() const
{
	return expressionParser_->parseExpression(addressSpaceWidth_).toInt();
}

//-----------------------------------------------------------------------------
// Function: setBottomCoordinate()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setBottomCoordinate(qreal yCoordinate)
{
	qreal width = rect().width();
	qreal height = yCoordinate - y();
	setRect(0, 0, width, height);
	repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: setHeight()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setHeight(qreal height)
{
	qreal width = rect().width();

    setRect(0, 0, width, height);
    
	VisualizerItem::repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: setOverlappingTop()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setTopLabelText(firstFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {        
        VisualizerItem::setBottomLabelText("");
    }
}

//-----------------------------------------------------------------------------
// Function: getOverlappingTop()
//-----------------------------------------------------------------------------
quint64 AddressSpaceVisualizationItem::getOverlappingTop()
{
    return firstFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: setOverlappingBottom()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setBottomLabelText(lastFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        VisualizerItem::setBottomLabelText("");
    }
}

//-----------------------------------------------------------------------------
// Function: getOverlappingBottom()
//-----------------------------------------------------------------------------
quint64 AddressSpaceVisualizationItem::getOverlappingBottom()
{
    return lastFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: setCompleteOverlap()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setCompleteOverlap()
{
    overlapped_ = true;
    hide();
}

//-----------------------------------------------------------------------------
// Function: isCompletelyOverlapped()
//-----------------------------------------------------------------------------
bool AddressSpaceVisualizationItem::isCompletelyOverlapped() const
{
    return overlapped_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizationItem::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> AddressSpaceVisualizationItem::getExpressionParser() const
{
    return expressionParser_;
}
