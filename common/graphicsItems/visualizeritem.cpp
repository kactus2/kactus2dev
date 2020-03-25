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
    labelPositioning_(VisualizerItem::UNDEFINED),
    nameItem_(this),
    topTextItem_(this),
    bottomTextItem_(this)
{
	setRect(0, 0, VisualizerItem::DEFAULT_WIDTH, VisualizerItem::DEFAULT_HEIGHT);

	QFont font = nameItem_.font();
	font.setPointSize(VisualizerItem::FONT_NAMESIZE);

	nameItem_.setFont(font);

	// set the fonts for the corner labels
	QFont cornerFont = topTextItem_.font();
	cornerFont.setPointSize(VisualizerItem::FONT_CORNERSIZE);
	topTextItem_.setFont(cornerFont);
	bottomTextItem_.setFont(cornerFont);
	
	setFlags(QGraphicsItem::ItemIsSelectable);    

    setLayoutType(LABELS_LEFT);
	VisualizerItem::reorganizeChildren();
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
// Function: VisualizerItem::name()
//-----------------------------------------------------------------------------
QString VisualizerItem::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setWidth()
//-----------------------------------------------------------------------------
void VisualizerItem::setWidth(qreal width)
{
    setRect(0, 0, width, VisualizerItem::DEFAULT_HEIGHT);
    VisualizerItem::reorganizeChildren();

    if (clipText_)
    {
        clipName();
        clipAddress(topText_, &topTextItem_);
        clipAddress(bottomText_, &bottomTextItem_);
    }
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal VisualizerItem::itemTotalWidth() const
{
    return VisualizerItem::DEFAULT_WIDTH;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::reorganizeChildren()
//-----------------------------------------------------------------------------
void VisualizerItem::reorganizeChildren()
{
    if (labelPositioning_ == LABELS_LEFT)
    {
        setLeftBottomPosition(&bottomTextItem_);

        int xCoordinate = rect().width() / 3 + VisualizerItem::NAME_INDENTATION;
        int yCoordinate = (rect().height() - nameItem_.boundingRect().height()) / 2;

        nameItem_.setPos(xCoordinate, yCoordinate);
    }
    else if (labelPositioning_ == LABELS_RIGHT)
    {
        setRightTopPosition(&topTextItem_);
        setRightBottomPosition(&bottomTextItem_);
    }
    else // TOP
    {
        setRightTopPosition(&bottomTextItem_);

        int xCoordinate = qMax(qreal(0), rect().center().x() - (nameItem_.boundingRect().width() / 2));
        int yCoordinate = rect().bottomRight().y() - VisualizerItem::FONT_NAMESIZE -
            3 * VisualizerItem::CORNER_INDENTATION;
        nameItem_.setPos(xCoordinate, yCoordinate);
    }

}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setName()
//-----------------------------------------------------------------------------
void VisualizerItem::setName(QString const& name)
{	
    name_ = name;
    if (clipText_)
    {
        clipName();
    }
    else
    {
        nameItem_.setText(name);
    }    
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setTopLabelText()
//-----------------------------------------------------------------------------
void VisualizerItem::setTopLabelText(QString const& text) 
{
    topText_ = text;
    if (clipText_)
    {
        clipAddress(text, &topTextItem_);

    }
    else
    {
        topTextItem_.setText(text);
    }
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setBottomLabelText()
//-----------------------------------------------------------------------------
void VisualizerItem::setBottomLabelText(QString const& text) 
{
    bottomText_ = text;
    if (clipText_)
    {
        clipAddress(text, &bottomTextItem_);
    }
    else
    {
        bottomTextItem_.setText(text);
    }
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setLayoutType()
//-----------------------------------------------------------------------------
void VisualizerItem::setLayoutType(LabelLayout labelLayout)
{
    bool changed = labelLayout != labelPositioning_;
    labelPositioning_ = labelLayout;

    if (changed)
    {
        setLabelPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::setClipText()
//-----------------------------------------------------------------------------
void VisualizerItem::setClipText(bool enableClip)
{
    clipText_ = enableClip;
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::minimumRect()
//-----------------------------------------------------------------------------
QRectF VisualizerItem::minimumRect() const 
{
	return QRectF(0, 0, VisualizerItem::DEFAULT_WIDTH, VisualizerItem::DEFAULT_HEIGHT);
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
// Function: VisualizerItem::setLabelPositions()
//-----------------------------------------------------------------------------
void VisualizerItem::setLabelPositions()
{
    if (labelPositioning_ == VisualizerItem::LABELS_LEFT)
    {
        setLeftTopPosition(&topTextItem_);
        setLeftBottomPosition(&bottomTextItem_);

        int xCoordinate = rect().width() / 3 + VisualizerItem::NAME_INDENTATION;
        int yCoordinate = (rect().height() - nameItem_.boundingRect().height()) / 2;

        nameItem_.setPos(xCoordinate, yCoordinate);
    }
    else if (labelPositioning_ == VisualizerItem::LABELS_RIGHT)
    {
        setRightTopPosition(&topTextItem_);
        setRightBottomPosition(&bottomTextItem_);

        int xCoordinate = NAME_INDENTATION;
        int yCoordinate = (rect().height() - nameItem_.boundingRect().height()) / 2;
        nameItem_.setPos(xCoordinate, yCoordinate);
    }
    else
    {
        setLeftTopPosition(&topTextItem_);
        setRightTopPosition(&bottomTextItem_);

        int xCoordinate = qMax(qreal(0), rect().center().x() - (nameItem_.boundingRect().width() / 2));
        int yCoordinate = rect().bottomRight().y() - VisualizerItem::FONT_NAMESIZE -
            3 * VisualizerItem::CORNER_INDENTATION;
        nameItem_.setPos(xCoordinate, yCoordinate);
    }
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
        VisualizerItem::DEFAULT_HEIGHT - VisualizerItem::NAME_INDENTATION - VisualizerItem::CORNER_INDENTATION);
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
// Function: VisualizerItem::clipName()
//-----------------------------------------------------------------------------
void VisualizerItem::clipName()
{
    int maxLength = 0;

    if (labelPositioning_ == LABELS_TOP)
    {
        maxLength = rect().width() - 2 * CORNER_INDENTATION;
    }
    else
    {
        maxLength = 2 * rect().width() / 3 - 2 * NAME_INDENTATION;
    }

    QFontMetrics fontMetrics(nameItem_.font());
    int charCount = qMax(int(0), maxLength) / fontMetrics.width("x");

    nameItem_.setText(clipText(name_, charCount));
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::clipAddress()
//-----------------------------------------------------------------------------
void VisualizerItem::clipAddress(QString const& text, QGraphicsSimpleTextItem* item)
{
    int maxLength = 0;

    if (labelPositioning_ == LABELS_LEFT)
    {
        maxLength = rect().width() / 3 - NAME_INDENTATION;
    }
    else if (labelPositioning_ == LABELS_RIGHT)
    {
        maxLength = rect().width() / 3 - NAME_INDENTATION;
    }
    else
    {
        maxLength = rect().width() / 2 * VisualizerItem::CORNER_INDENTATION;
    }

    QFontMetrics labelMetrics(item->font());
    int maxCharacters = qMax(int(0), maxLength) / labelMetrics.width("x");

    item->setText(clipText(text, maxCharacters));
}

//-----------------------------------------------------------------------------
// Function: VisualizerItem::clipText()
//-----------------------------------------------------------------------------
QString VisualizerItem::clipText(QString const& text, int maxChars) const
{
    // if the text is too wide to be displayed at all.
    if (maxChars < 3)
    {
        return QString();
    }

    if (text.size() <= maxChars)
    {
        return text;
    }

    // add "..." to the end to indicate the text has been partly hidden.
    return text.left(maxChars - 3) % QStringLiteral("...");
}
