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

	/*!
     *  A new file should be added to given index.
	 *
	 *      @param [in] index       The position where new item should be added at.
	 *      @param [in] filePath    Path to the file that is to be added.
	 */
	void addItem(const QModelIndex& index, const QString& filePath);

protected:

	//! Handler for mouse double click events.
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private slots:

	//! Handler for add action.
	virtual void onAddAction();

private:
	
	//! No copying. No assignment.
	FilesView(const FilesView& other);
	FilesView& operator=(const FilesView& other);

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // FILESVIEW_H
