/* 
 *
 *  Created on: 8.2.2011
 *      Author: Antti Kamppi
 * 		filename: targetnameedit.h
 */

#ifndef TARGETNAMEEDIT_H
#define TARGETNAMEEDIT_H

#include <QLineEdit>
#include <QFileInfo>
#include <QWidget>

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
	 * \param baseLocation Refers to the containing component's xml-file location.
	 * \param contents The optional parameter to specify the contents for the
	 * line edit.
	 *
	*/
	TargetNameEdit(QWidget *parent, 
		const QFileInfo& baseLocation,
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
	
	//! \brief Refers to the location in file system used as base for relative paths
	QFileInfo baseLocation_;
};

#endif // TARGETNAMEEDIT_H
