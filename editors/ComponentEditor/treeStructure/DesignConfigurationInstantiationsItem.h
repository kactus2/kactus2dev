//-----------------------------------------------------------------------------
// File: InstantiationsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The design configuration instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONINSTANTIATIONSITEM_H
#define DESIGNCONFIGURATIONINSTANTIATIONSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;
class View;

class ExpressionParser;
class ViewValidator;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! The design configuration instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class DesignConfigurationInstantiationsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] referenceCounter        The reference counter.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The used expression parser.
	 *      @param [in] parent                  The parent item.
	 */
	DesignConfigurationInstantiationsItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<InstantiationsValidator> validator,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~DesignConfigurationInstantiationsItem();

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
	DesignConfigurationInstantiationsItem(const DesignConfigurationInstantiationsItem& other);
	DesignConfigurationInstantiationsItem& operator=(const DesignConfigurationInstantiationsItem& other);

    //! The validator for design configuration instantiations.
    QSharedPointer<InstantiationsValidator> validator_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // DESIGNCONFIGURATIONINSTANTIATIONSITEM_H
