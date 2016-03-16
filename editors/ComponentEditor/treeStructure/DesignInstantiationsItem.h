//-----------------------------------------------------------------------------
// File: InstantiationsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The design instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATIONSITEM_H
#define DESIGNINSTANTIATIONSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;

class ExpressionParser;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! The design instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class DesignInstantiationsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] validator               The validator for design instantiation.
	 *      @param [in] parent                  The parent item.
	 */
	DesignInstantiationsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, QSharedPointer<InstantiationsValidator> validator,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~DesignInstantiationsItem();

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
     *  Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 * 
	 *      @param [in] index   The index to add the child into.
	 */
	virtual void createChild(int index);

private:

	//! No copying.
	DesignInstantiationsItem(const DesignInstantiationsItem& other);
	DesignInstantiationsItem& operator=(const DesignInstantiationsItem& other);

    //! Validator for design instantiations.
    QSharedPointer<InstantiationsValidator> validator_;
};

#endif // DESIGNINSTANTIATIONSITEM_H
