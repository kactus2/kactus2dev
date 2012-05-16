/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditordeffilebuildersitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORDEFFILEBUILDERSITEM_H
#define COMPONENTEDITORDEFFILEBUILDERSITEM_H

#include "componenteditoritem.h"
#include <models/filebuilder.h>
#include <IPXactWrapper/ComponentEditor/fileSet/filebuildereditor.h>

#include <QList>
#include <QSharedPointer>

/*! \brief The item for default file builders under file set in component editor's navigation tree.
 *
 */
class ComponentEditorDefFileBuildersItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorDefFileBuildersItem(QList<QSharedPointer<FileBuilder> >& fileBuilders,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorDefFileBuildersItem();

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	//! \brief No copying
	ComponentEditorDefFileBuildersItem(const ComponentEditorDefFileBuildersItem& other);

	//! \brief No assignment
	ComponentEditorDefFileBuildersItem& operator=(const ComponentEditorDefFileBuildersItem& other);

	//! \brief Contains the file builders to edit.
	QList<QSharedPointer<FileBuilder> >& fileBuilders_;

	//! \brief The editor to edit the file builders.
	FileBuilderEditor editor_;
};

#endif // COMPONENTEDITORDEFFILEBUILDERSITEM_H
