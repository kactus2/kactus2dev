//-----------------------------------------------------------------------------
// File: SingleMemoryMapEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Editor for editing the details of a single memory map.
//-----------------------------------------------------------------------------

#ifndef SINGLEMEMORYMAPEDITOR_H
#define SINGLEMEMORYMAPEDITOR_H

#include "memorymapeditor.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <QLineEdit>
#include <QLabel>

class Component;
class MemoryMap;
class MemoryRemap;
class MemoryMapBase;
class ExpressionEditor;
class ExpressionParser;
class MemoryMapInterface;
class SubspaceMapsEditor;
class RemapModeReferenceEditor;
class ModeReferenceInterface;

//-----------------------------------------------------------------------------
//! Editor for editing the details of a single memory map.
//-----------------------------------------------------------------------------
class SingleMemoryMapEditor : public ItemEditor
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] component           The component that contains the editor.
     *    @param [in] memoryRemap         The memory remap being edited.
     *    @param [in] parentMapName       Name of the parent of the memory remap being edited.
     *    @param [in] libHandler          The instance that manages the library.
     *    @param [in] parameterFinder     The finder for the parameter references.
     *    @param [in] expressionParser    The expression parser.
     *    @param [in] mapInterface        Interface for memory maps.
     *    @param [in] isMemoryRemap       Flag for informing if the edited item is a memory map or a remap.
     *    @param [in] parent              The owner of this editor.
     */
    SingleMemoryMapEditor(QSharedPointer<Component> component,
        QSharedPointer<MemoryMapBase> memoryRemap,
        QString const& parentMapName,
        LibraryInterface* libHandler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        MemoryMapInterface* mapInterface,
        bool isMemoryRemap,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleMemoryMapEditor() = default;

	/*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

public slots:
    
    /*
     *  Handles memory map name change from memory maps editor.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void onMemoryMapNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Handles of memory remap name change from memory maps editor.
     *
     *    @param [in] parentName  Name of the containing memory map.
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void onMemoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

    /*
     *  Handles memory item name change from memory name editor.
     */
    void onNameChange();

protected:
    
    /*!
     *  The show event for this editor.
     *
     *    @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent * event);

private slots:

    /*!
     *  Updates the name of the current target interface binding.
     */
    void refreshTargetBinding();

    /*!
     *  Updates the address unit bits [AUB].
     */
    void updateAddressUnitBits();
    
    //! Called when isPresent has been edited.
    void onIsPresentEdited();

    /*!
     *  Sets the new remap state to the memory remap.
     *
     *    @param [in] newRemapState   The name of the selected remap state.
     */
    void onRemapStateSelected(QString const& newRemapState);

    /*!
     *	Gets updated remap mode references from mode reference interface, and saves to remap.
     */
    void onRemapModeReferencesEdited();

signals:
    
    /*!
     *  Informs contents of the change in address unit bits.
     */
    void addressUnitBitsChanged();

    /*!
     *  Emitted when address unit bits are changed for memory map.
     *
     *    @param [in] newAddressUnitBits  The new address unit bits.
     */
    void assignNewAddressUnitBits(QString const& newAddressUnitBits);

    /*
     *  Informs of address block name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void addressBlockNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Informs of subspace map name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void subspaceMapNameChanged(QString const& oldName, QString const& newName);

    /*!
     *  Informs change in child addressing.
     *
     *    @param [in]
     */
    void childAddressingChanged(int);

    /*!
     *  Informs change in child graphics.
     *
     *    @param [in]
     */
    void childGraphicsChanged(int);

private:

    //! No copying. No assignment.
    SingleMemoryMapEditor(const SingleMemoryMapEditor& other);
    SingleMemoryMapEditor& operator=(const SingleMemoryMapEditor& other);

    /*!
     *  Connect the editor signals.
     */
    void connectSignals();

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    /*!
     *  Updates the remap state selector.
     */
    void refreshRemapStateSelector();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Editor to set the name, display name and description of the memory remap.
	NameGroupEditor nameEditor_;

    //! The editor for the address blocks.
    MemoryMapEditor* addressBlockEditor_;

    //! Editor for subspace maps.
    SubspaceMapsEditor* subspaceMapEditor_;

    //! The editor for the memory maps address unit bits [AUB].
    ExpressionEditor* addressUnitBitsEditor_;

    //! The editor for isPresent-property.
    ExpressionEditor* isPresentEditor_;

    //! The label for the target interface binding.
    QLabel* targetInterfaceLabel_;
    
    //! The remap state selector.
    ReferenceSelector* remapStateSelector_;

    //! Name of the memory remap being edited.
    std::string remapName_;

    //! Name of the parent of the memory remap.
    std::string parentMapName_;

    //! Interface for memory maps.
    MemoryMapInterface* mapInterface_;

    //! Flag for identifying memory maps and remaps.
    bool isMemoryRemap_;

    //! Editor for remap mode references.
    RemapModeReferenceEditor* modeReferenceEditor_;

    //! The current remap used by this instance of the editor. Set if the editor is used for a remap.
    QSharedPointer<MemoryRemap> currentRemap_;

    //! Interface for accessing remap mode references.
    ModeReferenceInterface* modeRefInterface_;
};

#endif // SINGLEMEMORYMAPEDITOR_H
