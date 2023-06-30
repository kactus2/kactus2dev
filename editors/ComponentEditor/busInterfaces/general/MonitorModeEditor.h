//-----------------------------------------------------------------------------
// File: MonitorModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor the edit a monitor settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef MonitorModeEditor_H
#define MonitorModeEditor_H

#include "ModeEditorBase.h"

#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QLabel>
#include <QSharedPointer>
#include <QComboBox>

class Component;
class BusIfGeneralTab;
class LibraryInterface;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor the edit a monitor settings of a bus interface.
//-----------------------------------------------------------------------------
class MonitorModeEditor : public ModeEditorBase
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] busName         Name of the edited bus interface.
	 *      @param [in] libHandler      The library handler instance.
	 *      @param [in] parent          The owner of this editor.
	 *
     */
    MonitorModeEditor(BusInterfaceInterface* busInterface,
        std::string const& busName, 
        Document::Revision docRevision,
        LibraryInterface* libHandler,
        QWidget *parent);

	/*!
     *  The destructor.
     */
	virtual ~MonitorModeEditor() = default;


    //! No copying. No assignment.
    MonitorModeEditor(const MonitorModeEditor& other) = delete;

    MonitorModeEditor& operator=(const MonitorModeEditor& other) = delete;


	/*!
     *  Check for the validity of the edited item.
     *
     *      @return True if item is valid.
     */
	bool isValid() const final;

	/*!
     *  Restore the changes made in the editor back to ones in the model.
     */
	void refresh() final;

	/*!
     *  Get the interface mode of the editor.
	 *
     *      @return General::InterfaceMode Specifies the interface mode.
     */
	General::InterfaceMode getInterfaceMode() const final;

	/*!
     *  Save the interface mode-specific details to the bus interface.
     */
	void saveModeSpecific() final;

private slots:

	/*!
     *  Handles the monitor interface mode change.
     *
     *      @param [in] mode    New interface mode.
     */
	void onInterfaceModeChange(General::InterfaceMode mode);

	/*!
     *  Handler for monitor system group changes.
     *
     *      @param [in] groupName   New system group.
     */
	void onSystemGroupChange(QString const& groupName);

private:

    //! Set the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* libHandler_;

	//! Combo box to select the interface type for monitor.
	InterfaceModeSelector interfaceMode_;

    //! Label for selecting system group.
    QLabel groupLabel_;

	//! Combo box to select the group for system interface types.
	QComboBox systemGroup_;
};

#endif // MonitorModeEditor_H
