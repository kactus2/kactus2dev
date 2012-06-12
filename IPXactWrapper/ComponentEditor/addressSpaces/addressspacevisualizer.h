/* 
 *  	Created on: 29.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEVISUALIZER_H
#define ADDRESSSPACEVISUALIZER_H

#include <models/segment.h>
#include <models/addressspace.h>

#include <QWidget>
#include <QSharedPointer>
#include <QPaintEvent>
#include <QString>
#include <QList>
#include <QColor>

/*! \brief A visualizer class to draw the address space being edited.
 *
 */
class AddressSpaceVisualizer : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of the visualizer.
	 *
	*/
	AddressSpaceVisualizer(QWidget *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceVisualizer();

	/*! \brief Set the segments that are to be displayed.
	 *
	 * \param addrSpace Pointer to the address space that contains the segments.
	 *
	*/
	void setSegments(QSharedPointer<AddressSpace> addrSpace);

public slots:

	/*! \brief Set the size of one byte in bits.
	 *
	 * \param bitCount Number of bits one byte contains.
	 *
	*/
	void setByteSize(int bitCount);

	/*! \brief Set how many bits one row contains.
	 *
	 * \param bitCount Number of bits on one row.
	 *
	*/
	void setRowWidth(int bitCount);

	/*! \brief Set the number of bytes the address space contains.
	 *
	 * \param byteCount Number of bytes in the address space.
	 *
	*/
	void setRange(quint64 byteCount);

	/*! \brief Set the number of bytes the address space contains.
	 *
	 * \param byteCount Number of bytes in the address space.
	 *
	*/
	void setRange(const QString& range);

	/*! \brief Draw a new segment.
	 *
	 * \param segment Pointer to the segment which specifies the name, range and offset.
	 *
	*/
	void addSegment(QSharedPointer<Segment> segment);

	/*! \brief Remove the segment from the visualization.
	 *
	 * \param segmentName The name of the segment to draw.
	 *
	*/
	void removeSegment(const QString& segmentName);

	/*! \brief Rename a segment.
	 *
	 * \param oldName The old name to identify the segment.
	 * \param newName The new name of a segment.
	 *
	*/
	void renameSegment(const QString& oldName, const QString& newName);

	/*! \brief Update the segment 
	 *
	 * \param segment Pointer to the segment that's range or/and offset has changed.
	 *
	*/
	void updateSegment(QSharedPointer<Segment> segment);

protected:

	//! \brief The paint event handler.
	virtual void paintEvent(QPaintEvent* event);

private:
	//! \brief No copying
	AddressSpaceVisualizer(const AddressSpaceVisualizer& other);

	//! \brief No assignment
	AddressSpaceVisualizer& operator=(const AddressSpaceVisualizer& other);

	/*! \brief Draw the given text with painter so text center is in given position.
	 *
	 * \param painter The painter to use for drawing.
	 * \param text The text to draw.
	 * \param textCenterPoint The point where text center is to be located.
	 *
	*/
	void drawCenteredText(QPainter& painter,
		const QString& text,
		const QPoint& textCenterPoint);

	/*! \brief Draw the headers for the address space.
	 *
	 * \param painter The painter instance to use for drawing.
	 *
	 * \return QRect The area that is taken by the headers.
	*/
	QRect drawHeaders(QPainter& painter);

	/*! \brief Draw a horizontal line that shows size limit |--------|
	 *
	 * \param painter The painter instance to use.
	 * \param bounds The bounds of the line.
	 *
	*/
	void drawHorizontalLimiter(QPainter& painter, const QRect& bounds, const QString& legend);

	/*! \brief Draw the grid that specifies the memory blocks.
	 *
	 * \param painter The painter to use for drawing.
	 * \param bounds The rectangle that limits the grid.
	 *
	*/
	void drawGrid(QPainter& painter, const QRect& bounds);

	/*! \brief Draw the segments to the visualization.
	 *
	 * \param painter The painter to use to paint the segments.
	 * \param bounds The bounds for the drawing.
	 * \param columnCount The number of columns in the address space.
	 * \param rowCount The number of rows in the address space.
	 *
	*/
	void drawSegments(QPainter& painter, const QRect& bounds, quint64 columnCount, quint64 rowCount);

	/*! \brief Get the rectangle that matches the area reserved by given address range.
	 *
	 * \param bounds Specifies the bounds that limit the address space.
	 * \param startAddress The starting address of the segment.
	 * \param endAddress The end address of the segment. This address is the last
	 * address within the segment.
	 * \param columnCount The number of columns the address space contains.
	 * \param rowCount The number of rows the address space contains.
	 *
	 * \return QRect The rectangle that limits the area of the segment.
	*/
	QRect segmentArea(const QRect& bounds, quint64 startAddress, quint64 endAddress, quint64 columnCount, quint64 rowCount) const;

	//! \brief Represents the 
	struct Area {
		//! \brief The name of the area.
		QString name_;

		//! \brief The offset from the start of the address space.
		quint64 offSet_;

		//! \brief The range of the area.
		quint64 range_;

		//! \brief The constructor
		Area(const QString& name, quint64 offset, quint64 range);

		//! \brief Copy constructor
		Area(const Area& other);

		//! \brief Assignment operator
		Area& operator=(const Area& other);

		//! \brief The < operator
		bool operator<(const Area& other) const;
	};

	//! \brief The number of bits in one byte.
	int byteSize_;

	//! \brief The number of bits on one row.
	int rowWidth_;

	//! \brief The number of bytes in the address space.
	quint64 byteCount_;

	//! \brief Contains the segments to display.
	QList<Area> segments_;
};

#endif // ADDRESSSPACEVISUALIZER_H
