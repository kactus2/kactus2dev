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
	 *      @param [in] model       Pointer to the model that owns the items.
	 *      @param [in] libHandler   [Description].
	 *      @param [in] component   [Description].
	 *      @param [in] parent   [Description].
	 */
	ComponentEditorChoicesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, ComponentEditorItem* parent);

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
};

#endif // ComponentEditorChoicesItem_H
