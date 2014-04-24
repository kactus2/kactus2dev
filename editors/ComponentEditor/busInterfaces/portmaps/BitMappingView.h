//-----------------------------------------------------------------------------
// File: BitMappingView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// View for bit-field mapping.
//-----------------------------------------------------------------------------

#ifndef BITMAPPINGVIEW_H
#define BITMAPPINGVIEW_H

#include <IPXACTmodels/component.h>

#include <QDropEvent>
#include <QTableView>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>


//-----------------------------------------------------------------------------
// class BitMappingView.
//-----------------------------------------------------------------------------
class BitMappingView : public QTableView {
	Q_OBJECT

public:

	//! The minimum height for the view.
	static const int MINIMUM_TABLE_HEIGHT = 100;

	/*! The constructor
	 *
     *  @param [in] component   The component whose ports are edited.
	 *  @param [in] parent      Pointer to the owner of this view.
	 *
	 */
	BitMappingView(QSharedPointer<Component> component, QWidget *parent);
	
	//! The destructor
	virtual ~BitMappingView();

    /*!
     *  Sets the source/proxy model for the view.
     *
     *      @param [in] model   The model to set.
     *
     */
	virtual void setModel(QAbstractItemModel* model);

public slots:

    /*!
     *  Called when the selected logical port changes.
     *
     *      @param [in] portName   Name of the new selected logical port.
     */
    virtual void onLogicalPortChanged(QString const& portName);

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

    /*!
     *  Handler for drop event. Either opens a menu on right-button drag or drops the 
     *  data to the model on left-button drag.
     *
     *      @param [in] event   The drop event.
     */
    virtual void dropEvent(QDropEvent *event);

	//! The point where mouse was clicked
	QPoint pressedPoint_;

protected slots:

	//! Handler for cell clear action.
	virtual void onClearAction(); 

    //! Handler for select bits action.
    virtual void onSelectBitsAction();

private:

	//! No copying
	BitMappingView(const BitMappingView& other);

	//! No assignment
	BitMappingView& operator=(const BitMappingView& other);

	//! Set up the actions for the context menu.
	void setupActions();

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! The point where drag-drop ended.
    QPoint dropPoint_;

    //! The data drag-dropped onto the view.
    QMimeData const* droppedData_;

    //! Action to clear selected cells.
    QAction clearAction_;

    //! Action to select bits to connect.
    QAction selectBitsAction_;

    QString logicalPort_;
};

#endif // BITMAPPINGVIEW_H
