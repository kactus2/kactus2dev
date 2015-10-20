//-----------------------------------------------------------------------------
// File: componenteditorfilesetsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The file sets-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFILESETSITEM_H
#define COMPONENTEDITORFILESETSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class PluginManager;
class Component;
class FileSet;
class File;

//-----------------------------------------------------------------------------
//! The file sets-item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFileSetsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] model       Pointer to the model that owns the items.
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the parent item.
	 */
	ComponentEditorFileSetsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        PluginManager& pluginMgr, QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorFileSetsItem();

    /*!
     *  Get the font to be used for text of this item.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 *
	 *      @param [in] index   The index to add the child into.
	 */
	virtual void createChild(int index);

signals:

    //! Emitted when the dependency model should be refreshed.
    void refreshDependencyModel();

private slots:
    /*!
     *  Handles the addition of a file outside the file set editor.
     */
    void onFileAdded(File* file);

private:

    bool isParentFileSet(File* file, const FileSet* fileset) const;

	//! No copying. No assignment.
	ComponentEditorFileSetsItem(const ComponentEditorFileSetsItem& other);
	ComponentEditorFileSetsItem& operator=(const ComponentEditorFileSetsItem& other);

	//! The file sets to edit.
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets_;

	//! The plugin manager.
	PluginManager& pluginMgr_;
};

#endif // COMPONENTEDITORFILESETSITEM_H
