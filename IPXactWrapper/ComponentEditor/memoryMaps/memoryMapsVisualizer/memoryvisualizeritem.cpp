/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizeritem.cpp
 *		Project: Kactus 2
 */

#include "memoryvisualizeritem.h"

#include <QFont>
#include <QTextDocument>
#include <QFontMetrics>

#include <QDebug>

MemoryVisualizerItem::MemoryVisualizerItem(QGraphicsItem *parent,
										   int maxWidth): 
QGraphicsRectItem(parent),
QObject(),
maxWidth_(maxWidth),
nameLabel_(this),
leftTopText_(this),
rightTopText_(this) {

	setRect(0, 0, MemoryVisualizerItem::MAX_ITEM_WIDTH, 
		MemoryVisualizerItem::ITEM_HEIGHT);

	QFont font = nameLabel_.font();
	font.setWeight(QFont::Bold);
	nameLabel_.setFont(font);
	nameLabel_.setTextInteractionFlags(Qt::TextEditorInteraction);

	// set the fonts for the corner labels
	QFont cornerFont = leftTopText_.font();
	cornerFont.setPointSize(cornerFont.pointSize() - 1);
	leftTopText_.setFont(cornerFont);
	rightTopText_.setFont(cornerFont);

	connect(nameLabel_.document(), SIGNAL(contentsChanged()), 
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

	MemoryVisualizerItem::reorganizeChildren();
}

MemoryVisualizerItem::~MemoryVisualizerItem() {
}

QRectF MemoryVisualizerItem::minimumRect() const {
	return QRectF(0, 0, MemoryVisualizerItem::MAX_ITEM_WIDTH, 
		MemoryVisualizerItem::ITEM_HEIGHT);
}

QRectF MemoryVisualizerItem::itemTotalRect() const {
	QRectF totalRect(rect());

	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {
		
		// if the child item is one of the memory visualizer items
		MemoryVisualizerItem* childMemItem = dynamic_cast<MemoryVisualizerItem*>(child);
		if (childMemItem) {
			totalRect = totalRect.united(mapRectFromItem(child, childMemItem->itemTotalRect()));
		}
	}

	return totalRect;
}

void MemoryVisualizerItem::reorganizeChildren() {

	// center the text to the middle of the item
	nameLabel_.setPos(rect().center().x() - (nameLabel_.boundingRect().width() / 2), 0);

	// set the left corner text position
	leftTopText_.setPos(2, 0);

	// set the right corner text position
	rightTopText_.setPos(rect().topRight().x() - rightTopText_.boundingRect().width(), 0);
}

qreal MemoryVisualizerItem::itemTotalWidth() const {
	qreal width = rect().width();

	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		MemoryVisualizerItem* childMemItem = dynamic_cast<MemoryVisualizerItem*>(child);
		if (childMemItem) {
			width = qMax(width, childMemItem->itemTotalWidth());
		}
	}

	return width;
}

void MemoryVisualizerItem::setWidth( qreal width ) {
	setRect(0, 0, width, ITEM_HEIGHT);
	MemoryVisualizerItem::reorganizeChildren();
}


void MemoryVisualizerItem::setName( const QString& name ) {

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

void MemoryVisualizerItem::setLeftCornerText( const QString& text ) {
	leftTopText_.setText(text);
	// set the left corner text position
	leftTopText_.setPos(2, 0);
}

void MemoryVisualizerItem::setRightCornerText( const QString& text ) {
	rightTopText_.setText(text);
	// set the right corner text position
	rightTopText_.setPos(rect().topRight().x() - rightTopText_.boundingRect().width(), 0);
}

int MemoryVisualizerItem::getOffset() const {
	return 0;
}

int MemoryVisualizerItem::getWidth() const {
	return maxWidth_;
}

