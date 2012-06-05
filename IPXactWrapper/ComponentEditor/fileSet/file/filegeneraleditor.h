/* 
 *
 *  Created on: 6.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraleditor.h
 */

#ifndef FILEGENERALEDITOR_H
#define FILEGENERALEDITOR_H

#include <models/file.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>

/*! \brief FileGeneralEditor is a widget to edit File's general settings.
 *
 */
class FileGeneralEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 * \param file Pointer to the file that is being edited.
	 *
	*/
	FileGeneralEditor(QWidget *parent, File* file);

	//! \brief The destructor
	virtual ~FileGeneralEditor();
	
	/*! \brief Restore the settings from the model.
	 *
	*/
	void restore();

	/*! \brief Apply the changes from the editor to the model.
	 *
	*/
	void apply();

signals:

	//! \brief Emitted when the contents on one of the elements change.
	void contentChanged();

private:

	//! \brief No copying
	FileGeneralEditor(const FileGeneralEditor& other);

	//! No assignment
	FileGeneralEditor& operator=(const FileGeneralEditor& other);
	
	//! \brief Pointer to the file instance that is modified.
	File* file_;

	//! \brief The editor to set the logical name for the file.
	QLineEdit logicalName_;

	//! \brief The checkbox to set the logical name as default or not.
	QCheckBox logicalDefault_;

	//! \brief The checkbox to set the file as include file or not.
	QCheckBox includeFile_;

	//! \brief The checkbox to inform that file contains external declarations.
	QCheckBox externalDec_;

	//! \brief Plain text editor to set the description of the file.
	QPlainTextEdit description_;
};

#endif // FILEGENERALEDITOR_H
