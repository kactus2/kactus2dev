/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditor.h
 */

#ifndef FILESETREFEDITOR_H
#define FILESETREFEDITOR_H

#include <common/widgets/listManager/listmanager.h>
#include <models/component.h>

#include <QSharedPointer>

/*! \brief Editor to set the file set references of a view.
 *
 */
class FileSetRefEditor : public ListManager {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param title Title for the QGroupBox
	 * \param parent Pointer to the owner of this editor.
	 * \param items QStringList containing the file set names for the editor.
	 *
	*/
	FileSetRefEditor(QSharedPointer<Component> component,
		const QString title = tr("List"), 
		QWidget *parent = 0,
		const QStringList& items = QStringList());
	
	//! \brief The destructor
	virtual ~FileSetRefEditor();

public slots:

	//! \brief Called when user clicks "Add" button.
	virtual void onAdd();

	//! \brief Called when user clicks "Edit" button.
	virtual void onEdit();

private:

	//! \brief No copying
	FileSetRefEditor(const FileSetRefEditor& other);

	//! No assignment
	FileSetRefEditor& operator=(const FileSetRefEditor& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // FILESETREFEDITOR_H
