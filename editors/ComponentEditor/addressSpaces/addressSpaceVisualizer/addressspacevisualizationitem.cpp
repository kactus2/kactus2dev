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
VisualizerItem(parent),
expressionParser_(expressionParser),
addressSpaceWidth_(addressSpaceWidth),
conflicted_(false),
overlapped_(false)
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
// Function: setLeftTopCorner()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setTopLabelText(quint64 address)
{
	QString padded = addr2Str(address, getBitWidth());
	VisualizerItem::setTopLabelText(padded);
}

//-----------------------------------------------------------------------------
// Function: setRightTopCorner()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setRightTopCorner(quint64 address)
{
	QString padded = addr2Str(address, getBitWidth());
	VisualizerItem::setTopLabelText(padded);
}

//-----------------------------------------------------------------------------
// Function: setLeftBottomCorner()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setBottomLabelText(quint64 address)
{
	QString padded = addr2Str(address, getBitWidth());
	VisualizerItem::setBottomLabelText(padded);
}	

//-----------------------------------------------------------------------------
// Function: setRightBottomCorner()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setRightBottomCorner(quint64 address)
{
	QString padded = addr2Str(address, getBitWidth());
	VisualizerItem::setBottomLabelText(padded);
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
// Function: setConflicted()
//-----------------------------------------------------------------------------
void AddressSpaceVisualizationItem::setConflicted(bool conflicted)
{
    conflicted_ = conflicted;

    if (conflicted)
    {
        setBrush(KactusColors::MISSING_COMPONENT);
    }
    else
    {
        setBrush(defaultBrush());
    }
}

//-----------------------------------------------------------------------------
// Function: isConflicted()
//-----------------------------------------------------------------------------
bool AddressSpaceVisualizationItem::isConflicted() const
{
    return conflicted_;
}

//-----------------------------------------------------------------------------
// Function: addr2Str()
//-----------------------------------------------------------------------------
QString AddressSpaceVisualizationItem::addr2Str(quint64 const address, int const bitWidth)
{
	// convert the number into hexadecimal form
	QString str = QString::number(address, 16);
	str = str.toUpper();

	// one hexadecimal number accounts for four bits
	int fieldSize = bitWidth / 4;
	QString padded = QString("%1").arg(str, fieldSize, QChar('0'));

	// group the string to groups of four characters
	int size = padded.size();
	for (int i = size; i > 0; i -= 4)
    {
		padded.insert(i, " ");
	}

	return padded;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceVisualizationItem::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> AddressSpaceVisualizationItem::getExpressionParser() const
{
    return expressionParser_;
}
