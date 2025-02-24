//-----------------------------------------------------------------------------
// File: TypeDefinitionsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 13.10.2023
//
// Description:
// The Type Definitions-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef TYPEDEFINITIONSITEM_H
#define TYPEDEFINITIONSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>
#include <QString>

//-----------------------------------------------------------------------------
//! The Type Definitions-item  in the component editor navigation tree.
//-----------------------------------------------------------------------------
class TypeDefinitionsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] model       Pointer to the model that owns the items.
	 *    @param [in] libHandler  Pointer to the instance that manages the library.
	 *    @param [in] component   Pointer to the component being edited.
	 *    @param [in] parent      Pointer to the parent item.
	 */
	TypeDefinitionsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor.
	~TypeDefinitionsItem() override = default;

    //! No copying. No assignment. No mercy.
    TypeDefinitionsItem(const TypeDefinitionsItem& other) = delete;
    TypeDefinitionsItem& operator=(const TypeDefinitionsItem& other) = delete;

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
     *  Get the font to be used for text of this item.
	 *
	 *    @return QFont instance that defines the font to be used.
	 */
    virtual QFont getFont() const;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *    @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *    @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
	 *  Refresh the contents of the editor associated with this item.
	 */
	void refreshEditor() override;

};

#endif // TYPEDEFINITIONSITEM_H
