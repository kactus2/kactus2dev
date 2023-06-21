//-----------------------------------------------------------------------------
// File: SingleCpuItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// The single cpu item used in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef SINGLE_CPU_ITEM_H
#define SINGLE_CPU_ITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

class Cpu;
class CPUValidator;

//-----------------------------------------------------------------------------
//! The single cpu item used in the component editor navigation tree.
//-----------------------------------------------------------------------------
class SingleCpuItem: public ComponentEditorItem
{
	Q_OBJECT

public:
	
    /*!
     *  The constructor.
     *
     *      @param [in] cpu                     The selected cpu.
     *      @param [in] model                   The model that owns the items.
     *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The counter for parameter references.
     *      @param [in] expressions				The collection of objects for expression handling.
     *      @param [in] parent                  The parent item.
     */
    SingleCpuItem(QSharedPointer<Cpu> cpu,
        ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        ExpressionSet expressions,
        QSharedPointer<CPUValidator> validator,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~SingleCpuItem() = default;

    //! No copying
    SingleCpuItem(const SingleCpuItem& other) = delete;

    //! No assignment
    SingleCpuItem& operator=(const SingleCpuItem& other) = delete;

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

    QSharedPointer<ExpressionParser> expressionParser_;

    //! Pointer to the currently selected remap state.
    QSharedPointer<Cpu> cpu_;

    QSharedPointer<CPUValidator> validator_;

    ExpressionSet expressions_;
};

#endif // SINGLE_CPU_ITEM_H
