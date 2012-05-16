/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.h
 */

#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include "filenameeditor.h"
#include "../itemeditor.h"
#include "filegeneraleditor.h"

#include <common/widgets/attributeBox/attributebox.h>

#include <QWidget>
#include <QFileInfo>
#include <QTabWidget>
#include <QSharedPointer>

class Component;
class File;
class FileGeneralTab;
class FileExtraTab;

/*! \brief FileEditor is a widget to edit the details of a File
 * 
 */
class FileEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param baseLocation Refers to the containing component's xml-file location
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the element that is edited
	 * \param parent Pointer to the owner of this widget
	 *
	*/
	FileEditor(const QFileInfo& baseLocation, 
		QSharedPointer<Component> component, 
		void* dataPointer,
		QWidget *parent);

	/*! \brief The constructor
	 *
	 * \param baseLocation Path to the containing component's xml-file location
	 * \param component Pointer to the component being edited.
	 * \param file Pointer to the file that is edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FileEditor(const QString& baseLocation, 
		QSharedPointer<Component> component, 
		QSharedPointer<File> file,
		QWidget *parent);

	//! \brief The destructor
	virtual ~FileEditor();

	/*! \brief Check for the validity of the edited item.
	* 
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Remove the File from the file set.
	 *
	 * This function should be used when user wants to remove a file from 
	 * the containing file set.
	 *
	*/
	virtual void removeModel();

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

private:

	//! No copying
	FileEditor(const FileEditor& other);

	//! No assignment
	FileEditor& operator=(const FileEditor& other);

	//! \brief Set up the file editor.
	 void initialize();

	//! \brief The tab widget that contains the other widgets.
	QTabWidget tabs_;

	//! \brief Pointer to the widget that is used for general settings of file.
	FileGeneralTab* generalTab_;

	//! \brief Pointer to the widget that is used to edit i.e external dependencies.
	FileExtraTab* extraTab_;

	//! \brief Pointer to the file that is edited in this editor.
	File* file_;

	//! \brief The path to the containing component's xml-file.
	QFileInfo baseLocation_;
};

#endif // FILEEDITOR_H
