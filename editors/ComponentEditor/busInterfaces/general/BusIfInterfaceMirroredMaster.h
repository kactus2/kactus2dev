//-----------------------------------------------------------------------------
// File: BusIfInterfaceMirroredMaster.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.03.2017
//
// Description:
// Display for mirrored master bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACEMIRROREDMASTER_H
#define BUSIFINTERFACEMIRROREDMASTER_H

#include "busifinterfacemodeeditor.h"

class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceMirroredMaster : public BusIfInterfaceModeEditor
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] parent          Parent widget of this editor.
     */
    BusIfInterfaceMirroredMaster(BusInterfaceInterface* busInterface, std::string const& busName, QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~BusIfInterfaceMirroredMaster();

	/*!
     *   Check for the validity of the edited item.
	 *
	 *       @return True if item is valid.
	 */
	virtual bool isValid() const;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
	 */
	virtual void refresh();

	/*!
     *  Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
	 */
	virtual General::InterfaceMode getInterfaceMode() const;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	virtual void saveModeSpecific();

private:

	//! No copying.
    BusIfInterfaceMirroredMaster(const BusIfInterfaceMirroredMaster& other);

	//! No assignment.
    BusIfInterfaceMirroredMaster& operator=(const BusIfInterfaceMirroredMaster& other);
};

#endif // BUSIFINTERFACEMIRROREDMASTER_H
