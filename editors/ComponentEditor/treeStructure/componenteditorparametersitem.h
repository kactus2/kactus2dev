//-----------------------------------------------------------------------------
// File: componenteditorparametersitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The parameters-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORPARAMETERITEM_H
#define COMPONENTEDITORPARAMETERITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/common/Parameter.h>

class Component;
class Choice;
class ExpressionParser;
class ParameterValidator2014;
//-----------------------------------------------------------------------------
//! The parameters-item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorParametersItem : public ComponentEditorItem
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] refCounter              The reference counter.
	 *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionParser        The expression parser.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] parent                  The parent item.
	 */
	ComponentEditorParametersItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> refCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
         QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorParametersItem();

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  @Get the tool tip for the item.
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
     *  Check the validity of this item and sub items.
	 *
	 *      @return True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

private:

	//! No copying. No assignment.
	ComponentEditorParametersItem(const ComponentEditorParametersItem& other);
	ComponentEditorParametersItem& operator=(const ComponentEditorParametersItem& other);

    //! Expression parser for parameter values.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for parameters.
    QSharedPointer<ParameterValidator2014> validator_;

};

#endif // COMPONENTEDITORPARAMETERITEM_H
