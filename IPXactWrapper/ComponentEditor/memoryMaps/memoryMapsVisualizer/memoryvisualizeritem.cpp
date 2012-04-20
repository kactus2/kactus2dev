/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizeritem.cpp
 *		Project: Kactus 2
 */

#include "memoryvisualizeritem.h"

#include <QFont>
#include <QTextDocument>

MemoryVisualizerItem::MemoryVisualizerItem(QGraphicsItem *parent): 
QGraphicsRectItem(parent),
QObject(),
nameLabel_(this) {

	setRect(pos().x(), pos().y(), MemoryVisualizerItem::MAX_ITEM_WIDTH, 
		MemoryVisualizerItem::ITEM_HEIGHT);

	QFont font = nameLabel_.font();
	font.setWeight(QFont::Bold);
	nameLabel_.setFont(font);
	nameLabel_.setPos(rect().center().x() - (nameLabel_.boundingRect().width() / 2), this->pos().y());
	nameLabel_.setTextInteractionFlags(Qt::TextEditorInteraction);

	connect(nameLabel_.document(), SIGNAL(contentsChanged()), 
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

MemoryVisualizerItem::~MemoryVisualizerItem() {
}

void MemoryVisualizerItem::setName( const QString& name ) {
	nameLabel_.setPlainText(name);
	nameLabel_.setPos(rect().center().x() - (nameLabel_.boundingRect().width() / 2), this->pos().y());
}

