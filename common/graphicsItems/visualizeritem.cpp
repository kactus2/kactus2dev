/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: visualizeritem.cpp
 *		Project: Kactus 2
 */

#include "visualizeritem.h"

#include <QStringBuilder>
#include <QFontMetrics>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: VisualizerItem::VisualizerItem()
//-----------------------------------------------------------------------------
VisualizerItem::VisualizerItem(QGraphicsItem* parent /*= 0*/) :
    QGraphicsRectItem(parent),
    name_(),
    defaultBrush_(),
    nameHorizontalPos_(VisualizerItem::NAME_LEFT_ALIGN),
    nameVerticalPos_(VisualizerItem::NAME_MIDDLE),
    addressPosition_(VisualizerItem::LABELS_LEFT),
    nameLabel_(this),
    topText_(this),
    bottomText_(this)
{
	setRect(0, 0, VisualizerItem::DEFAULT_WIDTH, VisualizerItem::DEFAULT_HEIGHT);

	QFont font = nameLabel_.font();
	font.setPointSize(VisualizerItem::FONT_NAMESIZE);

	nameLabel_.setFont(font);

	// set the fonts for the corner labels
	QFont cornerFont = topText_.font();
	cornerFont.setPointSize(VisualizerItem::FONT_CORNERSIZE);
	topText_.setFont(cornerFont);
	bottomText_.setFont(cornerFont);
	
	setFlags(QGraphicsItem::ItemIsSelectable);    

    setAddressPosition(LABELS_LEFT);
	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setName()
//-----------------------------------------------------------------------------
void VisualizerItem::setName(QString const& name)
{	
    name_ = name;
    nameLabel_.setText(name);

    //TODO: CLIP
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftTopCorner(QString const& text) 
{
    topText_.setText(text);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftBottomCorner(QString const& text) 
{
    bottomText_.setText(text);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setLeftTopPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftTopPosition(QGraphicsSimpleTextItem* label)
{
    label->setPos(VisualizerItem::CORNER_INDENTATION, VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setLeftBottomPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftBottomPosition(QGraphicsSimpleTextItem* label)
{
    label->setPos(VisualizerItem::CORNER_INDENTATION,
        rect().bottomLeft().y() - label->boundingRect().height() - VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setRightTopPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightTopPosition(QGraphicsSimpleTextItem* label)
{
    label->setPos(rect().topRight().x() - label->boundingRect().width() -
        VisualizerItem::CORNER_INDENTATION, VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setRightBottomPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightBottomPosition(QGraphicsSimpleTextItem* label)
{
    label->setPos(rect().topRight().x() - label->boundingRect().width() - VisualizerItem::CORNER_INDENTATION,
		rect().bottomRight().y() - label->boundingRect().height() - VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::minimumRect()
//-----------------------------------------------------------------------------
QRectF VisualizerItem::minimumRect() const 
{
	return QRectF(0, 0, VisualizerItem::DEFAULT_WIDTH, VisualizerItem::DEFAULT_HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::reorganizeChildren()
//-----------------------------------------------------------------------------
void VisualizerItem::reorganizeChildren()
{
    setAddressPosition(addressPosition_);
    resizeLabels();
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setDefaultBrush()
//-----------------------------------------------------------------------------
void VisualizerItem::setDefaultBrush(QBrush brush)
{
    defaultBrush_ = brush;
    setBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::defaultBrush()
//-----------------------------------------------------------------------------
QBrush VisualizerItem::defaultBrush()
{
    return defaultBrush_;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::itemTotalRect()
//-----------------------------------------------------------------------------
QRectF VisualizerItem::itemTotalRect() const
{
	// the rectangle that contains this item
	QRectF totalRect(rect());

	// if there are children	
	for (QGraphicsItem const* child : childItems())
    {
		// the rectangle must contain this item and also the child item
		VisualizerItem const* childItem = dynamic_cast<VisualizerItem const*>(child);
		if (childItem && childItem->isVisible())
        {
			totalRect = totalRect.united(mapRectFromItem(child, childItem->itemTotalRect()));
		}
	}
	return totalRect;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal VisualizerItem::itemTotalWidth() const
{
	return VisualizerItem::DEFAULT_WIDTH;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setWidth()
//-----------------------------------------------------------------------------
void VisualizerItem::setWidth(qreal width)
{
	setRect(0, 0, width, VisualizerItem::DEFAULT_HEIGHT);
	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::name()
//-----------------------------------------------------------------------------
QString VisualizerItem::name() const
{
	return name_;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setNamePosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setNamePosition( const NameHorizontalAlign hPos, const NameVerticalAlign vPos ) 
{
	nameHorizontalPos_ = hPos;
    nameVerticalPos_ = vPos;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setAddressPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setAddressPosition(LabelPosition position)
{
    addressPosition_ = position;

    if (position == VisualizerItem::LABELS_LEFT)
    {
        setLeftTopPosition(&topText_);
        setLeftBottomPosition(&bottomText_);

        int xCoordinate = topText_.x() + topText_.boundingRect().width() + VisualizerItem::NAME_INDENTATION;
        int yCoordinate = (rect().height() - nameLabel_.boundingRect().height())/2;
        nameLabel_.setPos(xCoordinate, yCoordinate);
    }
    else if (position == VisualizerItem::LABELS_RIGHT)
    {
        setRightTopPosition(&topText_);
        setRightBottomPosition(&bottomText_);

       int xCoordinate = topText_.x() - nameLabel_.boundingRect().width() - VisualizerItem::NAME_INDENTATION;
       int yCoordinate = (rect().height() - nameLabel_.boundingRect().height())/2;
       nameLabel_.setPos(xCoordinate, yCoordinate);
    }
    else
    {
        setLeftTopPosition(&topText_);
        setRightTopPosition(&bottomText_);

        int xCoordinate = rect().center().x() - (nameLabel_.boundingRect().width() / 2);
        int yCoordinate = rect().bottomRight().y() - VisualizerItem::FONT_NAMESIZE - 
            3*VisualizerItem::CORNER_INDENTATION;
        nameLabel_.setPos(xCoordinate, yCoordinate);
    }
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::resizeLabels()
//-----------------------------------------------------------------------------
void VisualizerItem::resizeLabels()
{    
    qreal topLeft = 0;
    qreal topRight = 0;
    qreal topSize = 0;

    qreal bottomLeft = 0;
    qreal bottomRight = 0;
    qreal bottomSize = 0;

    qreal nameLeft = 0;
    qreal nameRight = 0;
    qreal nameSize = 0;

    const unsigned int MARGIN = VisualizerItem::CORNER_INDENTATION;

    if (addressPosition_ == LABELS_LEFT)
    {
        topLeft = MARGIN;
        topRight = rect().width() / 3;
        topSize = qMax(0.0, rect().width() / 3 - MARGIN);

        bottomLeft = MARGIN; 
        bottomRight = rect().width() / 3;
        bottomSize = qMax(0.0, rect().width() / 3 - MARGIN);

        nameLeft = rect().width() / 3 + VisualizerItem::NAME_INDENTATION;
        nameRight = rect().width() - VisualizerItem::NAME_INDENTATION;
        nameSize = qMax(0.0, rect().width() - rect().width() / 3 - 2* VisualizerItem::NAME_INDENTATION);
    }
    else if (addressPosition_ == LABELS_RIGHT)
    {
        topLeft = 2 * rect().width() / 3;
        topRight = rect().width() - MARGIN; 
        topSize = qMax(0.0, rect().width() /3 - MARGIN);

        bottomLeft = 2 * rect().width() / 3;
        bottomRight = rect().width() - MARGIN;
        bottomSize = qMax(0.0, rect().width() / 3 - MARGIN);

        nameLeft = MARGIN;
        nameRight = 2*rect().width() / 3 - VisualizerItem::NAME_INDENTATION;
        nameSize = qMax(0.0, 2 * rect().width() / 3 - VisualizerItem::NAME_INDENTATION - MARGIN);
    }
    else
    {
        topLeft = MARGIN;
        topRight = rect().width() / 2 - MARGIN;
        topSize = qMax(0.0, rect().width() / 2 - MARGIN);

        bottomLeft = rect().width() / 2 + MARGIN;
        bottomRight = rect().width() - MARGIN;
        bottomSize = qMax(0.0, rect().width() / 2 - MARGIN);

        nameLeft = VisualizerItem::NAME_INDENTATION;
        nameRight = rect().width() - VisualizerItem::NAME_INDENTATION;
        nameSize = qMax(0.0, rect().width() - 2*VisualizerItem::NAME_INDENTATION);
    }

    QFontMetrics labelMetrics(topText_.font());
    int topCharaters = qMax(0.0, topRight - topLeft)/labelMetrics.width("x");
    int bottomCharacters = qMax(0.0, bottomRight - bottomLeft)/ labelMetrics.width("x");

    QFontMetrics fontMetrics(nameLabel_.font());
    int charCount = qMax(0.0, nameRight - nameLeft)/fontMetrics.width("x");

    topText_.setText(clipText(topText_.text(), topCharaters));
    bottomText_.setText(clipText(bottomText_.text(), bottomCharacters));
    nameLabel_.setText(clipText(name_, charCount));
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::clipText()
//-----------------------------------------------------------------------------
QString VisualizerItem::clipText(QString const& text, int maxChars) const
{
    // if the text is too wide to be displayed at all.
    if (maxChars < 2)
    {
        return QString();
    }

    if (text.size() <= maxChars)
    {
        return text;
    }

    // add "..." to the end to indicate the text has been partly hidden.
    QString chopped(text.left(maxChars - 3) % QStringLiteral("..."));
    return chopped;
}
