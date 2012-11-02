/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: visualizeritem.cpp
 *		Project: Kactus 2
 */

#include "visualizeritem.h"

#include <QFontMetrics>
#include <QFont>

VisualizerItem::VisualizerItem( QGraphicsItem* parent /*= 0*/ ):
QGraphicsRectItem(parent),
nameLabel_(this),
leftTopText_(this),
leftBottomText_(this),
rightTopText_(this),
rightBottomText_(this) {

	setRect(0, 0, VisualizerItem::MAX_WIDTH, VisualizerItem::ITEM_HEIGHT);

	QFont font = nameLabel_.font();
	font.setWeight(QFont::Bold);
	nameLabel_.setFont(font);
	nameLabel_.setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

	// set the fonts for the corner labels
	QFont cornerFont = leftTopText_.font();
	cornerFont.setPointSize(cornerFont.pointSize() - 1);
	leftTopText_.setFont(cornerFont);
	leftBottomText_.setFont(cornerFont);
	rightTopText_.setFont(cornerFont);
	rightBottomText_.setFont(cornerFont);

	setFlags(QGraphicsItem::ItemIsSelectable);

	VisualizerItem::reorganizeChildren();
}

VisualizerItem::~VisualizerItem() {
}

void VisualizerItem::setName( const QString& name ) {
	// used to calculate the needed size of the text
	QFontMetrics fontMetrics(nameLabel_.font());

	// The maximum width for the text
	int maxTextSize = rect().width() / 2;

	// if the text is too wide to be displayed fully
	if (fontMetrics.boundingRect(name).width() > maxTextSize) {

		// calculate how many characters can be fitted to the available space
		int charCount = maxTextSize / fontMetrics.width("x");

		// create the name to be displayed.
		QString choppedName(name);
		// remove the characters that do not fit to screen
		choppedName.resize(charCount);
		// add "..." to the end to indicate the name has been partly hidden
		choppedName.append("...");

		nameLabel_.setPlainText(choppedName);
	}
	// if the text fits in the rectangle
	else {
		nameLabel_.setPlainText(name);
	}

	// center the text to the middle of the item
	nameLabel_.setPos(rect().center().x() - (nameLabel_.boundingRect().width() / 2), 0);
}

void VisualizerItem::setLeftTopCorner( const QString& text ) {
	leftTopText_.setText(text);
	// set the left corner text position
	leftTopText_.setPos(2, 0);
}

void VisualizerItem::setLeftBottomCorner( const QString& text ) {
	leftBottomText_.setText(text);
	// set the left bottom text position
	leftBottomText_.setPos(2, rect().bottomLeft().y());
}

void VisualizerItem::setRightTopCorner( const QString& text ) {
	rightTopText_.setText(text);
	// set the right corner text position
	rightTopText_.setPos(rect().topRight().x() - rightTopText_.boundingRect().width(), 0);
}

void VisualizerItem::setRightBottomCorner( const QString& text ) {
	rightBottomText_.setText(text);
	// set the right bottom text position
	rightBottomText_.setPos(rect().topRight().x() - rightBottomText_.boundingRect().width(),
		rect().bottomRight().y());
}

QRectF VisualizerItem::minimumRect() const {
	return QRectF(0, 0, VisualizerItem::MAX_WIDTH, 
		VisualizerItem::ITEM_HEIGHT);
}

void VisualizerItem::reorganizeChildren() {
	// center the text to the middle of the item
	nameLabel_.setPos(rect().center().x() - (nameLabel_.boundingRect().width() / 2), 0);

	leftTopText_.setPos(2, 0);
	leftBottomText_.setPos(2, rect().bottomLeft().y());
	rightTopText_.setPos(rect().topRight().x() - rightTopText_.boundingRect().width(), 0);
	rightBottomText_.setPos(rect().topRight().x() - rightBottomText_.boundingRect().width(),
		rect().bottomRight().y());
}

QRectF VisualizerItem::itemTotalRect() const {
	// the rectangle that contains this item
	QRectF totalRect(rect());

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// the rectangle must contain this item and also the child item
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			totalRect = totalRect.united(mapRectFromItem(child, childItem->itemTotalRect()));
		}
	}
	return totalRect;
}

qreal VisualizerItem::itemTotalWidth() const {
	// The width of this item
	qreal width = rect().width();

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// The larger width
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			width = qMax(width, childItem->itemTotalWidth());
		}
	}
	return width;
}

void VisualizerItem::setWidth( qreal width ) {
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	VisualizerItem::reorganizeChildren();
}

QString VisualizerItem::getName() const {
	return nameLabel_.toPlainText();
}
