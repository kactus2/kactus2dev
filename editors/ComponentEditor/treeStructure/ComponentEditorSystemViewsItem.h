//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 13.7.2012
//
// Description:
// The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORSYSTEMVIEWSITEM_H
#define COMPONENTEDITORSYSTEMVIEWSITEM_H

#include "componenteditoritem.h"

class SystemView;
class FileSetInterface;

//-----------------------------------------------------------------------------
//! The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorSystemViewsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
	 *      @param [in] component   The component being edited.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
	 *      @param [in] parent      The parent item.
     */
	ComponentEditorSystemViewsItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentEditorItem* parent);

	/*!
     *  The destructor
     */
	virtual ~ComponentEditorSystemViewsItem() = default;

	/*!
     *  Get the font to be used for text of this item. The font is bold, if system views exist, otherwise not bold.
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
	 *      @return QString Contains the text to display.
     */
	virtual QString text() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return The editor to use for this item.
     */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
     */
	virtual void createChild(int index);

private:
    //! No copying. No assignment.
	ComponentEditorSystemViewsItem(const ComponentEditorSystemViewsItem& other);
	ComponentEditorSystemViewsItem& operator=(const ComponentEditorSystemViewsItem& other);

    /*!
     *  Construct the interface for accessing file sets.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     */
    void constructFileSetInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing file sets.
    FileSetInterface* fileSetInterface_;
};

#endif // COMPONENTEDITORSYSTEMVIEWSITEM_H
