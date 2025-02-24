//-----------------------------------------------------------------------------
// File: SlaveModeEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------

#ifndef SLAVE_MODE_EDITOR_H
#define SLAVE_MODE_EDITOR_H

#include "ModeEditorBase.h"
#include "bridgeseditor.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>
#include <editors/ComponentEditor/instantiations/filesetrefeditor.h>

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/TargetInterface.h>

#include <QSharedPointer>
#include <QCheckBox>

class BusInterface;
class Component;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Editor to the slave details of a bus interface.
//-----------------------------------------------------------------------------
class SlaveModeEditor : public ModeEditorBase 
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] busInterface    Interface for accessing bus interfaces.
     *    @param [in] busName         Name of the edited bus interface.
     *    @param [in] revision        Currently active IP-XACT revision.
     *    @param [in] parent          The owner of this editor.
     */
    SlaveModeEditor(BusInterfaceInterface* busInterface, std::string const& busName, Document::Revision revision, QWidget* parent);

	/*!
     *  The destructor.
     */
	~SlaveModeEditor() final = default;


    //! No copying. No assignment.
    SlaveModeEditor(const SlaveModeEditor& other) = delete;
    SlaveModeEditor& operator=(const SlaveModeEditor& other) = delete;

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
     *  Handler for changes in the memory map reference.
     *
     *    @param [in] newMemoryMapName    The name of the selected memory map.
     */
	void onMemoryMapChange(QString const& newMemoryMapName);

    /*!
     *  Handler for selecting the memory map.
     *
     *    @param [in] checked     True, if the memory map was selected.
     */
    void onMemoryMapSelected(bool checked);

    /*!
     *  Handler for selecting the transparent bridges.
     *
     *    @param [in] checked     True, if the transparent bridges was selected.
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

    //! Editor to set the file set references.
    FileSetRefEditor fileSetRefs_;
};

#endif // SLAVE_MODE_EDITOR_H
