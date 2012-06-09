/* 
 *
 *  Created on: 8.2.2011
 *      Author: Antti Kamppi
 * 		filename: targetnameedit.h
 */

#ifndef TARGETNAMEEDIT_H
#define TARGETNAMEEDIT_H

#include <models/component.h>

#include <QLineEdit>
#include <QFileInfo>
#include <QWidget>

class LibraryInterface;

/*! \brief TargetNameEdit is a line-edit to select a file path for target file.
 * 
 * This widget is used to specify a value for spirit:targetName within
 * spirit:buildCommand in a file description.
 */
class TargetNameEdit : public QLineEdit {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param contents The optional parameter to specify the contents for the
	 * line edit.
	 *
	*/
	TargetNameEdit(QWidget *parent, 
		LibraryInterface* handler,
		QSharedPointer<Component> component,
		const QString contents = QString());

	//! \brief The destructor
	virtual ~TargetNameEdit();

protected:
	
	/*! \brief Event handler for mouse press events.
	 * 
	 * This event handler is used to create a file dialog to select a file
	 * that's filepath is added to this widget's line edit
	 * 
	 * \param e Pointer to the event that is passed to this event handler.
	*/
	void mousePressEvent(QMouseEvent * e);

private:

	//! \brief No copying
	TargetNameEdit(const TargetNameEdit& other);

	//! No assignment
	TargetNameEdit& operator=(const TargetNameEdit& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // TARGETNAMEEDIT_H
