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
#include <common/widgets/attributeBox/attributebox.h>

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
	 * \param fileName The file name that is set to the name-field.
	 * \param attributes QMap containing the attributes to set.
	 *
	*/
	FileNameEditor(QWidget *parent,
		LibraryInterface* handler,
		QSharedPointer<Component> component,
		const QString fileName = QString(),
		const QMap<QString, QString>& attributes = QMap<QString, QString>());

	//! \brief The destructor
	virtual ~FileNameEditor();
	
	/*! \brief Get the file name stored in the widget.
	 *
	 *
	 * \return QString containing the file name.
	*/
	QString getFileName() const;
	
	/*! \brief Set the file name for the widget
	 *
	 * \param fileName The file name to be set.
	 *
	*/
	void setFileName(const QString& fileName);

	/*! \brief Checks that the widget has valid inputs.
	 *
	 *
	 * \return True if the widget is in valid state and changes can be applied.
	*/
	bool isValid() const;

signals:

	//! \brief Emitted when contents of the widget change.
	void contentChanged();

	//! \brief Emitted when the name of the file changes.
	void nameChanged(const QString& newName);

private:

	//! No copying
	FileNameEditor(const FileNameEditor& other);

	//! No assignment
	FileNameEditor& operator=(const FileNameEditor& other);
	
	//! \brief The editor to edit the name of the file.
	FileNameLineEdit nameEdit_;
};

#endif // FILENAMEEDITOR_H
