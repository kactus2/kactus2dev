//-----------------------------------------------------------------------------
// File: SingleRemapStateEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// Editor to edit a remap state within a component.
//-----------------------------------------------------------------------------

#ifndef SINGLEREMAPSTATEEDITOR_H
#define SINGLEREMAPSTATEEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/remapStates/RemapConditionEditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/remapstate.h>

//-----------------------------------------------------------------------------
//! Editor to edit a remap state within a component.
//-----------------------------------------------------------------------------
class SingleRemapStateEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component that contains the view.
	 *      @param [in] remapState              Pointer to the remap state being edited.
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
     *      @param [in] parameterFinder         The finder for the parameter references.
     *      @param [in] expressionFormatter     Changes the referenced ids to parameter names.
	 *      @param [in] parent                  Pointer to the owner of this editor.
	 */
    SingleRemapStateEditor(QSharedPointer<Component> component,
        QSharedPointer<RemapState> remapState,
        LibraryInterface* libHandler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent = 0);

	//! The destructor
    virtual ~SingleRemapStateEditor();

    /*!
	 *  Check for the validity of the edited remap state.
	 *
     *      @return True, if the remap state is valid, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:
    
    /*!
     *  The show event for this editor.
     *
     *      @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent * event);

private:

	//! No copying
    SingleRemapStateEditor(const SingleRemapStateEditor& other);

	//! No assignment
    SingleRemapStateEditor& operator=(const SingleRemapStateEditor& other);

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    //! Pointer to the remap state being edited.
    QSharedPointer<RemapState> remapState_;

	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

    //! The editor for the remap condition.
    RemapConditionEditor* remapConditionEditor_;
};

#endif // SINGLEREMAPSTATEEDITOR_H
