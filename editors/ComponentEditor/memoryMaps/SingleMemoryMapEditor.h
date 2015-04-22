//-----------------------------------------------------------------------------
// File: SingleMemoryMapEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/AbstractMemoryMap.h>

#include <QLineEdit>
#include <QLabel>

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
     *      @param [in] component               Pointer to the component that contains the editor.
     *      @param [in] memoryRemap             Pointer to the memory remap being edited.
     *      @param [in] parentMemoryMap         Pointer to the parent of the memory remap being edited.
     *      @param [in] libHandler              Pointer to the instance that manages the library.
     *      @param [in] parameterFinder         The finder for the parameter references.
     *      @param [in] expressionFormatter     Changes the referenced ids to parameter names.
     *      @param [in] parent                  Pointer to the owner of this editor.
     */
    SingleMemoryMapEditor(QSharedPointer<Component> component,
        QSharedPointer<AbstractMemoryMap> memoryRemap,
        QSharedPointer<MemoryMap> parentMemoryMap,
        LibraryInterface* libHandler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SingleMemoryMapEditor();

    /*!
	 *  Check for the validity of the edited remap state.
	 *
     *      @return True, if the remap state is valid, false otherwise.
	 */
	virtual bool isValid() const;

	/*!
	 *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:
    
    /*!
     *  The show event for this editor.
     *
     *      @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent * event);

private slots:

    /*!
     *  Updates the name of the current slave interface binding.
     */
    void refreshSlaveBinding();

    /*!
     *  Updates the address unit bits [AUB].
     *
     *      @param [in] newAddressUnitBits  The new address unit bits [AUB].
     */
    void updateAddressUnitBits(QString const& newAddressUnitBits);

    /*!
     *  Sets the new remap state to the memory remap.
     *
     *      @param [in] newRemapState   The name of the selected remap state.
     */
    void onRemapStateSelected(QString const& newRemapState);

signals:
    
    /*!
     *  Informs contents of the change in address unit bits.
     */
    void addressUnitBitsChanged();

private:

	//! No copying
    SingleMemoryMapEditor(const SingleMemoryMapEditor& other);

	//! No assignment
    SingleMemoryMapEditor& operator=(const SingleMemoryMapEditor& other);

	/*!
	 *  Set up the layout for the editor.
	 */
	void setupLayout();

    /*!
     *  Updates the remap state selector.
     */
    void refreshRemapStateSelector();

    /*!
     *  Check if the memory remap is actually a memory map.
     *
     *      @return True, if the memory remap is memory map, false otherwise.
     */
    bool isMemoryMap();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Editor to set the name, display name and description of the memory remap.
	NameGroupEditor nameEditor_;

    //! The editor for the address blocks.
    MemoryMapEditor* memoryMapEditor_;

    //! The editor for the memory maps address unit bits [AUB].
    QLineEdit* addressUnitBitsEditor_;

    //! The label for the slave interface binding.
    QLabel* slaveInterfaceLabel_;
    
    //! The remap state selector.
    ReferenceSelector* remapStateSelector_;

    //! The memory remap being edited.
    QSharedPointer<AbstractMemoryMap> memoryRemap_;

    //! The parent of the memory remap.
    QSharedPointer<MemoryMap> parentMemoryMap_;

};

#endif // SINGLEMEMORYMAPEDITOR_H
