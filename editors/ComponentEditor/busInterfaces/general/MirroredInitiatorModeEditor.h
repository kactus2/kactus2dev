//-----------------------------------------------------------------------------
// File: MirroredInitiatorModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.6.2023
//
// Description:
// Editor to edit mirrored initiator settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef MIRRORED_INITIATOR_MODE_EDITOR_H
#define MIRRORED_INITIATOR_MODE_EDITOR_H

#include "ModeEditorBase.h"

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit mirrored initiator settings of a bus interface.
//-----------------------------------------------------------------------------
class MirroredInitiatorModeEditor : public ModeEditorBase
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] busInterface    Interface for accessing bus interfaces.
     *    @param [in] parent          Parent widget of this editor.
     */
    MirroredInitiatorModeEditor(BusInterfaceInterface* busInterface, std::string const& busName, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~MirroredInitiatorModeEditor() = default;

    //! No copying.
    MirroredInitiatorModeEditor(const MirroredInitiatorModeEditor& other) = delete;

    //! No assignment.
    MirroredInitiatorModeEditor& operator=(const MirroredInitiatorModeEditor& other) = delete;

	/*!
     *   Check for the validity of the edited item.
	 *
	 *    @return True if item is valid.
	 */
	bool isValid() const final;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	void refresh() final;

	/*!
     *  Get the interface mode of the editor
	 * 
	 *    @return General::InterfaceMode Specifies the interface mode.
	 */
	General::InterfaceMode getInterfaceMode() const final;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	void saveModeSpecific() final;

};

#endif // MIRRORED_INITIATOR_MODE_EDITOR_H
