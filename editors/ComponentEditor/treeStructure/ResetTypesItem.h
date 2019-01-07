//-----------------------------------------------------------------------------
// File: ResetTypesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2019
//
// Description:
// The Reset types-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef RESETTYPESITEM_H
#define RESETTYPESITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/Component/validators/ComponentValidator.h>

//-----------------------------------------------------------------------------
//! The Reset types-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ResetTypesItem: public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
	 *      @param [in] component   The component being edited.
     *      @param [in] validator   Validator for component items.
     *      @param [in] parent      The parent item.
     */
    ResetTypesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, ComponentValidator& validator, ComponentEditorItem* parent);

	/*!
     *  The destructor.
     */
    virtual ~ResetTypesItem();

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
	 *      @return The text to display.
     */
	virtual QString text() const;

	/*!
     *  Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
     */
	virtual ItemEditor* editor();

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return True if item is in valid state.
     */
	virtual bool isValid() const;

private:
	//! No copying.
	ResetTypesItem(const ResetTypesItem& other);
    ResetTypesItem& operator=(const ResetTypesItem& other);

    //! Validator for component items.
    ComponentValidator& componentValidator_;
};

#endif // RESETTYPESITEM_H
