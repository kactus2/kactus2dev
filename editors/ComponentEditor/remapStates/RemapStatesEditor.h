//-----------------------------------------------------------------------------
// File: RemapStatesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The editor to add / remove / edit remap states of a component.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATESEDITOR_H
#define REMAPSTATESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>
#include "RemapStatesModel.h"

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
     *      @param [in] component   Pointer to the component being edited.
     *      @param [in] handler     Pointer to the instance managing the library.
     *      @param [in] parent      Pointer to the parent of this editor.
     */
    RemapStatesEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent = 0);
	
	/*!
	 *  The destructor.
	 */
	virtual ~RemapStatesEditor();

	/*!
	 *  Check for the validity of the editor.
	 *
     *      @return True, if the editor is in valid state, false otherwise.
	 */
	virtual bool isValid() const;

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
	 *      @param [in] event   The given show event.
	 */
	virtual void showEvent(QShowEvent* event);

private:
	//! No copying
    RemapStatesEditor(const RemapStatesEditor& other);

	//! No assignment
    RemapStatesEditor& operator=(const RemapStatesEditor& other);

	//! The view to display the remap states.
	EditableTableView view_;

    //! The model for managing the views summary.
    RemapStatesModel model_;
};

#endif // REMAPSTATESEDITOR_H
