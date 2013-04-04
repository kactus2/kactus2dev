/* 
 *	Created on:	4.4.2013
 *	Author:		Antti Kamppi
 *	File name:	filetypesview.h
 *	Project:		Kactus 2
*/

#ifndef FILETYPESVIEW_H
#define FILETYPESVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

/*! \brief A view for the file type settings in the settings dialog.
 *
 */
class FileTypesView : public EditableTableView {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		FileTypesView
	 * Full name:	FileTypesView::FileTypesView
	 * Access:		private 
	 *
	 * \param parent Pointer to the owner of the view.
	 *
	*/
	FileTypesView(QWidget *parent);
	
	//! \brief The destructor.
	virtual ~FileTypesView();

protected:

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

private:
	
	//! \brief No copying
	FileTypesView(const FileTypesView& other);

	//! \brief No assignment
	FileTypesView& operator=(const FileTypesView& other);

};

#endif // FILETYPESVIEW_H
