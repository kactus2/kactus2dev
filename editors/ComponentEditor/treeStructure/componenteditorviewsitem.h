//-----------------------------------------------------------------------------
// File: componenteditorviewsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORVIEWSITEM_H
#define COMPONENTEDITORVIEWSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;
class View;

class ExpressionParser;
class ViewValidator;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! The Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorViewsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] referenceCounter        Pointer to the reference counter.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] expressionParser        The used expression parser.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
	ComponentEditorViewsItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorViewsItem();

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
    virtual QFont getFont() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
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
	ComponentEditorViewsItem(const ComponentEditorViewsItem& other);

	//! No assignment.
	ComponentEditorViewsItem& operator=(const ComponentEditorViewsItem& other);

	//! The views being edited.
    QSharedPointer<QList<QSharedPointer<View> > > views_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator used for views.
    QSharedPointer<ViewValidator> viewValidator_;
};

#endif // COMPONENTEDITORVIEWSITEM_H
