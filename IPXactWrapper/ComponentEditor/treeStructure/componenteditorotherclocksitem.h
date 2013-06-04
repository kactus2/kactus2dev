/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorotherclocksitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITOROTHERCLOCKSITEM_H
#define COMPONENTEDITOROTHERCLOCKSITEM_H

#include "componenteditoritem.h"
#include <models/otherclockdriver.h>

/*! \brief The Other clock drivers-item in the component editor's navigation tree.
 *
 */
class ComponentEditorOtherClocksItem : public ComponentEditorItem {
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
	ComponentEditorOtherClocksItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorOtherClocksItem();

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
	ComponentEditorOtherClocksItem(const ComponentEditorOtherClocksItem& other);

	//! \brief No assignment
	ComponentEditorOtherClocksItem& operator=(const ComponentEditorOtherClocksItem& other);

	//! \brief The other clock drivers to edit
	QList<QSharedPointer<OtherClockDriver> >& otherClocks_;
};

#endif // COMPONENTEDITOROTHERCLOCKSITEM_H
