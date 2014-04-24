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

    //! The constructor.
	LogicalListView(QWidget *parent);

	//! The destructor.
	virtual ~LogicalListView();    

public slots:

    /*!
     *  Called when a port is restored from the port maps.
     *
     *      @param [in] portName   The name of the restored port.
     */
    void onPortRestored(QString const& portName);

    /*!
     *  Called when a port is removed from the port map.
     *
     */
    void onPortRemoved();

protected:

	/*! Handler for drop events on drag & drop.
	 *
	 * \param event Pointer to the event object.
	 *
	*/
	virtual void dropEvent(QDropEvent* event);


    virtual void mousePressEvent(QMouseEvent* event);


private:
	//! No copying
	LogicalListView(const LogicalListView& other);

	//! No assignment
	LogicalListView& operator=(const LogicalListView& other);

    //! The last index selected.
    QModelIndex selectedIndex_;
};

#endif // LOGICALLISTVIEW_H
