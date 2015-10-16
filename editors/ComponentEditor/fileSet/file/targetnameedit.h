//-----------------------------------------------------------------------------
// File: targetnameedit.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 8.2.2011
//
// Description:
// TargetNameEdit is a line-edit to select a file path for target file.
//-----------------------------------------------------------------------------

#ifndef TARGETNAMEEDIT_H
#define TARGETNAMEEDIT_H

#include <QLineEdit>
#include <QWidget>

class LibraryInterface;
class Component;

//-----------------------------------------------------------------------------
//! TargetNameEdit is a line-edit to select a file path for target file.
//
//  This widget is used to specify a value for ipxact:targetName within ipxact:buildCommand in a file description.
//-----------------------------------------------------------------------------
class TargetNameEdit : public QLineEdit
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent      Pointer to the owner of this widget.
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] contents    The optional parameter to specify the contents for the line edit.
	 */
	TargetNameEdit(QWidget *parent, LibraryInterface* handler, QSharedPointer<Component> component,
        const QString contents = QString());

	//! The destructor.
	virtual ~TargetNameEdit();

signals:

	//! Emitted when the target name changes.
	void contentChanged();

protected:
	
	/*!
     *  Event handler for mouse press events.
	 * 
	 *  This event handler is used to create a file dialog to select a file that's filepath is added to this
     *  widget's line edit
	 * 
	 *      @param [in] e   Pointer to the event that is passed to this event handler.
	 */
	void mousePressEvent(QMouseEvent * e);

private:

	//! No copying. No assignment. No remorse.
	TargetNameEdit(const TargetNameEdit& other);
	TargetNameEdit& operator=(const TargetNameEdit& other);

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // TARGETNAMEEDIT_H
