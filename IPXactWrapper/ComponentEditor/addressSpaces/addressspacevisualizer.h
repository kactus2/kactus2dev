/* 
 *  	Created on: 29.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacevisualizer.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEVISUALIZER_H
#define ADDRESSSPACEVISUALIZER_H

#include <models/segment.h>

#include <QWidget>
#include <QSharedPointer>
#include <QPaintEvent>
#include <QString>

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
	 * \param segment
	 *
	 * \return void
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

	/*! \brief Draw the grid that specifies the memory blocks.
	 *
	 * \param painter The painter to use for drawing.
	 * \param bounds The rectangle that limits the grid.
	 *
	*/
	void drawGrid(QPainter& painter, const QRect& bounds);

	//! \brief The number of bits in one byte.
	int byteSize_;

	//! \brief The number of bits on one row.
	int rowWidth_;

	//! \brief The number of bytes in the address space.
	quint64 byteCount_;
};

#endif // ADDRESSSPACEVISUALIZER_H
