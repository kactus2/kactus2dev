//-----------------------------------------------------------------------------
// File: InstantiationsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef INSTANTIATIONSITEM_H
#define INSTANTIATIONSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;

class ExpressionParser;
class ViewValidator;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! The Instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class InstantiationsItem : public ComponentEditorItem
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
	InstantiationsItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~InstantiationsItem();

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
	InstantiationsItem(const InstantiationsItem& other);
	InstantiationsItem& operator=(const InstantiationsItem& other);

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for instantiations.
    QSharedPointer<InstantiationsValidator> validator_;

    //! Child item for component instantiations.
    QSharedPointer<ComponentEditorItem> componentInstantiationsItem_;

    //! Child item for design configuration instantiations.
    QSharedPointer<ComponentEditorItem> designConfigurationInstantiationsItem_;

    //! Child item for design instantiations.
    QSharedPointer<ComponentEditorItem> designInstantiationsItem_;
};

#endif // INSTANTIATIONSITEM_H
