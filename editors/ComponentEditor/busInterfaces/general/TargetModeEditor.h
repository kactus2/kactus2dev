//-----------------------------------------------------------------------------
// File: TargetModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.06.2023
//
// Description:
// Editor to the target details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef TARGET_MODE_EDITOR_H
#define TARGET_MODE_EDITOR_H

#include "ModeEditorBase.h"
#include "bridgeseditor.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>
#include <editors/ComponentEditor/instantiations/filesetrefeditor.h>

#include <IPXACTmodels/Component/TargetInterface.h>

#include <QSharedPointer>
#include <QCheckBox>

class BusInterface;
class Component;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------
class TargetModeEditor : public ModeEditorBase 
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] busName         Name of the edited bus interface.
     *      @param [in]   parent      The owner of this editor.
     */
    TargetModeEditor(BusInterfaceInterface* busInterface, std::string const& busName, QWidget* parent);

	/*!
     *  The destructor.
     */
	~TargetModeEditor() final = default;


    //! No copying. No assignment.
    TargetModeEditor(const TargetModeEditor& other) = delete;
    TargetModeEditor& operator=(const TargetModeEditor& other) = delete;

	/*!
     *  Check for the validity of the edited item.
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

private slots:

	/*!
     *  Handler for changes in the memory map reference.
     *
     *      @param [in] newMemoryMapName    The name of the selected memory map.
     */
	void onMemoryMapChange(QString const& newMemoryMapName);

    /*!
     *  Handler for selecting the memory map.
     *
     *      @param [in] checked     True, if the memory map was selected.
     */
    void onMemoryMapSelected(bool checked);

    /*!
     *  Handler for selecting the transparent bridges.
     *
     *      @param [in] checked     True, if the transparent bridges was selected.
     */
    void onTransparentBridgeSelected(bool checked);

    /*!
     *  Handler for changes in the file set references.
     */
    void onFileSetReferencesChanged();

private:

    /*!
     *  Setup the current file set references.
     */
    void setupFileSetReferences();

    //! Save the current file set references.
    void saveFileSetReferences();

    /*!
     *  Sets the editor layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Select memory map as access target.
    QCheckBox memoryMapSelector_;

    //! Select transparent bridge(s) as access target.
    QCheckBox bridgesSelector_;

	//! The editor to select a memory map from the component.
	ReferenceSelector memoryMapReferenceSelector_;

    //! List of available transparent bridges.
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > slaveBridges_;

	//! The editor to edit the bridges of a slave interface.
	BridgesEditor bridges_;

    //! The editor for mode reference selections.
    ListManager modeReferences_;

    //! Editor to set the file set references.
    FileSetRefEditor fileSetRefs_;
};

#endif // TARGET_MODE_EDITOR_H
