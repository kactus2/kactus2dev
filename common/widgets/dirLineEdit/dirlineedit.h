/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: dirlineedit.h
 *		Project: Kactus 2
 */

#ifndef DIRLINEEDIT_H
#define DIRLINEEDIT_H

#include <QLineEdit>
#include <QFileInfo>
#include <QMouseEvent>

/*! \brief The line editor to select a directory.
 *
 */
class DirLineEdit : public QLineEdit {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this editor
	 *
	*/
	DirLineEdit(QWidget *parent, const QFileInfo& basePath);
	
	//! \brief The destructor
	virtual ~DirLineEdit();

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
	DirLineEdit(const DirLineEdit& other);

	//! \brief No assignment
	DirLineEdit& operator=(const DirLineEdit& other);

	//! \brief The location used as base for relative paths
	QFileInfo baseLocation_;
};

#endif // DIRLINEEDIT_H
