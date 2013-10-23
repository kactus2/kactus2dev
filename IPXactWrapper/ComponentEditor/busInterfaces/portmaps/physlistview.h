//-----------------------------------------------------------------------------
// File: physlistview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.7.2011
//
// Description:
// List view for physical ports.
//-----------------------------------------------------------------------------

#ifndef PHYSLISTVIEW_H
#define PHYSLISTVIEW_H

#include "portlistview.h"

#include <QDropEvent>


class PhysListView : public PortListView {
	Q_OBJECT

public:

    // The constructor.
	PhysListView(QWidget *parent);

	// The destructor.
	virtual ~PhysListView();

public slots:

    /*!
     *  Handles changes in the name filter by selecting the best matching port from the list.
     *
     *      @param [in] portName   Name given in filter.
     */
    virtual void onFilterNameChanged(QString const& portName);

protected:

	/* Handler for drop events on drag & drop.
	 *
	 *      @param event Pointer to the event object.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);
	
private:
	// No copying
	PhysListView(const PhysListView& other);

	// No assignment
	PhysListView& operator=(const PhysListView& other);
};

#endif // PHYSLISTVIEW_H
