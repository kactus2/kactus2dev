/* 
 *  Created on: 21.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsview.h
 */

#ifndef BUSPORTSVIEW_H
#define BUSPORTSVIEW_H

#include <QTableView>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QModelIndexList>
#include <QAction>

/*! \brief The view that displays the contents of the BusPortsModel
 *
 */
class BusPortsView : public QTableView {
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

	//! \brief The given items should be removed from the model
        void removeItems(const QModelIndexList& indexes);

	//! \brief The given items should be copied in the model
        void copyItems(const QModelIndexList& indexes);

protected:

	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* e);

private slots:

	//! \brief Handler for remove action triggers
	void onRemove();

	//! \brief Handler for copy action triggers
	void onCopy();

private:
	//! \brief No copying
	BusPortsView(const BusPortsView& other);

	//! \brief No assignment
	BusPortsView& operator=(const BusPortsView& other);	

	//! \brief Set up the actions and connect their handlers
	void setupActions();

	//! \brief Action to remove all selected indexes.
	QAction removeAction_;

	//! \brief Action to copy all selected indexes.
	QAction copyAction_;
};

#endif // BUSPORTSVIEW_H
