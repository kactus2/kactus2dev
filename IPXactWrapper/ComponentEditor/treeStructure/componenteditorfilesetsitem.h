/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILESETSITEM_H
#define COMPONENTEDITORFILESETSITEM_H

#include "componenteditoritem.h"
#include <models/fileset.h>
#include <IPXactWrapper/ComponentEditor/fileSet/filesetseditor.h>

#include <QSharedPointer>

class PluginManager;

/*! \brief The file sets-item in the component editor navigation tree.
 *
 */
class ComponentEditorFileSetsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileSetsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
        PluginManager& pluginMgr,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileSetsItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const;

	/*! \brief Add a new child to the item.
	 *
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

private:
	//! \brief No copying
	ComponentEditorFileSetsItem(const ComponentEditorFileSetsItem& other);

	//! \brief No assignment
	ComponentEditorFileSetsItem& operator=(const ComponentEditorFileSetsItem& other);

	//! \brief The file sets to edit.
	QList<QSharedPointer<FileSet> >& fileSets_;

	//! \brief The editor to edit the file sets.
	FileSetsEditor editor_;
};

#endif // COMPONENTEDITORFILESETSITEM_H
