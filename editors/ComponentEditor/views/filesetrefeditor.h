/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditor.h
 */

#ifndef FILESETREFEDITOR_H
#define FILESETREFEDITOR_H

#include <common/widgets/listManager/listmanager.h>
#include <IPXACTmodels/component.h>

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
	 *
	*/
	FileSetRefEditor(QSharedPointer<Component> component,
		const QString title = tr("List"), 
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~FileSetRefEditor();

	/*! \brief Initialize the file set reference editor.
	 *
	 * This function must be called after creating the file set reference editor and before
	 * using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

private:

	//! \brief No copying
	FileSetRefEditor(const FileSetRefEditor& other);

	//! No assignment
	FileSetRefEditor& operator=(const FileSetRefEditor& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // FILESETREFEDITOR_H
