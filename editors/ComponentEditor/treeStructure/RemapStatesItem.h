//-----------------------------------------------------------------------------
// File: RemapStatesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// The Remap states-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATESITEM_H
#define REMAPSTATESITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/remapstate.h>

//-----------------------------------------------------------------------------
//! The remap states-item used in the component editor navigation tree.
//-----------------------------------------------------------------------------
class RemapStatesItem : public ComponentEditorItem
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] model                   Pointer to the model that owns the items.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] parent                  Pointer to the parent item.
	 */
    RemapStatesItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~RemapStatesItem();

    /*!
	 *  Get the font to be used for text of this item.
	 *
     *      @return The font to be used.
	 */
	virtual QFont getFont() const;

    /*!
	 *  Get the tool tip for the item.
	 *
     *      @return The text for the tool tip for the item.
	 */
	virtual QString getTooltip() const;

    /*!
	 *  Get the text to be displayed in the tree for this item.
	 *
     *      @return The text to be displayed.
	 */
	virtual QString text() const;

    /*!
	 *  Get the pointer to the editor of this item.
	 *
     *      @return Pointer to the editor to use with this item.
	 */
	virtual ItemEditor* editor();

    /*!
     *  Create a child item.
     *
     *      @param [in] index   The index of the child item.
     */
    virtual void createChild(int index);

private:
	//! No copying
    RemapStatesItem(const RemapStatesItem& other);

	//! No assignment
    RemapStatesItem& operator=(const RemapStatesItem& other);

    //! The list containing the remap states to edit.
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates_;
};

#endif // REMAPSTATESITEM_H
