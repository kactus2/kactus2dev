//-----------------------------------------------------------------------------
// File: PowerDomainsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// The Power domains-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINSITEM_H
#define POWERDOMAINSITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

//-----------------------------------------------------------------------------
//! The Reset types-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class PowerDomainsItem: public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
     *      @param [in] component   The component being edited.
     *      @param [in] expressions	The collection of objects for expression handling.
     *      @param [in] parent      The parent item.
     */
    PowerDomainsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, 
		QSharedPointer<ReferenceCounter> referenceCounter,
		ExpressionSet expressions, 
		ComponentEditorItem* parent);

	/*!
     *  The destructor.
     */
    virtual ~PowerDomainsItem();

    //! No copying.
    PowerDomainsItem(const PowerDomainsItem& other) =  delete;
    PowerDomainsItem& operator=(const PowerDomainsItem& other) = delete;

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

    //! Validator for component items.
    //ComponentValidator& componentValidator_;
	
	ExpressionSet expressions_;
};

#endif // POWERDOMAINSITEM_H
