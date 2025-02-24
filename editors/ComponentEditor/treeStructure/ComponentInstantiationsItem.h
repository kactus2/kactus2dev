//-----------------------------------------------------------------------------
// File: InstantiationsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The component instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONSITEM_H
#define COMPONENTINSTANTIATIONSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;
class ComponentInstantiation;
class ExpressionParser;
class InstantiationsValidator;
class ParametersInterface;
class SingleComponentInstantiationItem;
class FileBuilderInterface;
class ComponentInstantiationInterface;
class AllInstantiationsValidator;

//-----------------------------------------------------------------------------
//! The component instantiations-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentInstantiationsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] model						The model that owns the items.
	 *    @param [in] libHandler					The instance that manages the library.
	 *    @param [in] validator					The single instantiation validator.
	 *    @param [in] allInstantiationsValidator  The validator validating all instantiations together.
	 *    @param [in] component					The component being edited.
	 *    @param [in] referenceCounter			The reference counter.
	 *    @param [in] parameterFinder				The parameter finder.
	 *    @param [in] expressionFormatter			The expression formatter.
     *    @param [in] expressionParser			The used expression parser.
     *    @param [in] instantiationInterface		Interface for accessing component instantiations.
	 *    @param [in] parent						The parent item.
	 */
	ComponentInstantiationsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
		QSharedPointer<Component> component, QSharedPointer<InstantiationsValidator> validator,
		QSharedPointer<AllInstantiationsValidator> allInstantiationsValidator,
		QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
		QSharedPointer<ExpressionFormatter> expressionFormatter,
		QSharedPointer<ExpressionParser> expressionParser,
		ComponentInstantiationInterface* instantiationInterface,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentInstantiationsItem();

	/*!
     *  Get the tool tip for the item.
	 * 
	 *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *    @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Get The editor of this item.
	 *
	 *    @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 * 
	 *    @param [in] index   The index to add the child into.
	 */
	virtual void createChild(int index);

	/*!
	 *  Check the validity of this item.
	 *
	 *    @return bool True if item is in valid state.
	 */
	bool isValid() const override;

private:

	//! No copying.
	ComponentInstantiationsItem(const ComponentInstantiationsItem& other);
	ComponentInstantiationsItem& operator=(const ComponentInstantiationsItem& other);

    //! Create a child component instantiation item.
    QSharedPointer<SingleComponentInstantiationItem> createChildItem(
        QSharedPointer<ComponentInstantiation> instantiation);

    //! Validator for child component instantiations.
    QSharedPointer<InstantiationsValidator> validator_;
    
	//! Validator for all instantiations together.
	QSharedPointer<AllInstantiationsValidator> allInstantiationsValidator_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Interface for accessing component instantiations.
    ComponentInstantiationInterface* instantiationInterface_;
};

#endif // COMPONENTINSTANTIATIONSITEM_H
