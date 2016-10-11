//-----------------------------------------------------------------------------
// File: filesview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.6.2012
//
// Description:
// The view to display the files contained in a file set.
//-----------------------------------------------------------------------------

#ifndef FILESVIEW_H
#define FILESVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QAction>
#include <QMouseEvent>
#include <QSharedPointer>

class LibraryInterface;
class Component;

//-----------------------------------------------------------------------------
//! The view to display the files contained in a file set.
//-----------------------------------------------------------------------------
class FilesView : public EditableTableView
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 * 
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the owner of this view.
	 */
	FilesView(LibraryInterface* handler, QSharedPointer<Component> component, QWidget *parent);
	
	//! The destructor.
	virtual ~FilesView();

signals:

    //! Emitted when a new file should be added by file browsing.
    void addItemByBrowsing();

protected:

	//! Handler for context menu events.
    virtual void contextMenuEvent(QContextMenuEvent* event);

private:
	
	//! No copying. No assignment.
	FilesView(const FilesView& other);
	FilesView& operator=(const FilesView& other);

	//! The instance that manages the library.
	LibraryInterface* handler_;

	//! The component being edited.
	QSharedPointer<Component> component_;

    //! Action for adding new files by browsing.
    QAction* addByBrowsingAction_;
};

#endif // FILESVIEW_H
