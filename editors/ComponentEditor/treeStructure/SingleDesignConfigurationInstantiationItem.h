//-----------------------------------------------------------------------------
// File: SingleDesignConfigurationInstantiationItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The item for one design configuration instantiation-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef SINGLEDESIGNCONFIGURATIONINSTANTIATIONITEM_H
#define SINGLEDESIGNCONFIGURATIONINSTANTIATIONITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;
class DesignConfigurationInstantiation;
class ExpressionParser;
class InstantiationsValidator;

//-----------------------------------------------------------------------------
//! The item for one design configuration instantiation-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class SingleDesignConfigurationInstantiationItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] instantiation           The instantiation of the item.
	 *      @param [in] parent                  The parent item.
	 */
	SingleDesignConfigurationInstantiationItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<InstantiationsValidator> validator,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~SingleDesignConfigurationInstantiationItem();

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

    //! Checks if the item represented by the item is valid.
    bool isValid() const;
    
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
	SingleDesignConfigurationInstantiationItem(const SingleDesignConfigurationInstantiationItem& other);
	SingleDesignConfigurationInstantiationItem& operator=(const SingleDesignConfigurationInstantiationItem& other);

    //! The instantiation represented by the item.
    QSharedPointer<DesignConfigurationInstantiation> instantiation_;

    //! Validator for the instantiation.
    QSharedPointer<InstantiationsValidator> validator_;

};

#endif // SINGLEDESIGNCONFIGURATIONINSTANTIATIONITEM_H
