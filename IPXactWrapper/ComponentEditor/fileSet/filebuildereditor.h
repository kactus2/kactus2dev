/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildereditor.h
 */

#ifndef FILEBUILDEREDITOR_H
#define FILEBUILDEREDITOR_H

#include "../itemeditor.h"

#include <IPXactWrapper/ComponentEditor/fileBuilders/filebuilderseditor.h>

#include <QSharedPointer>

class Component;

/*! \brief Container widget to edit file's file builders.
 *
 */
class FileBuilderEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the QList containing the file builders.
	 * \param parent Pointer to the parent of this container.
	 *
	*/
	FileBuilderEditor(QSharedPointer<Component> component, 
		void* dataPointer, 
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~FileBuilderEditor();

	/*! \brief Check for the validity of the edited item.
	* 
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	 * The base class implementation does nothing.
	*/
	virtual void makeChanges();

private:

	//! \brief No copying
	FileBuilderEditor(const FileBuilderEditor& other);

	//! No assignment
	FileBuilderEditor& operator=(const FileBuilderEditor& other);

	//! \brief The editor to set the file builders
	FileBuildersEditor editor_;
	
};

#endif // FILEBUILDEREDITOR_H
