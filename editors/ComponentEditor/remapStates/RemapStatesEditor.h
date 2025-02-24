//-----------------------------------------------------------------------------
// File: RemapStatesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The editor to add / remove / edit remap states of a component.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATESEDITOR_H
#define REMAPSTATESEDITOR_H

#include "RemapStatesModel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor used to add / remove / edit remap states of a component.
//-----------------------------------------------------------------------------
class RemapStatesEditor : public ItemEditor
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] component           Pointer to the component being edited.
     *    @param [in] handler             Pointer to the instance managing the library.
     *    @param [in] parameterFinder     Finder used to locate parameter ids.
     *    @param [in] parent              Pointer to the parent of this editor.
     */
    RemapStatesEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent = 0);
	
	/*!
	 *  The destructor.
	 */
	virtual ~RemapStatesEditor() = default;

    //! No copying
    RemapStatesEditor(const RemapStatesEditor& other) = delete;
    //! No assignment
    RemapStatesEditor& operator=(const RemapStatesEditor& other) = delete;

    /*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

public slots:

    /*!
     *  Called to update the view sorting when the model changes.
     */
    virtual void onItemChanged();

protected:

    /*!
	 *  Handler for widget's show event.
	 *
	 *    @param [in] event   The given show event.
	 */
	virtual void showEvent(QShowEvent* event);

private:

    //! Setup the widget layout.
    void setupLayout();

	//! The view to display the remap states.
	EditableTableView view_;

    //! The model for managing the views summary.
    RemapStatesModel model_;
};

#endif // REMAPSTATESEDITOR_H
