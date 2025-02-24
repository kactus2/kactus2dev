//-----------------------------------------------------------------------------
// File: RemapStatesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// The Remap states-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATESITEM_H
#define REMAPSTATESITEM_H

#include "componenteditoritem.h"

class ExpressionParser;
class RemapState;
class RemapStateValidator;

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
	 *    @param [in] model                   The model that owns the items.
	 *    @param [in] libHandler              The instance that manages the library.
	 *    @param [in] component               The component being edited.
     *    @param [in] referenceCounter        The counter for the references.
     *    @param [in] parameterFinder         Finds the referenced parameters.
     *    @param [in] expressionFormatter     Changes the referenced ids to parameter names.
     *    @param [in] expressionParser        Expression parser for remap states.
	 *    @param [in] parent                  The parent item.
	 */
    RemapStatesItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~RemapStatesItem() = default;

    /*!
	 *  Get the font to be used for text of this item.
	 *
     *    @return The font to be used.
	 */
	virtual QFont getFont() const;

    /*!
	 *  Get the tool tip for the item.
	 *
     *    @return The text for the tool tip for the item.
	 */
	virtual QString getTooltip() const;

    /*!
	 *  Get the text to be displayed in the tree for this item.
	 *
     *    @return The text to be displayed.
	 */
	virtual QString text() const;

    /*!
	 *  Get the The editor of this item.
	 *
     *    @return The editor to use with this item.
	 */
	virtual ItemEditor* editor();

    /*!
     *  Create a child item.
     *
     *    @param [in] index   The index of the child item.
     */
    virtual void createChild(int index);

private:
	//! No copying
    RemapStatesItem(const RemapStatesItem& other);

	//! No assignment
    RemapStatesItem& operator=(const RemapStatesItem& other);

    //! The list containing the remap states to edit.
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates_;

    //! The validator for remap states.
    QSharedPointer<RemapStateValidator> validator_;
};

#endif // REMAPSTATESITEM_H
