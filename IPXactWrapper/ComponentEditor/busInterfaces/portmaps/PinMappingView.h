//-----------------------------------------------------------------------------
// File: PinMappingView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// View for bit-field mapping.
//-----------------------------------------------------------------------------

#ifndef PINMAPPINGVIEW_H
#define PINMAPPINGVIEW_H

#include <QDropEvent>
#include <QTableView>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>


//-----------------------------------------------------------------------------
// class PinMappingView.
//-----------------------------------------------------------------------------
class PinMappingView : public QTableView {
	Q_OBJECT

public:

	//! The minimum height for the view.
	static const int MINIMUM_TABLE_HEIGHT = 100;


	/*! The constructor
	 *
	 *  @param parent Pointer to the owner of this view.
	 *
	 */
	PinMappingView(QWidget *parent);
	
	//! The destructor
	virtual ~PinMappingView();


    /*!
     *  Sets the source/proxy model for the view.
     *
     *      @param [in] model   The model to set.
     *
     */
	virtual void setModel(QAbstractItemModel* model);

protected:


	//! Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! Handler for mouse press events
	virtual void mousePressEvent(QMouseEvent* event);

	//! Handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent* event);

	//! Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

    /*!
     * Handler for drop enter events. Discards any event not sourced from physical port list view.
     *
     *      @param [in] event   The drop event.
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);

	//! The point where mouse was clicked
	QPoint pressedPoint_;

	//! Action to clear selected cells
	QAction clearAction_;

protected slots:


	//! Handler for cell clear action.
	virtual void onClearAction(); 

private:

	//! No copying
	PinMappingView(const PinMappingView& other);

	//! No assignment
	PinMappingView& operator=(const PinMappingView& other);

	//! Set up the actions for the context menu.
	void setupActions();

};

#endif // PINMAPPINGVIEW_H
