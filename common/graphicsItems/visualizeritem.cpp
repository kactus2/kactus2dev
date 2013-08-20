/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: visualizeritem.cpp
 *		Project: Kactus 2
 */

#include "visualizeritem.h"

#include <QFontMetrics>
#include <QFont>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: VisualizerItem()
//-----------------------------------------------------------------------------
VisualizerItem::VisualizerItem( QGraphicsItem* parent /*= 0*/ ):
QGraphicsRectItem(parent),
nameHorizontalPos_(VisualizerItem::NAME_LEFT_ALIGN),
nameVerticalPos_(VisualizerItem::NAME_MIDDLE),
name_(),
nameLabel_(this),
leftTopText_(this),
leftBottomText_(this),
rightTopText_(this),
rightBottomText_(this) {

	setRect(0, 0, VisualizerItem::DEFAULT_WIDTH, VisualizerItem::ITEM_HEIGHT);

	QFont font = nameLabel_.font();
	//font.setWeight(QFont::Bold);
	font.setPointSize(VisualizerItem::FONT_NAMESIZE);

	nameLabel_.setFont(font);
	nameLabel_.setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	nameLabel_.setTextInteractionFlags(Qt::NoTextInteraction);

	// set the fonts for the corner labels
	QFont cornerFont = leftTopText_.font();
	cornerFont.setPointSize(VisualizerItem::FONT_CORNERSIZE);
	leftTopText_.setFont(cornerFont);
	leftBottomText_.setFont(cornerFont);
	rightTopText_.setFont(cornerFont);
	rightBottomText_.setFont(cornerFont);

	setFlags(QGraphicsItem::ItemIsSelectable);    

	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: ~VisualizerItem()
//-----------------------------------------------------------------------------
VisualizerItem::~VisualizerItem() {
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void VisualizerItem::setName( const QString& name ) 
{	
    name_ = name;
    setToolTip(name);
    nameLabel_.setPlainText(name);   
    setNameLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: setLeftTopCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftTopCorner( const QString& text ) 
{
    setLeftTopPosition();	
    leftTopText_.setText(text);
}

//-----------------------------------------------------------------------------
// Function: setLeftBottomCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftBottomCorner( const QString& text ) 
{
	setLeftBottomPosition();
    leftBottomText_.setText(text);
}

//-----------------------------------------------------------------------------
// Function: setRightTopCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightTopCorner( const QString& text ) 
{
    rightTopText_.setText(text);
    setRightTopPosition();
}

//-----------------------------------------------------------------------------
// Function: setRightBottomCorner()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightBottomCorner( const QString& text ) 
{
	rightBottomText_.setText(text);
	setRightBottomPosition();
}

//-----------------------------------------------------------------------------
// Function: setLeftTopPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftTopPosition()
{
    leftTopText_.setPos(VisualizerItem::CORNER_INDENTATION, VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: setLeftBottomPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setLeftBottomPosition()
{
    leftBottomText_.setPos(VisualizerItem::CORNER_INDENTATION, 
        rect().bottomLeft().y() - VisualizerItem::FONT_CORNERSIZE - 2 * VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: setRightTopPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightTopPosition()
{
   rightTopText_.setPos(rect().topRight().x() - rightTopText_.boundingRect().width() - VisualizerItem::CORNER_INDENTATION, 
    VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: setRightBottomPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setRightBottomPosition()
{
    rightBottomText_.setPos(rect().topRight().x() - rightBottomText_.boundingRect().width() - VisualizerItem::CORNER_INDENTATION,
		rect().bottomRight().y() - VisualizerItem::FONT_CORNERSIZE - 2 * VisualizerItem::CORNER_INDENTATION);
}

//-----------------------------------------------------------------------------
// Function: setNameLabelPosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setNameLabelPosition()
{
    // Horizontal position.
    int xCoordinate = 0;
    switch (nameHorizontalPos_) {
    case VisualizerItem::NAME_RIGHT_ALIGN: {
        xCoordinate = rightTopText_.x() - nameLabel_.boundingRect().width() - VisualizerItem::NAME_INDENTATION;
        break;
                                     }

    case VisualizerItem::NAME_CENTERED: {
        xCoordinate = rect().center().x() - (nameLabel_.boundingRect().width() / 2);
        break;
                                        }

    case VisualizerItem::NAME_LEFT_ALIGN:
    default: {
        xCoordinate = leftTopText_.boundingRect().right() +  VisualizerItem::NAME_INDENTATION;
        break;
             }
    }

    // Vertical position.
    int yCoordinate = 0;
    switch (nameVerticalPos_) {
    case VisualizerItem::NAME_TOP: {
        yCoordinate = VisualizerItem::CORNER_INDENTATION;
        break;
                                   }

    case VisualizerItem::NAME_BOTTOM: {
        yCoordinate = rect().bottomRight().y() - VisualizerItem::FONT_NAMESIZE  - 3 * VisualizerItem::CORNER_INDENTATION;
        break;
                                      }

    case VisualizerItem::NAME_MIDDLE:
    default: {
        yCoordinate = (rect().height() - nameLabel_.boundingRect().height()) / 2;
        break;
             }
    }

    nameLabel_.setPos(xCoordinate,yCoordinate);
}

//-----------------------------------------------------------------------------
// Function: minimumRect()
//-----------------------------------------------------------------------------
QRectF VisualizerItem::minimumRect() const {
	return QRectF(0, 0, VisualizerItem::DEFAULT_WIDTH, 
		VisualizerItem::ITEM_HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: reorganizeChildren()
//-----------------------------------------------------------------------------
void VisualizerItem::reorganizeChildren() {

	// the label position 
    setLeftTopPosition();
	setLeftBottomPosition();
    setRightTopPosition();
	setRightBottomPosition();
    //setNameLabelPosition();

    nameLabel_.setPlainText(name_);
    resizeLabels();
}

//-----------------------------------------------------------------------------
// Function: itemTotalRect()
//-----------------------------------------------------------------------------
QRectF VisualizerItem::itemTotalRect() const {
	// the rectangle that contains this item
	QRectF totalRect(rect());

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// the rectangle must contain this item and also the child item
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem && childItem->isVisible()) {
			totalRect = totalRect.united(mapRectFromItem(child, childItem->itemTotalRect()));
		}
	}
	return totalRect;
}

//-----------------------------------------------------------------------------
// Function: itemTotalWidth()
//-----------------------------------------------------------------------------
qreal VisualizerItem::itemTotalWidth() const {
	return VisualizerItem::DEFAULT_WIDTH;
}

//-----------------------------------------------------------------------------
// Function: setWidth()
//-----------------------------------------------------------------------------
void VisualizerItem::setWidth( qreal width ) {
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString VisualizerItem::getName() const {
	return name_;
}

//-----------------------------------------------------------------------------
// Function: setNamePosition()
//-----------------------------------------------------------------------------
void VisualizerItem::setNamePosition( const NameHorizontalAlign hPos, 
    const NameVerticalAlign vPos ) 
{
	nameHorizontalPos_ = hPos;
    nameVerticalPos_ = vPos;
}

//-----------------------------------------------------------------------------
// Function: resizeLabels()
//-----------------------------------------------------------------------------
void VisualizerItem::resizeLabels()
{
    qreal leftTextStart = leftTopText_.x();
    qreal leftTextEnd = qMax(leftTopText_.boundingRect().right(),leftBottomText_.boundingRect().right());
    qreal rightTextStart = qMin(rightTopText_.x(),  rightBottomText_.x());

    // Left text does not fit at all.
    if (leftTextStart > rightTextStart)
    {
        leftTopText_.setText("");
        leftBottomText_.setText("");
    }

    // Left text would overlap the right text.
    else if (leftTextEnd > rightTextStart)
    {
        // calculate how many characters can be fitted to the available space
        const unsigned int margin = 2 * VisualizerItem::NAME_INDENTATION;
        int maxTextSize = rightTextStart - leftTextStart - margin; 
        QFontMetrics fontMetrics(leftTopText_.font());                 
        int charCount = (maxTextSize / fontMetrics.width("x")) + 1;

        leftTopText_.setText(clipText(leftTopText_.text(), charCount));
        leftBottomText_.setText(clipText(leftBottomText_.text(), charCount));       
    }    

    // Right text is placed on left text or .
    if (rightTextStart < leftTextStart || leftTextEnd > rightTextStart)
    {
        rightTopText_.setText("");
        rightBottomText_.setText("");
    }
    

    leftTextEnd = qMax(leftTopText_.boundingRect().right(),leftBottomText_.boundingRect().right());
    rightTextStart = qMin(rightTopText_.x(),  rightBottomText_.x());

    // calculate how many characters can be fitted to the available space
    int maxTextSize = 0;                
    if (nameVerticalPos_ == VisualizerItem::NAME_BOTTOM)
    {        
        const unsigned int margin = VisualizerItem::NAME_INDENTATION;
        maxTextSize = rect().right() - rect().left() - margin;
    }
    else
    {   
        const unsigned int margin = 2 * VisualizerItem::NAME_INDENTATION;     
        maxTextSize = rightTextStart - leftTextEnd - margin;
    }   

    QFontMetrics fontMetrics(nameLabel_.font());
    int charCount = (maxTextSize / fontMetrics.width("x")) + 1;

    if (charCount < name_.size())
    {
        nameLabel_.setPlainText(clipText(name_,charCount));        
    }
   
    setNameLabelPosition();
}


//-----------------------------------------------------------------------------
// Function: clipText()
//-----------------------------------------------------------------------------
QString VisualizerItem::clipText(QString const& text, int maxChars) const
{
    // if the text is too wide to be displayed at all.
    if (text.size() > maxChars && maxChars < 2)
    {
        return "";
    }       
    else if (text.size() > maxChars) {    
 
        QString chopped(text);
        chopped.resize(maxChars - 2);

        // add "..." to the end to indicate the text has been partly hidden.
        chopped.append("...");
        return chopped;
    }

    return text;
}