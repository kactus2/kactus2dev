/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapview.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPVIEW_H
#define MEMORYMAPVIEW_H

#include <QGraphicsView>

/*! \brief The graphics view that displays the memory map graphics items.
 *
 */
class MemoryMapView : public QGraphicsView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent The pointer to the owner of the graphics view.
	 *
	*/
	MemoryMapView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapView();

protected:

    //! \brief Event handler for resizing.
    virtual void resizeEvent(QResizeEvent* event);

private:
	
	//! \brief No copying
	MemoryMapView(const MemoryMapView& other);

	//! \brief No assignment
	MemoryMapView& operator=(const MemoryMapView& other);
};

#endif // MEMORYMAPVIEW_H
