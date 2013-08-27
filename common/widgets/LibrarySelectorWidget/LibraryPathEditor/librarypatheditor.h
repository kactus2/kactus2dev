/* 
 *  	Created on: 26.1.2012
 *      Author: Antti Kamppi
 * 		filename: librarypatheditor.h
 *		Project: Kactus 2
 */

#ifndef LIBRARYPATHEDITOR_H
#define LIBRARYPATHEDITOR_H

#include <QLineEdit>
#include <QString>
#include <QKeyEvent>

/*! \brief Line edit to manually set the path to save the new item to.
 *
 */
class LibraryPathEditor : public QLineEdit {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this line editor.
	 *
	*/
	LibraryPathEditor(QWidget *parent);

	//! \brief The destructor
	virtual ~LibraryPathEditor();

	/*! \brief Set the minimum path.
	 *
	 * \param path The path to set.
	 *
	*/
	void setUnmodifiablePath(const QString& path);

protected:

	/*! \brief Handler for key press events.
	 *
	 * \param event Pointer to the key event.
	 *
	*/
	virtual void keyPressEvent(QKeyEvent* event);

private:
	
	//! \brief No copying
	LibraryPathEditor(const LibraryPathEditor& other);

	//! \brief No assignment
	LibraryPathEditor& operator=(const LibraryPathEditor& other);

	//! \brief The minimum path that can not be changed.
	QString unmodifiablePath_;
};

#endif // LIBRARYPATHEDITOR_H
