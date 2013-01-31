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

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor for SW properties.
//-----------------------------------------------------------------------------
class SWPropertiesEditor : public ItemEditor
{
	Q_OBJECT

public:
	
	/*! \brief The constructor
	 *
	 * Method: 		SWPropertiesEditor
	 * Full name:	SWPropertiesEditor::SWPropertiesEditor
	 * Access:		private 
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	SWPropertiesEditor(QSharedPointer<Component> component,
		LibraryInterface* handler,
		QWidget *parent = 0);
	
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

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in properties.
	void onPropertyChange();

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
