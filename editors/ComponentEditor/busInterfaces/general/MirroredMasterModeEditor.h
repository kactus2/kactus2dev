//-----------------------------------------------------------------------------
// File: MirroredMasterModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.03.2017
//
// Description:
// Display for mirrored master bus interface.
//-----------------------------------------------------------------------------

#ifndef MIRRORED_MASTER_MODE_EDITOR_H
#define MIRRORED_MASTER_MODE_EDITOR_H

#include "ModeEditorBase.h"

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------
class MirroredMasterModeEditor : public ModeEditorBase
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] parent          Parent widget of this editor.
     */
    MirroredMasterModeEditor(BusInterfaceInterface* busInterface, std::string const& busName, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~MirroredMasterModeEditor() = default;

    //! No copying.
    MirroredMasterModeEditor(const MirroredMasterModeEditor& other) = delete;

    //! No assignment.
    MirroredMasterModeEditor& operator=(const MirroredMasterModeEditor& other) = delete;

	/*!
     *   Check for the validity of the edited item.
	 *
	 *       @return True if item is valid.
	 */
	bool isValid() const final;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	void refresh() final;

	/*!
     *  Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
	 */
	General::InterfaceMode getInterfaceMode() const final;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	void saveModeSpecific() final;

};

#endif // MIRRORED_MASTER_MODE_EDITOR_H
