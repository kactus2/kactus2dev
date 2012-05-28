//-----------------------------------------------------------------------------
// File: SWPropertiesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.5.2012
//
// Description:
// Editor for SW properties.
//-----------------------------------------------------------------------------

#ifndef SWPROPERTIESEDITOR_H
#define SWPROPERTIESEDITOR_H

#include <IPXactWrapper/ComDefinitionEditor/ComPropertyModel.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Editor for SW properties.
//-----------------------------------------------------------------------------
class SWPropertiesEditor : public ItemEditor
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
     *
     *      @param [in] dataPointer  The component being edited.
     *      @param [in] parent       The parent widget.
     */
	SWPropertiesEditor(QSharedPointer<Component> component, QWidget *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~SWPropertiesEditor();

	/*!
     *  Checks for the validity of the edited parameters.
	 *
	 *      @return True if all parameters are in valid state.
	 */
	virtual bool isValid() const;

	/*!
     *  Makes the changes from the widgets editors to the IPXact model.
	 */
	virtual void makeChanges();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:
	// Disable copying.
	SWPropertiesEditor(SWPropertiesEditor const& rhs);
	SWPropertiesEditor& operator=(SWPropertiesEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view widget.
    EditableTableView view_;

    //! The filter to do the sorting of items to display.
    QSortFilterProxyModel filter_;

    //! The model to edit the properties.
    ComPropertyModel model_;
};

//-----------------------------------------------------------------------------

#endif // SWPROPERTIESEDITOR_H
