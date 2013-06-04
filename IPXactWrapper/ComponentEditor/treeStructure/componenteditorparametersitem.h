/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorparametersitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORPARAMETERITEM_H
#define COMPONENTEDITORPARAMETERITEM_H

#include "componenteditoritem.h"
#include <models/parameter.h>

/*! \brief The parameters-item in the component editor navigation tree.
 *
 */
class ComponentEditorParametersItem : public ComponentEditorItem {
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
	ComponentEditorParametersItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorParametersItem();

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
	ComponentEditorParametersItem(const ComponentEditorParametersItem& other);

	//! \brief No assignment
	ComponentEditorParametersItem& operator=(const ComponentEditorParametersItem& other);

	//! \brief The list containing the parameters to edit.
	QList<QSharedPointer<Parameter> >& parameters_;
};

#endif // COMPONENTEDITORPARAMETERITEM_H
