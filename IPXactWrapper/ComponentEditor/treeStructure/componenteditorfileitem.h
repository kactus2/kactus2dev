/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILEITEM_H
#define COMPONENTEDITORFILEITEM_H

#include "componenteditoritem.h"
#include <models/file.h>
#include <IPXactWrapper/ComponentEditor/fileSet/fileeditor.h>

/*! \brief The item for a single file in component editor's navigation tree.
 *
 */
class ComponentEditorFileItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param file Pointer to the file being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileItem(QSharedPointer<File> file,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

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

private:
	//! \brief No copying
	ComponentEditorFileItem(const ComponentEditorFileItem& other);

	//! \brief No assignment
	ComponentEditorFileItem& operator=(const ComponentEditorFileItem& other);

	//! \brief Pointer to the file being edited.
	QSharedPointer<File> file_;

	//! \brief The editor to edit the file.
	FileEditor editor_;
};

#endif // COMPONENTEDITORFILEITEM_H
