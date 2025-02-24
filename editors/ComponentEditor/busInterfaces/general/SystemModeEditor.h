//-----------------------------------------------------------------------------
// File: SystemModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit system or mirrored system details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef SYSTEM_MODE_EDITOR_H
#define SYSTEM_MODE_EDITOR_H

#include "ModeEditorBase.h"

#include <IPXACTmodels/Component/BusInterface.h>

#include <QComboBox>
#include <QSharedPointer>

class Component;
class BusIfGeneralTab;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to edit system or mirrored system details of a bus interface.
//-----------------------------------------------------------------------------
class SystemModeEditor : public ModeEditorBase
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] busInterface    Interface for accessing bus interfaces.
     *    @param [in] busName         Name of the edited bus interface.
	 *    @param [in] libHandler      The library handler instance.
	 *    @param [in] parent          The owner of this editor.
     */
    SystemModeEditor(BusInterfaceInterface* busInterface,
        std::string const& busName,
        LibraryInterface* libHandler,
        QWidget *parent);

	/*!
     *  The destructor.
     */
	virtual ~SystemModeEditor() = default;

    //! No copying. No assignment.
    SystemModeEditor(const SystemModeEditor& other) = delete;
    SystemModeEditor& operator=(const SystemModeEditor& other) = delete;

	/*!
     *  Check for the validity of the edited item.
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

private slots:

	/*!
     *  Handler for changes in system group selector.
     *
     *    @param [in] newGroup    New system group.
     */
	void onGroupChange(QString const& newGroup);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The general tab to ask for bus type.
	BusIfGeneralTab* generalTab_;

	//! The library handling interface.
	LibraryInterface* libHandler_;

	//! Select the group this interface belongs to.
	QComboBox groupEditor_;
};

#endif // SYSTEM_MODE_EDITOR_H
