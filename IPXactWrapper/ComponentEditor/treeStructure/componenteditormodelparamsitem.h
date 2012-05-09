/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormodelparamsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMODELPARAMSITEM_H
#define COMPONENTEDITORMODELPARAMSITEM_H

#include "componenteditoritem.h"

#include <IPXactWrapper/ComponentEditor/modelParameters/modelparametereditor.h>

/*! \brief The Model parameters-item in the component editor navigation tree.
 *
 */
class ComponentEditorModelParamsItem : public ComponentEditorItem {
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
	ComponentEditorModelParamsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorModelParamsItem();

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
	ComponentEditorModelParamsItem(const ComponentEditorModelParamsItem& other);

	//! \brief No assignment
	ComponentEditorModelParamsItem& operator=(const ComponentEditorModelParamsItem& other);

	//! \brief The editor to edit the model parameters of the component.
	ModelParameterEditor editor_;
};

#endif // COMPONENTEDITORMODELPARAMSITEM_H
