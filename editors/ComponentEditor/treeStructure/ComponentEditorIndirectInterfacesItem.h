//-----------------------------------------------------------------------------
// File: ComponentEditorIndirectInterfacesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2017
//
// Description:
// The Indirect interfaces -item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITOR_INDIRECT_INTERFACES_ITEM_H
#define COMPONENTEDITOR_INDIRECT_INTERFACES_ITEM_H

#include "componenteditoritem.h"

class IndirectInterface;
class IndirectInterfaceValidator;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The Bus interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorIndirectInterfacesItem : public ComponentEditorItem
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
	 *      @param [in] expressionParser        The expression parser.
	 *      @param [in] parent                  The owner of this item.
	 *      @param [in] parentWnd               The parent window.
	 */
	ComponentEditorIndirectInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		ComponentEditorItem* parent,
        QWidget* parentWnd);

	//! The destructor
	virtual ~ComponentEditorIndirectInterfacesItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if bus interfaces exist, otherwise not bold.
	*
	*      @return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return The text to display.
	*/
	virtual QString text() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 *
	 *      @param [in] index The index to add the child into.
	*/
	virtual void createChild(int index);

private:
	//! No copying
	ComponentEditorIndirectInterfacesItem(const ComponentEditorIndirectInterfacesItem& other);

	//! No assignment
	ComponentEditorIndirectInterfacesItem& operator=(const ComponentEditorIndirectInterfacesItem& other);

    /*!
     *  Create the validator for bus interfaces.
     */
    //void createIndirectInterfaceValidator();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Contains the bus interfaces to edit.
	QSharedPointer<QList<QSharedPointer<IndirectInterface> > > indirectInterfaces_;

    //! The parent window.
    QWidget* parentWnd_;

    //! The expression parser used to define the results of expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for bus interfaces.
    QSharedPointer<IndirectInterfaceValidator> validator_;
};

#endif // COMPONENTEDITOR_INDIRECT_INTERFACES_ITEM_H
