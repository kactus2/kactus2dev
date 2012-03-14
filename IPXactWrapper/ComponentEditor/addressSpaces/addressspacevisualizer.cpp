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
#include <QRect>
#include <QPoint>
#include <QPointF>
#include <QApplication>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QFontMetrics>

#include <QDebug>

AddressSpaceVisualizer::AddressSpaceVisualizer(QWidget *parent):
QWidget(parent),
byteSize_(8), 
rowWidth_(8),
byteCount_(1),
segments_() {

	setMinimumSize(400, 200);
}

AddressSpaceVisualizer::~AddressSpaceVisualizer() {
}

void AddressSpaceVisualizer::setSegments( AddressSpace* addrSpace ) {
	QList<QSharedPointer<Segment> > segmentList =  addrSpace->getSegments();

	foreach (QSharedPointer<Segment> segment, segmentList) {
		// create new area for the segment
		AddressSpaceVisualizer::Area newArea(segment->getName(),
			Utils::str2Int(segment->getAddressOffset()), 
			Utils::str2Int(segment->getRange()));
		segments_.append(newArea);
	}

	qSort(segments_);
	update();
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

	// the pen instance to use
	QPen pen(QColor("black"));
	pen.setWidth(1);

	painter.setPen(pen);

	// the bounds to draw
	QRect bounds = rect();
	bounds.adjust(0, 0, -2, -2);

	// the brush that uses the gradient
	QBrush brush(QColor("white"));
	painter.setBrush(brush);

	// draw the base of the address space
	painter.drawRect(bounds);

	// after the base has been drawn then draw the segments and the vertical column lines
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
	quint64 columnCount = rowWidth_ / byteSize_;
	if (columnCount <= 0) {
		return;
	}

	// calculate the number of rows the address space contains

	if (rowWidth_ <= 0) {
		return;
	}

	// number of rows to draw
	quint64 rowCount = (byteCount_ * byteSize_) / rowWidth_;
	Q_ASSERT(rowCount >= 0);
	if (rowCount <= 0) {
		return;
	}

	// draw the rectangles for the segments
	drawSegments(painter, bounds, columnCount, rowCount);

	QPen pen(QColor("black"));
	painter.setPen(pen);

	// draw the horizontal grid lines

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

void AddressSpaceVisualizer::drawSegments( QPainter& painter, const QRect& bounds, 
										  quint64 columnCount, quint64 rowCount ) {

	Q_ASSERT(columnCount > 0);
	Q_ASSERT(rowCount > 0);

	// the brush and pen to use for overlapping segment areas
	QBrush errorBrush(QColor("orangered"));
	QPen errorPen(QColor("orangered"));

	// the pen to use to write text
	QPen textPen(QColor("black"));
	painter.setPen(textPen);

	// the height and width of the fonts
	int fontHeight = painter.fontMetrics().height();
	int fontWidth = painter.fontMetrics().lineWidth();

	// the address where the previous segment ends.
	// used to check if the segments are overlapping each other.
	// only the previous segment needs to be checked because the segments
	// are sorted in order by offset
	quint64 previousLimit = 0;

	// the rectangle of the previous segment
	QRect previousRect;

	// the name of the previous segment
	QString previousName;

	// used to change the color after each segment
	int colorStep = 150;

	for (int i = 0; i < segments_.size(); ++i) {

		AddressSpaceVisualizer::Area& segment = segments_[i];

		if (colorStep < 50) {
			colorStep = 150;
		}

		// the color to use for the segment
		QColor color(colorStep - 50, colorStep + 50, 255);

		// the brush to use inside the segmentRect
		QBrush brush(color);
		painter.setBrush(brush);

		// make sure not to use the same color again in next segment
		colorStep -= 20;

		// get the rectangle that limits the segment area
		QRect segmentRect = segmentArea(bounds, segment.offSet_, segment.offSet_ + segment.range_,
			columnCount, rowCount);

		// make sure the segment rect does not go over the limit of the bounds
		if (segmentRect.bottomLeft().y() > bounds.bottomLeft().y() ||
			segmentRect.bottomRight().y() > bounds.bottomRight().y()) {

			segmentRect.setBottomLeft(bounds.bottomLeft());
			segmentRect.setBottomRight(bounds.bottomRight());
		}

		// draw the segmentRect so that it never exceeds the limit of the bounds
		painter.drawRect(segmentRect);

		// if this is not the first segment and it intersects with the previous segment
		if (previousRect.isValid() && previousLimit >= segment.offSet_) {

			painter.setBrush(errorBrush);

			// the segmentRect that overlaps
			QRect errorArea = previousRect.intersected(segmentRect);

			// if the segment is too small then use the error pen to make sure 
			// the overlapping is visible
			if (errorArea.height() < 3) {
				painter.setPen(errorPen);
			}

			// draw the overlapping area
			painter.drawRect(errorArea);

			// restore back to normal pen
			painter.setPen(textPen);

			// draw the text that tells where the previous area ends
			QPoint previousEnd(errorArea.bottomLeft().x() + fontWidth, errorArea.bottomLeft().y());
			QString prevEndStr = QString::number(previousLimit, 16).toUpper();
			prevEndStr.prepend("0x");
			painter.drawText(previousEnd, prevEndStr);

			// if the name of the previous segment was drawn over by the new segment
			if (errorArea.contains(previousRect.center())) {
				QPoint oldNamePoint(errorArea.center().x(), errorArea.top());
				painter.drawText(oldNamePoint, previousName);
			}

			painter.setBrush(brush);
		}

		// if the segments don't overlap by addresses
		// if the rectangles intersect then the previous limit has to be redrawn 
		// because the new rectangle was drawn on top of it.
		else if (previousRect.isValid() && previousRect.intersects(segmentRect)) {
			
			// draw the text that tells where the previous area ends
			QPoint previousEnd(segmentRect.topLeft().x() + fontWidth, segmentRect.topLeft().y());
			QString prevEndStr = QString::number(previousLimit, 16).toUpper();
			prevEndStr.prepend("0x");
			painter.drawText(previousEnd, prevEndStr);

			// if the name of the previous segment was drawn over by the new segment
			if (segmentRect.contains(previousRect.center())) {
				QRect intersectingRect = segmentRect.intersected(previousRect);
				QPoint oldNamePoint(intersectingRect.center().x(), intersectingRect.top());
				painter.drawText(oldNamePoint, previousName);
			}
		}

		// update the previous limit for the next round
		previousLimit = segment.offSet_ + segment.range_ - 1;

		// draw the name of the segment to the center of the segment
		QPoint centerPoint = segmentRect.center();
		painter.drawText(centerPoint, segment.name_);

		// update the name for the next round
		previousName = segment.name_;

		// draw the starting address to the start of the segment
		QPoint startPoint = QPoint(segmentRect.topLeft().x() + fontWidth, segmentRect.topLeft().y() + fontHeight);
		QString startStr = QString::number(segment.offSet_, 16).toUpper();
		startStr.prepend("0x");
		painter.drawText(startPoint, startStr);

		// draw the end address to the end of the segment
		QPoint endPoint = QPoint(segmentRect.bottomLeft().x() + fontWidth, segmentRect.bottomLeft().y());
		QString endStr = QString::number(segment.offSet_ + segment.range_ - 1, 16).toUpper();
		endStr.prepend("0x");
		painter.drawText(endPoint, endStr);

		// if the segment goes further than the limit of the address space
		if (previousLimit > byteCount_ - 1) {
			errorPen.setWidth(2);
			painter.setPen(errorPen);
			painter.drawLine(bounds.bottomLeft(), bounds.bottomRight());
			painter.setPen(textPen);
			errorPen.setWidth(1);
		}

		// the segment rectangle becomes the previous rectangle for the next segment
		previousRect = segmentRect;
	}
}

QRect AddressSpaceVisualizer::segmentArea( const QRect& bounds,
														quint64 startAddress, 
														quint64 endAddress, 
														quint64 columnCount, 
														quint64 rowCount ) const {

	Q_ASSERT(columnCount > 0);
	Q_ASSERT(rowCount > 0);

	// total number of pixels divided by the number of rows there are 
	double rowHeight = bounds.bottomRight().y() / double(rowCount);

	// calculate the row numbers of the limits
	quint64 startRow = startAddress / columnCount;
	quint64 endRow = endAddress / columnCount;

	// The heights of the limits of the segmentRect
	quint64 startHeight = startRow * rowHeight;
	quint64 endHeight = (endRow + 1) * rowHeight;

	// the top left and bottom right corners that define the segmentRect
	QPoint topLeft(bounds.left(), startHeight);
	QPoint bottomRight(bounds.right(), endHeight);

	return QRect(topLeft, bottomRight);
}

void AddressSpaceVisualizer::addSegment( QSharedPointer<Segment> segment ) {

	// create new area for the segment
	AddressSpaceVisualizer::Area newArea(segment->getName(),
		Utils::str2Int(segment->getAddressOffset()), 
		Utils::str2Int(segment->getRange()));
	segments_.append(newArea);
	
	// sort the list to make sure the new area is in correct place
	qSort(segments_);

	update();
}

void AddressSpaceVisualizer::removeSegment( const QString& segmentName ) {
	
	for (int i = 0; i < segments_.size(); ++i) {
		
		// if the area has the specified name
		if (segments_.at(i).name_ == segmentName) {
			segments_.removeAt(i);
			update();
			return;
		}
	}
}

void AddressSpaceVisualizer::renameSegment( const QString& oldName, const QString& newName ) {
	
	for (int i = 0; i < segments_.size(); ++i) {

		// find the segment with the given name
		if (segments_.at(i).name_ == oldName) {
			
			// change the name to new name
			segments_[i].name_ = newName;
			update();
			return;
		}
	}
}

void AddressSpaceVisualizer::updateSegment( QSharedPointer<Segment> segment ) {

	for (int i = 0; i < segments_.size(); ++i) {

		if (segments_.at(i).name_ == segment->getName()) {

			segments_[i].offSet_ = Utils::str2Int(segment->getAddressOffset());
			segments_[i].range_ = Utils::str2Int(segment->getRange());
			
			qSort(segments_);
			update();
			return;
		}
	}
}

AddressSpaceVisualizer::Area::Area( const QString& name, quint64 offset, quint64 range ):
name_(name), 
offSet_(offset),
range_(range) {
}

AddressSpaceVisualizer::Area::Area( const AddressSpaceVisualizer::Area& other ):
name_(other.name_),
offSet_(other.offSet_),
range_(other.range_) {
}

AddressSpaceVisualizer::Area& AddressSpaceVisualizer::Area::operator=(
	const AddressSpaceVisualizer::Area& other ) {

	if (this != &other) {
		name_ = other.name_;
		offSet_ = other.offSet_;
		range_ = other.range_;
	}
	return *this;
}

bool AddressSpaceVisualizer::Area::operator<( const AddressSpaceVisualizer::Area& other ) const {
	return offSet_ < other.offSet_;
}
