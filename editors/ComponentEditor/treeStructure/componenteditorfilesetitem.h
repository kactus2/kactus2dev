/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILESETITEM_H
#define COMPONENTEDITORFILESETITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/fileset.h>
#include <QSharedPointer>

/*! \brief The item for a single file set in the component editor's navigation tree.
 * 
 */
class ComponentEditorFileSetItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param fileSet Pointer to the file set being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileSetItem();

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

	/*! \brief Add a new child to the item.
	 *
	 * \param index The index to add the child into.
	 *
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
    void childRemoved(int index);

private:
	//! \brief No copying
	ComponentEditorFileSetItem(const ComponentEditorFileSetItem& other);

	//! \brief No assignment
	ComponentEditorFileSetItem& operator=(const ComponentEditorFileSetItem& other);
    
	//! \brief Pointer to the file set being edited.
	QSharedPointer<FileSet> fileSet_;

	//! \brief Contains the files of the file set.
	QList<QSharedPointer<File> >& files_;
};

#endif // COMPONENTEDITORFILESETITEM_H
