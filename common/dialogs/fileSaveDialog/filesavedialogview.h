/* 
 *	Created on: 19.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	filesavedialogview.h
 * 	Project:	Kactus 2
*/

#ifndef FILESAVEDIALOGVIEW_H
#define FILESAVEDIALOGVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QAction>

/*! \brief The view used to display the files to be created in the FileSaveDialog.
 *
 */
class FileSaveDialogView : public QTableView {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		FileSaveDialogView
	 * Full name:	FileSaveDialogView::FileSaveDialogView
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the view.
	 *
	*/
	FileSaveDialogView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~FileSaveDialogView();

protected:

	//! \brief Handler for mouse double click events
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

	//! \brief Handler for path changes through context menu. 
	virtual void onChange();

private:
	
	//! \brief No copying
	FileSaveDialogView(const FileSaveDialogView& other);

	//! \brief No assignment
	FileSaveDialogView& operator=(const FileSaveDialogView& other);

	//! \brief The action in the context menu to change the path.
	QAction* changeAction_;

	//! \brief The point where e.g. context menu was requested.
	QPoint pressedPoint_;
};

#endif // FILESAVEDIALOGVIEW_H
