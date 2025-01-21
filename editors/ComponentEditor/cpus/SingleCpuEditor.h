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
#include "RegionsEditor.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <editors/common/ExpressionSet.h>

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
	 *    @param [in] component               The component that contains the view.
     *    @param [in] cpu                     The remap state being edited.
     *    @param [in] regionValidator         The validator for regions in the cpu.
	 *    @param [in] libHandler              The instance that manages the library.
     *    @param [in] expressions             Changes the referenced ids to parameter names.
	 *    @param [in] parent                  The owner of this editor.
	 */
    SingleCpuEditor(QSharedPointer<Component> component,
        QSharedPointer<Cpu> cpu, 
        QSharedPointer<RegionValidator> regionValidator,
        LibraryInterface* libHandler,
        ExpressionSet expressions,
        QWidget* parent = nullptr);

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
     *    @param [in] event   The show event.
     */
    void showEvent(QShowEvent * event) final;

private:

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The remap state being edited.
    QSharedPointer<Cpu> cpu_;

	//! Editor to set the name, display name and description of the view.
	NameGroupEditor nameEditor_;

    //! Editor for memory references and other details.
    CpuDetailsEditor detailsEditor_;

    //! Editor for region data.
    RegionsEditor regionsEditor_;

};

#endif // SINGLE_CPU_EDITOR_H
