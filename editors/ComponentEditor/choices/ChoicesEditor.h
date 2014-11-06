//-----------------------------------------------------------------------------
// File: ChoicesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.11.2014
//
// Description:
// Editor for component choices.
//-----------------------------------------------------------------------------

#ifndef CHOICESEDITOR_H
#define CHOICESEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class ChoicesModel;

//-----------------------------------------------------------------------------
//! Editor for component choices.
//-----------------------------------------------------------------------------
class ChoicesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component   The component whose choices to edit.
	 *      @param [in] parent      The parent widget.
	 *
	 *      @return <Description>.
	 */
	ChoicesEditor(QSharedPointer<Component> component, QWidget* parent = 0);

	//! The destructor
	virtual ~ChoicesEditor();

   /*! Check for the validity of the choices.
	* 
	*       @return True, if all the choices are valid, otherwise false.
	*/
    virtual bool isValid() const;
    
	//! Reloads the information from the model to the editor.
    virtual void refresh();

protected:

    //! Called when the editor is shown.
    void showEvent( QShowEvent* event );

private:

    //! No copying
    ChoicesEditor(const ChoicesEditor& other);
    ChoicesEditor& operator=(const ChoicesEditor& other);

    //! Creates the editor layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! View for the choices.
    EditableTableView* view_;

    //! Model for the component choices.
    ChoicesModel* model_;
};

#endif // CHOICESEDITOR_H
