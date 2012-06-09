/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenameeditor.h
 */

#ifndef FILENAMEEDITOR_H
#define FILENAMEEDITOR_H

#include "filenamelineedit.h"
#include <models/component.h>

#include <QGroupBox>
#include <QMap>
#include <QString>
#include <QFileInfo>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief FileNameEditor is a widget to edit the name element in FileSet.
 * 
 * This widget is also used to manage the attributes for the name-element.
 */
class FileNameEditor : public QGroupBox {
	Q_OBJECT

public:

	
	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param file Pointer to the file being edited.
	 *
	*/
	FileNameEditor(QWidget *parent,
		LibraryInterface* handler,
		QSharedPointer<Component> component,
		QSharedPointer<File> file);

	//! \brief The destructor
	virtual ~FileNameEditor();

	/*! \brief Checks that the widget has valid inputs.
	 *
	 *
	 * \return True if the widget is in valid state and changes can be applied.
	*/
	bool isValid() const;

	//! \brief Refresh the editor.
	void refresh();

signals:

	//! \brief Emitted when contents of the widget change.
	void contentChanged();

private:

	//! No copying
	FileNameEditor(const FileNameEditor& other);

	//! No assignment
	FileNameEditor& operator=(const FileNameEditor& other);
	
	//! \brief The editor to edit the name of the file.
	FileNameLineEdit nameEdit_;

	//! \brief Pointer to the file being edited.
	QSharedPointer<File> file_;
};

#endif // FILENAMEEDITOR_H
