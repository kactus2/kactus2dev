/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenamelineedit.h
 */

#ifndef FILENAMELINEEDIT_H
#define FILENAMELINEEDIT_H

#include <models/component.h>

#include <QLineEdit>
#include <QFileInfo>
#include <QMouseEvent>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief FileNameLineEdit is used to select a file to QString format.
 *
 * This widget is used to select a file in a file system and convert it's 
 * file path to a relative file path.
 */
class FileNameLineEdit : public QLineEdit {
	Q_OBJECT

public:
	
	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param contents Contains the contents to set in the line edit.
	 *
	*/
	FileNameLineEdit(QWidget *parent, 
		LibraryInterface* handler,
		QSharedPointer<Component> component,
		const QString& contents = QString());

	//! \brief The destructor
	virtual ~FileNameLineEdit();
	
	/*! \brief Checks that the widget is in valid state.
	 *
	 *
	 * \return True if the widget has valid input.
	*/
	bool isValid() const;

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

	//! No copying
	FileNameLineEdit(const FileNameLineEdit& other);

	//! No assignment
	FileNameLineEdit& operator=(const FileNameLineEdit& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

};

#endif // FILENAMELINEEDIT_H
