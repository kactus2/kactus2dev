//-----------------------------------------------------------------------------
// File: CellEditTableView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 4.6.2013
//
// Description:
// View for editing tables one cell at a time.
//-----------------------------------------------------------------------------
#ifndef CELLEDITTABLEVIEW_H
#define CELLEDITTABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>

//-----------------------------------------------------------------------------
//! CellEditTableView class.
//-----------------------------------------------------------------------------
class CellEditTableView : public QTableView {
	Q_OBJECT

public:

	//! \brief The minimum height for the view.
	static const int MINIMUM_TABLE_HEIGHT = 120;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this view.
	 *
	*/
	CellEditTableView(QWidget* parent);
	
	//! \brief The destructor
	virtual ~CellEditTableView();

	/*! \brief Set the model class for this view.
	 *
	 * @param [in] model Pointer to the model class which is displayed by this view.
	*/
    virtual void setModel( QAbstractItemModel* model );

protected:


	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! \brief Handler for mouse press events
	virtual void mousePressEvent(QMouseEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

	//! \brief The point where mouse was clicked
	QPoint pressedPoint_;

	//! \brief Action to copy item
	QAction copyAction_;

	//! \brief Action to paste item
	QAction pasteAction_;

	//! \brief Action to clear selected cell
	QAction clearAction_;

protected slots:

	//! \brief Handler for copy action.
	virtual void onCopyAction();

	//! \brief Handler for paste action
	virtual void onPasteAction();

	//! \brief Handler for cell clear action
	virtual void onClearAction(); 

private:
	//! \brief No copying
	CellEditTableView(const CellEditTableView& other);

	//! \brief No assignment
	CellEditTableView& operator=(const CellEditTableView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

};

#endif // EDITABLETABLEVIEW_H
