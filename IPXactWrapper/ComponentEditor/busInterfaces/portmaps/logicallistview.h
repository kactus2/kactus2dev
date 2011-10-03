/* 
 *  	Created on: 7.7.2011
 *      Author: Antti Kamppi
 * 		filename: logicallistview.h
 *		Project: Kactus 2
 */

#ifndef LOGICALLISTVIEW_H
#define LOGICALLISTVIEW_H

#include "portlistview.h"

#include <QDropEvent>

class LogicalListView : public PortListView {
	Q_OBJECT

public:

	LogicalListView(QWidget *parent);

	//! \brief The destructor
	virtual ~LogicalListView();

protected:

	/*! \brief Handler for drop events on drag & drop.
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);

private:
	//! \brief No copying
	LogicalListView(const LogicalListView& other);

	//! \brief No assignment
	LogicalListView& operator=(const LogicalListView& other);
};

#endif // LOGICALLISTVIEW_H
