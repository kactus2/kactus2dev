/* 
 *  	Created on: 7.7.2011
 *      Author: Antti Kamppi
 * 		filename: physlistview.h
 *		Project: Kactus 2
 */

#ifndef PHYSLISTVIEW_H
#define PHYSLISTVIEW_H

#include "portlistview.h"

#include <QDropEvent>

class PhysListView : public PortListView {
	Q_OBJECT

public:

	PhysListView(QWidget *parent);

	//! \brief The destructor
	virtual ~PhysListView();

protected:

	/*! \brief Handler for drop events on drag & drop.
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);
	
private:
	//! \brief No copying
	PhysListView(const PhysListView& other);

	//! \brief No assignment
	PhysListView& operator=(const PhysListView& other);
};

#endif // PHYSLISTVIEW_H
