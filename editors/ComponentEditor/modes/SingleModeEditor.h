//-----------------------------------------------------------------------------
// File: SingleModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to edit a single mode in a component.
//-----------------------------------------------------------------------------

#ifndef SINGLE_MODE_EDITOR_H
#define SINGLE_MODE_EDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/common/ExpressionSet.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>

#include <IPXACTmodels/Component/validators/ModeValidator.h>

#include "PortSliceEditor.h"
#include "FieldSliceEditor.h"

class Mode;
class ExpressionParser;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to edit a single Mode in a component.
//-----------------------------------------------------------------------------
class SingleModeEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component that contains the view.
     *      @param [in] Mode                     The remap state being edited.
	 *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] expressions             Changes the referenced ids to parameter names.
	 *      @param [in] parent                  The owner of this editor.
	 */
    SingleModeEditor(QSharedPointer<Component> component,
        QSharedPointer<Mode> mode,
        QSharedPointer<ModeValidator> validator,
        LibraryInterface* libHandler,
        ExpressionSet expressions,
        QWidget* parent = nullptr);

	//! The destructor
    virtual ~SingleModeEditor() = default;

    //! No copying
    SingleModeEditor(const SingleModeEditor& other) = delete;

    //! No assignment
    SingleModeEditor& operator=(const SingleModeEditor& other) = delete;

	/*!
	 *  Reload the information from the model to the editor.
	 */
	void refresh() final;

protected:
    
    /*!
     *  The show event for this editor.
     *
     *      @param [in] event   The show event.
     */
    void showEvent(QShowEvent * event) final;

private slots:

    void onConditionChanged(); 

private:

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The remap state being edited.
    QSharedPointer<Mode> mode_;

	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

    ExpressionEditor conditionEditor_;

    PortSliceEditor portSliceEditor_;

    FieldSliceEditor fieldSliceEditor_;
};

#endif // SINGLE_Mode_EDITOR_H
