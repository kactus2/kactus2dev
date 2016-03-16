//-----------------------------------------------------------------------------
// File: ComponentEditorChoicesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// The choices item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef ComponentEditorChoicesItem_H
#define ComponentEditorChoicesItem_H

#include "componenteditoritem.h"

#include <QList>

class ChoiceValidator;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The choices item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorChoicesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] model               Pointer to the model that owns the items.
	 *      @param [in] libHandler          The library interface.
	 *      @param [in] component           The currently active component.
     *      @param [in] expressionParser    The selected expression parser.
	 *      @param [in] parent              The owner of this item.
	 */
	ComponentEditorChoicesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorChoicesItem();

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
	 *      @return QString Contains the text to display.
	 */
    virtual QString text() const;

    /*!
     *  Check whether this item is in a valid state.
     *
     *      @return True, if the item is valid, false otherwise.
     */
    virtual bool isValid() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	 */
    virtual ItemEditor* editor();

private:

    //! No copying. No assignment.
	ComponentEditorChoicesItem(const ComponentEditorChoicesItem& other);
	ComponentEditorChoicesItem& operator=(const ComponentEditorChoicesItem& other);

    //! The choice validator.
    QSharedPointer<ChoiceValidator> choiceValidator_;
};

#endif // ComponentEditorChoicesItem_H
