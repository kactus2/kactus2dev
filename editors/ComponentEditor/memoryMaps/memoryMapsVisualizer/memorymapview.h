//-----------------------------------------------------------------------------
// File: memorymapview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.04.2012
//
// Description:
// The graphics view that displays the memory map graphics items.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPVIEW_H
#define MEMORYMAPVIEW_H

#include <QGraphicsView>

//-----------------------------------------------------------------------------
//! The graphics view that displays the memory map graphics items.
//-----------------------------------------------------------------------------
class MemoryMapView : public QGraphicsView
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  The pointer to the owner of the graphics view.
	 */
	MemoryMapView(QWidget *parent);
	
	//! The destructor.
	virtual ~MemoryMapView();

protected:

    //! Event handler for resizing.
    virtual void resizeEvent(QResizeEvent* event);

    //! Handler for mouse press events.
    void mousePressEvent(QMouseEvent* mouseEvent);

private:
	
	//! No copying.
	MemoryMapView(const MemoryMapView& other);

	//! No assignment.
	MemoryMapView& operator=(const MemoryMapView& other);
};

#endif // MEMORYMAPVIEW_H
