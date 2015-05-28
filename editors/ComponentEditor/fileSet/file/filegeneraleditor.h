/* 
 *
 *  Created on: 6.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraleditor.h
 */

#ifndef FILEGENERALEDITOR_H
#define FILEGENERALEDITOR_H

#include <IPXACTmodels/file.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QSharedPointer>

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
	FileGeneralEditor(QWidget *parent, 
		QSharedPointer<File> file);

	//! \brief The destructor
	virtual ~FileGeneralEditor();
	
	/*! \brief Restore the settings from the model.
	 *
	*/
	void refresh();

signals:

	//! \brief Emitted when the contents on one of the elements change.
	void contentChanged();

private slots:

	//! \brief Handler for changes in logical name.
	void onLogicalNameChanged();

	//! \brief Handler for changes in include file setting.
	void onIncludeFileChanged();

	//! \brief Handler for external declarations changes.
	void onExternalDecChanged();

private:

	//! \brief No copying
	FileGeneralEditor(const FileGeneralEditor& other);

	//! No assignment
	FileGeneralEditor& operator=(const FileGeneralEditor& other);
	
	//! \brief Pointer to the file instance that is modified.
	QSharedPointer<File> file_;

	//! \brief The editor to set the logical name for the file.
	QLineEdit logicalName_;

	//! \brief The check box to set the logical name as default or not.
	QCheckBox logicalDefault_;

	//! \brief The check box to set the file as include file or not.
	QCheckBox includeFile_;

	//! \brief The check box to inform that file contains external declarations.
	QCheckBox externalDec_;
};

#endif // FILEGENERALEDITOR_H
