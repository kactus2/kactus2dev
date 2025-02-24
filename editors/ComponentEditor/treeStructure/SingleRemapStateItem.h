//-----------------------------------------------------------------------------
// File: SingleRemapStateItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The single remap state item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef SINGLEREMAPSTATEITEM_H
#define SINGLEREMAPSTATEITEM_H

#include "componenteditoritem.h"

class RemapState;
class RemapStateValidator;
class RemapPort;
//-----------------------------------------------------------------------------
//! The single remap state item used in the component editor navigation tree.
//-----------------------------------------------------------------------------
class SingleRemapStateItem: public ComponentEditorItem
{
	Q_OBJECT

public:
	
    /*!
     *  The constructor.
     *
     *    @param [in] remapState              Pointer to the selected remap state.
     *    @param [in] model                   Pointer to the model that owns the items.
     *    @param [in] libHandler              Pointer to the instance that manages the library.
     *    @param [in] component               Pointer to the component being edited.
     *    @param [in] referenceCounter        The counter for parameter references.
     *    @param [in] parameterFinder         Finds the referenced parameter ids.
     *    @param [in] expressionFormatter     Changes the referenced ids to parameter names.
     *    @param [in] parent                  Pointer to the parent item.
     */
    SingleRemapStateItem(QSharedPointer<RemapState> remapState,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<RemapStateValidator> validator,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~SingleRemapStateItem() = default;

	/*!
	 *  Get the tool tip for the item.
	 *
     *    @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
	 *  Get the text to be displayed in the tree for this item.
	 *
     *    @return The text to display.
	 */
	virtual QString text() const;

    /*!
	 *  Check the validity of this item and sub items.
	 *
     *    @return True, if the item is in a valid state, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Get the pointer to the editor of this item.
	 *
     *    @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor();

private:
	//! No copying
    SingleRemapStateItem(const SingleRemapStateItem& other);

	//! No assignment
    SingleRemapStateItem& operator=(const SingleRemapStateItem& other);

    //! Pointer to the currently selected remap state.
    QSharedPointer<RemapState> remapState_;

    QSharedPointer<RemapStateValidator> validator_;
};

#endif // SINGLEREMAPSTATEITEM_H
