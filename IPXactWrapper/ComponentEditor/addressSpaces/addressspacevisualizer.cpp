/* 
 *  	Created on: 29.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.cpp
 *		Project: Kactus 2
 */

#include "addressspacevisualizer.h"

#include <common/utils.h>

#include <QColor>
#include <QPainter>
#include <QLinearGradient>
#include <QRect>
#include <QPoint>
#include <QPointF>
#include <QApplication>
#include <QPalette>
#include <QBrush>
#include <QPen>

AddressSpaceVisualizer::AddressSpaceVisualizer(QWidget *parent):
QWidget(parent),
byteSize_(8), 
rowWidth_(8),
byteCount_(1) {

	setMinimumSize(400, 200);
}

AddressSpaceVisualizer::~AddressSpaceVisualizer() {
}

void AddressSpaceVisualizer::setByteSize( int bitCount ) {
	Q_ASSERT(bitCount > 0);
	byteSize_ = bitCount;
	update();
}

void AddressSpaceVisualizer::setRowWidth( int bitCount ) {
	rowWidth_ = bitCount;
	update();
}

void AddressSpaceVisualizer::setRange(quint64 byteCount ) {
	byteCount_ = byteCount;
	update();
}

void AddressSpaceVisualizer::setRange( const QString& range ) {
	byteCount_ = Utils::str2Int(range);
	update();
}

void AddressSpaceVisualizer::paintEvent( QPaintEvent* event ) {
	
	// first call the base class implementation to draw widget
	QWidget::paintEvent(event);

	QPalette palette = QApplication::palette();

	// create painter instance and initialize it
	QPainter painter(this);
	//painter.setBrush(brush_);
	painter.setPen(QPen(QColor("black")));

	// the bounds to draw
	QRect bounds = rect();
	bounds.adjust(0, 0, -2, -2);

	QLinearGradient fillGradient(bounds.topLeft(), bounds.bottomRight());
	
	// the top color for gradient
	fillGradient.setColorAt(0, palette.color(QPalette::Disabled, QPalette::Highlight));

	// the bottom color for gradient
	fillGradient.setColorAt(1, palette.color(QPalette::Active, QPalette::Highlight));
	QBrush brush(fillGradient);

	//brush.setColor(QColor("cornflowerblue"));
	painter.setBrush(brush);

	painter.drawRect(bounds);

	drawGrid(painter, bounds);
}

void AddressSpaceVisualizer::drawGrid( QPainter& painter, const QRect& bounds) {
	if (byteSize_ == 0) {
		return;
	}

	// find the bounds for the drawing
	int leftBound = bounds.topLeft().x();
	int rightBound = bounds.topRight().x();
	int topBound = bounds.topLeft().y();
	int bottomBound = bounds.bottomLeft().y();

	// first draw the vertical lines

	// the number of columns to draw
	int columnCount = rowWidth_ / byteSize_;
	if (columnCount <= 0) {
		return;
	}

	double increaseStep = (rightBound - leftBound) / double(columnCount);
	double currentPos = leftBound;
	
	// draw the right amount of columns
	for (int currentColumn = 0; currentColumn < columnCount -1; ++currentColumn) {

		currentPos += increaseStep;

		// draw the vertical line from top to bottom
		QPointF top(currentPos, topBound);
		QPointF bottom(currentPos, bottomBound);
		painter.drawLine(top, bottom);
	}

	// draw the right number of rows

// 	if (rowWidth_ <= 0) {
// 		return;
// 	}
// 	
// 	// number of rows to draw
// 	int rowCount = (byteCount_ * byteSize_) / rowWidth_;
// 	if (rowCount <= 0) {
// 		return;
// 	}
// 
// 	increaseStep = (bottomBound - topBound) / double(rowCount);
// 	currentPos = topBound;
// 
// 	// draw the right amount of rows
// 	for (int currentRow = 0; currentRow < rowCount - 1; ++currentRow) {
// 
// 		currentPos += increaseStep;
// 
// 		// draw the horizontal line from left to right
// 		QPointF left(leftBound, currentPos);
// 		QPointF right(rightBound, currentPos);
// 		painter.drawLine(left, right);
// 	}
}

void AddressSpaceVisualizer::addSegment( QSharedPointer<Segment> segment ) {

}

void AddressSpaceVisualizer::removeSegment( const QString& segmentName ) {

}

void AddressSpaceVisualizer::renameSegment( const QString& oldName, const QString& newName ) {

}

void AddressSpaceVisualizer::updateSegment( QSharedPointer<Segment> segment ) {

}
