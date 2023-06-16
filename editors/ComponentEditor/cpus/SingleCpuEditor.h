//-----------------------------------------------------------------------------
// File: SingleCpuEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Editor to edit a single cpu in a component.
//-----------------------------------------------------------------------------

#ifndef SINGLE_CPU_EDITOR_H
#define SINGLE_CPU_EDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include "CpuDetailsEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

class Cpu;
class ExpressionParser;
class ParameterFinder;
//-----------------------------------------------------------------------------
//! Editor to edit a single cpu in a component.
//-----------------------------------------------------------------------------
class SingleCpuEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component that contains the view.
	 *      @param [in] cpu                     The remap state being edited.
	 *      @param [in] libHandler              The instance that manages the library.
     *      @param [in] parameterFinder         The finder for the parameter references.
     *      @param [in] expressionFormatter     Changes the referenced ids to parameter names.
	 *      @param [in] parent                  The owner of this editor.
	 */
    SingleCpuEditor(QSharedPointer<Component> component,
        QSharedPointer<Cpu> cpu,
        LibraryInterface* libHandler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent = 0);

	//! The destructor
    virtual ~SingleCpuEditor() = default;

    //! No copying
    SingleCpuEditor(const SingleCpuEditor& other) = delete;

    //! No assignment
    SingleCpuEditor& operator=(const SingleCpuEditor& other) = delete;

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
    virtual void showEvent(QShowEvent * event);

private:

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    //! The remap state being edited.
    QSharedPointer<Cpu> cpu_;

	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

    CpuDetailsEditor detailsEditor_;
};

#endif // SINGLE_CPU_EDITOR_H
