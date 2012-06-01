/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: fileseditor.h
 *		Project: Kactus 2
 */

#ifndef FILESEDITOR_H
#define FILESEDITOR_H

#include "filesmodel.h"
#include <models/fileset.h>
#include <models/file.h>
#include <models/component.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSharedPointer>
#include <QPushButton>

/*! \brief The editor to add/remove/edit files of a file set.
 *
 */
class FilesEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param fileSet Pointer to the file set being edited.
	 * \param parent Pointer to the owner of this editor.
	 * \param title The title for the group box.
	 *
	*/
	FilesEditor(QSharedPointer<Component> component,
		QSharedPointer<FileSet> fileSet,
		QWidget *parent,
		const QString& title = tr("Files"));

	//! \brief The destructor
	~FilesEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

public slots:

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

signals:

	//! \brief Emitted when the files change.
	void contentChanged();

private slots:

	/*! \brief Add new files to file set.
	 *
	*/
	void onAddFiles();

private:
	
	//! \brief No copying
	FilesEditor(const FilesEditor& other);

	//! \brief No assignment
	FilesEditor& operator=(const FilesEditor& other);

	//! \brief Contains the files to display.
	QList<QSharedPointer<File> >& files_;

	//! \brief The view to display the file information.
	EditableTableView view_;

	//! \brief The model to edit the file summary.
	FilesModel model_;

	//! \brief The button to add new files to file set.
	QPushButton addFilesButton_;
};

#endif // FILESEDITOR_H
