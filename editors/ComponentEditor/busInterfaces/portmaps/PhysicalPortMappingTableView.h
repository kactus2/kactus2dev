//-----------------------------------------------------------------------------
// File: PhysicalPortMappingTableView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Table view for physical ports.
//-----------------------------------------------------------------------------

#ifndef PHYSICALPORTMAPPINGTABLEVIEW_H
#define PHYSICALPORTMAPPINGTABLEVIEW_H

#include "PortMappingTableView.h"

#include <QDropEvent>

//-----------------------------------------------------------------------------
//! Table view for physical ports.
//-----------------------------------------------------------------------------
class PhysicalPortMappingTableView : public PortMappingTableView
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this view.
     */
	PhysicalPortMappingTableView(QWidget *parent);

	/*!
     *  The destructor.
     */
    virtual ~PhysicalPortMappingTableView();

public slots:

    /*!
     *  Handles changes in the name filter by selecting the best matching port from the list.
     *
     *      @param [in] portName   Name given in filter.
     */
    virtual void onFilterNameChanged(QString const& portName);

protected:

	/*
     *  Handler for drop events on drag & drop.
	 *
	 *      @param [in] event   Pointer to the event object.
	 */
	virtual void dropEvent(QDropEvent* event);

    /*!
     *  Handler for mouse move events.
	 *
	 *      @param [in] event   Pointer to the event object.
	 */
    void mouseMoveEvent(QMouseEvent* event);
	
    /*!
     *  Handler for mouse press events.
	 *
	 *      @param [in] event   Pointer to the event object.
	 */
    void mousePressEvent(QMouseEvent* event);

private:

	// No copying.
	PhysicalPortMappingTableView(const PhysicalPortMappingTableView& other);
	PhysicalPortMappingTableView& operator=(const PhysicalPortMappingTableView& other);
};

#endif // PHYSICALPORTMAPPINGTABLEVIEW_H
