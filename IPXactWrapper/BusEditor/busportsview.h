/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsview.h
 */

#ifndef BUSPORTSVIEW_H
#define BUSPORTSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QTableView>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QModelIndexList>
#include <QAction>

/*! \brief The view that displays the contents of the BusPortsModel
 *
 */
class BusPortsView : public EditableTableView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	BusPortsView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusPortsView();

	/*! \brief Get list of currently selected indexes.
	 *
	 * \return QModelIndexList contains the model indexes of the selected items.
	*/
	QModelIndexList selected() const;

signals:

	//! \brief 
    void addSignalOptions();

protected:

	//! \brief Handler for key press events
	//virtual void keyPressEvent(QKeyEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* e);

    QAction addOptionsAction_;

private:
	//! \brief No copying
	BusPortsView(const BusPortsView& other);

	//! \brief No assignment
	BusPortsView& operator=(const BusPortsView& other);	

	//! \brief Set up the actions and connect their handlers
	void setupActions();

	//! \brief Action to remove all selected indexes.
	//QAction removeAction_;

	//! \brief Action to copy all selected indexes.
	//QAction copyAction_;
};

#endif // BUSPORTSVIEW_H
