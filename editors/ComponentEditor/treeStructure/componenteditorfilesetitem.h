//-----------------------------------------------------------------------------
// File: fileseteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single file set in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFILESETITEM_H
#define COMPONENTEDITORFILESETITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class LibraryInterface;
class FileSet;
class File;
//-----------------------------------------------------------------------------
//! The item for a single file set in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFileSetItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] fileSet     Pointer to the file set being edited.
	 *      @param [in] model       Pointer to the model that owns the items.
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the parent item.
	 */
	ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet, ComponentEditorTreeModel* model,
        LibraryInterface* libHandler, QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorFileSetItem();

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
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

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

    /*!
     *  Handles the addition of a file outside the editor.
     */
    void onFileAdded(File* file);

    /*!
     *  Returns the file set corresponding to this item.
     */
    FileSet const* getFileSet() const;

signals:
    
    /*!
     *  Indicates the removal of an item.
     *
     *      @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

private:

    //! No copying. No assignment.
	ComponentEditorFileSetItem(const ComponentEditorFileSetItem& other);
	ComponentEditorFileSetItem& operator=(const ComponentEditorFileSetItem& other);
    
	//! Pointer to the file set being edited.
	QSharedPointer<FileSet> fileSet_;

	//! Contains the files of the file set.
    QSharedPointer<QList<QSharedPointer<File> > > files_;
};

#endif // COMPONENTEDITORFILESETITEM_H
