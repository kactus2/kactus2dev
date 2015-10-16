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

#include <editors/ComponentEditor/itemeditor.h>

#include <editors/ComDefinitionEditor/ComPropertyModel.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QSharedPointer>
#include <QShowEvent>
#include <QSortFilterProxyModel>
#include <QString>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor for SW properties.
//-----------------------------------------------------------------------------
class SWPropertiesEditor : public ItemEditor
{
	Q_OBJECT

public:
	
	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] handler     The instance managing the library.
	 *      @param [in] parent      The owner of the editor.
	*/
	SWPropertiesEditor(QSharedPointer<Component> component,	LibraryInterface* handler, QWidget *parent = 0);
	
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

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private slots:

	//! Handler for changes in properties.
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
