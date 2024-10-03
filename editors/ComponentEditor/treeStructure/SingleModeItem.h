//-----------------------------------------------------------------------------
// File: SingleModeItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// The single Mode item used in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef SINGLE_MODE_ITEM_H
#define SINGLE_MODE_ITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

class Mode;
class ModeValidator;
class ModeConditionParser;

//-----------------------------------------------------------------------------
//! The single Mode item used in the component editor navigation tree.
//-----------------------------------------------------------------------------
class SingleModeItem: public ComponentEditorItem
{
	Q_OBJECT

public:
	
    /*!
     *  The constructor.
     *
     *      @param [in] Mode                     The selected Mode.
     *      @param [in] model                   The model that owns the items.
     *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The counter for parameter references.
     *      @param [in] expressions				The collection of objects for expression handling.
     *      @param [in] modeConditionParser     The mode condition parser to use.
     *      @param [in] parent                  The parent item.
     */
    SingleModeItem(QSharedPointer<Mode> mode, ComponentEditorTreeModel* model, 
        LibraryInterface* libHandler, QSharedPointer<Component> component, 
        QSharedPointer<ReferenceCounter> referenceCounter, ExpressionSet expressions, 
        QSharedPointer<ModeValidator> validator, QSharedPointer<ExpressionParser> modeConditionParser, 
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~SingleModeItem() = default;

    //! No copying
    SingleModeItem(const SingleModeItem& other) = delete;

    //! No assignment
    SingleModeItem& operator=(const SingleModeItem& other) = delete;

	/*!
	 *  Get the tool tip for the item.
	 *
     *      @return The text for the tool tip to print to user.
	 */
	QString getTooltip() const final;

	/*!
	 *  Get the text to be displayed in the tree for this item.
	 *
     *      @return The text to display.
	 */
	QString text() const final;

    /*!
	 *  Check the validity of this item and sub items.
	 *
     *      @return True, if the item is in a valid state, false otherwise.
	 */
	bool isValid() const final;

	/*!
	 *  Get the pointer to the editor of this item.
	 *
     *      @return Pointer to the editor to use for this item.
	 */
	ItemEditor* editor() final;

private:

    //! Pointer to the currently selected remap state.
    QSharedPointer<Mode> mode_;

    QSharedPointer<ModeValidator> validator_;

    ExpressionSet expressions_;

    //! The mode condition parser to use.
    QSharedPointer<ExpressionParser> modeConditionParser_;

};

#endif // SINGLE_MODE_ITEM_H
