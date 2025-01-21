//-----------------------------------------------------------------------------
// File: componenteditortreemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The general-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORGENERALITEM_H
#define COMPONENTEDITORGENERALITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>
#include <QString>

class ComponentEditorTreeModel;
class ItemEditor;
class Component;
class ComponentPreviewBox;

//-----------------------------------------------------------------------------
//! The general-item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorGeneralItem : public ComponentEditorItem
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
	ComponentEditorGeneralItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor.
	~ComponentEditorGeneralItem() override = default;

    //! No copying. No assignment. No mercy.
    ComponentEditorGeneralItem(const ComponentEditorGeneralItem& other) = delete;
    ComponentEditorGeneralItem& operator=(const ComponentEditorGeneralItem& other) = delete;

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
	 *  Get pointer to the visualizer of this item.
	 *
	 *    @return Pointer to the visualizer to use for this item.
	 */
	ItemVisualizer* visualizer() override;

	/*!
	 *  Refresh the contents of the editor associated with this item.
	 */
	void refreshEditor() override;

private:

	//! Component preview for visualization.
	ComponentPreviewBox* previewBox_;
};

#endif // COMPONENTEDITORGENERALITEM_H
