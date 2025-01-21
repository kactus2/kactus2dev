//-----------------------------------------------------------------------------
// File: ModesItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// The Modes-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef MODESITEM_H
#define MODESITEM_H

#include "componenteditoritem.h"

#include <editors/common/ExpressionSet.h>

class ExpressionParser;
class Mode;
class ModeValidator;
class HierarchicalValidator;

//-----------------------------------------------------------------------------
//! The Modes-item used in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ModesItem : public ComponentEditorItem
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
    ModesItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        ExpressionSet expressions,
        ComponentEditorItem* parent);

    /*!
     *  The destructor.
     */
    virtual ~ModesItem() = default;

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

    /*!
     *  Check the validity of this item.
     *
     *    @return bool True if item is in valid state.
     */
    bool isValid() const override;

private:
	//! No copying
    ModesItem(const ModesItem& other);

	//! No assignment
    ModesItem& operator=(const ModesItem& other);

    //! The list containing the modes to edit.
    QSharedPointer<QList<QSharedPointer<Mode> > > modes_;

    ExpressionSet expressions_;

    //! The mode validator to use.
    QSharedPointer<ModeValidator> modeValidator_;

    //! The validator to validate modes together.
    QSharedPointer<HierarchicalValidator> modesValidator_;

};

#endif // MODESITEM_H
