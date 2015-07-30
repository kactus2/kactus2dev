//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying references to a parameter within a component.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREFERENCETREE_H
#define PARAMETERREFERENCETREE_H

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/AbstractMemoryMap.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/MemoryRemap.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/remapstate.h>
#include <IPXACTmodels/remapport.h>

#include <QTreeWidget>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Tree widget for displaying references to a parameter within a component.
//-----------------------------------------------------------------------------
class ParameterReferenceTree : public QTreeWidget 
{
    Q_OBJECT
public:

    //! Enumeration of the columns.
    enum COLUMNS
    {
        ITEM_NAME = 0,
        ITEM_EXPRESSION,
        COLUMN_COUNT
    };
    
    /*!
     *   The constructor.
     *
     *      @param [in] component               The component whose references to display.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] targetID                The parameter id whose references to display.
     *      @param [in] parent                  The parent widget.
     */
    ParameterReferenceTree(QSharedPointer<Component> component,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QString const& targetID, QWidget *parent = 0);

	//! The destructor.
    ~ParameterReferenceTree();

private:
    
	// Disable copying.
    ParameterReferenceTree(ParameterReferenceTree const& rhs);
    ParameterReferenceTree& operator=(ParameterReferenceTree const& rhs);

    /*!
     *  Setup the reference tree.
     */
    void setupTree();

    /*!
     *  Check if a reference exists in components parameters.
     *
     *      @param[in] parameterList    The list of parameters being looked at.
     *
     *      @return True, if there is a reference in parameters to this parameter, false otherwise.
     */
    bool referenceExistsInParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList) const;

    /*!
     *  Check if a parameter has a reference to this parameter.
     *
     *      @param [in] parameter   The parameter being studied.
     *
     *      @return True, if a reference exists in this parameter, false otherwise.
     */
    bool parameterHasReference(QSharedPointer<Parameter> parameter) const;

    /*!
     *  Create the references for parameters.
     *
     *      @param [in] parameters      The model parameters to create references for.
     *      @param [in] parentItem      The parent item for references.
     */
    void createParameterReferences(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QTreeWidgetItem* parentItem);

    /*!
     *  Check if a reference exists in components model parameters.
     *
     *      @return True, if there is a reference in model parameters to this parameter, false otherwise.
     */
    bool referenceExistsInModelParameters(
        QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters) const;

    /*!
     *  Create the references for model parameters.
     *
     *      @param [in] modelParametes  The model parameters to create references for.
     *      @param [in] parentItem      The parent item for references.
     */
    void createReferencesForModelParameters(QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParametes,
        QTreeWidgetItem* parentItem);

    /*!
     *  Check if a reference exists in address spaces.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInAddressSpaces() const;
    
    /*!
     *  Check if a reference exists in an address space.
     *
     *      @param[in] addressSpace    The address space to search for.
     *
     *      @return True, if there is a reference in the address space to this parameter, otherwise false.
     */
    bool referenceExistsInSingleAddressSpace(QSharedPointer<AddressSpace> addressSpace) const;
        
    /*!
     *  Check if a reference exists in an address space segments.
     *
     *      @param[in] addressSpace    The address space whose segments to search for.
     *
     *      @return True, if there is a reference in the segments to this parameter, otherwise false.
     */
    bool referenceExistsInSegments(QSharedPointer<AddressSpace> addressSpace) const;
            
    /*!
     *  Check if a reference exists in a segment.
     *
     *      @param[in] segment      The segment to search for.
     *
     *      @return True, if there is a reference in the segment to this parameter, otherwise false.
     */
    bool referenceExistsInSingleSegment(QSharedPointer<Segment> segment) const;

    /*!
     *  Check if a reference exists in views' parameters.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInViews() const;

    /*!
     *  Check if a reference exists in a single view parameters.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInView(QSharedPointer<View> view) const;

    /*!
     *  Create the references for views.
     */
    void createReferencesForViews();

    /*!
     *  Check if a reference exists in ports.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInPorts();

    /*!
     *  Check if a single port has a reference to this parameter.
     *
     *      @param [in] port    The port being studied.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool portHasreference(QSharedPointer<Port> port);

    /*!
     *  Create the references for ports.
     */
    void createReferencesForPorts();

    /*!
     *  Check if a reference exist in memory maps.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInMemoryMaps();

    /*!
     *  Check if a reference exists in a single memory map.
     *
     *      @param [in] memoryMap   The memory map whose references are being looked at.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap);

    /*!
     *  Check if a reference exists in the default memory remap (memory map).
     *
     *      @param [in] memoryMap   The memory map being examined.
     *
     *      @return True, if a reference exists in this memory map, false otherwise.
     */
    bool referenceExistsInDefaultMemoryRemap(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Check if a reference exists in a single memory remap of a memory map.
     *
     *      @param [in] memoryRemap     The memory remap being examined.
     *
     *      @return True, if a reference exists in this memory remap, false otherwise.
     */
    bool referenceExistsInSingleMemoryRemap(QSharedPointer<MemoryRemap> memoryRemap);

    /*!
     *  Check if a reference exists in an address block.
     *
     *      @param [in] addressBlock    The address block whose references are being looked at.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInAddressBlock(QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Checks if a reference exists in an address blocks values.
     *
     *      @param [in] addressBlock    The address block whose references are being looked at.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInAddressBlockValues(QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Check if a register has a reference.
     *
     *      @param [in] targetRegister      The register being studied.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool registerHasReference(QSharedPointer<Register> targetRegister) const;

    /*!
     *  Check if any of the register fields has a reference.
     *
     *      @param [in] targetRegister      The register, whose fields are being checked.
     */
    bool referenceExistsInRegisterFields(QSharedPointer<Register> targetRegister) const;

    /*!
     *  Check if a single field has a reference.
     *
     *      @param [in] targetField         The target field.
     */
    bool registerFieldHasReference(QSharedPointer<Field> targetField) const;

    /*!
     *  Create the references for address spaces.
     */
    void createReferencesForAddressSpaces();

    /*!
     *  Create the references for a single address space.
     *
     *      @param [in] addressSpace    The address space to create references for.
     *      @param [in] parent          The parent tree item of the new address space item.
     */
    void createItemsForAddressSpace(QSharedPointer<AddressSpace> addressSpace, QTreeWidgetItem* parent);
    
    /*!
     *  Create the references for segments within an address space.
     *
     *      @param [in] addressSpace    The address space to create references for.
     *      @param [in] parent          The parent tree item of the new segment items.
     */
    void createReferencesForSegments(QSharedPointer<AddressSpace> addressSpace, QTreeWidgetItem* parent);
        
    /*!
     *  Create the references a segment within an address space.
     *
     *      @param [in] segment    The segment to create references for.
     *      @param [in] parent     The parent tree item of the segment items.
     */
    void createItemsForSegment(QSharedPointer<Segment> segment, QTreeWidgetItem* parent);

    /*!
     *  Create the references for memory maps.
     */
    void createReferencesForMemoryMaps();

    /*!
     *  Create the references for a single abstract memory map.
     *
     *      @param [in] memoryMap           The memory map / memory remap.
     *      @param [in] topMemoryMapItem    The parent tree item of the new memory map tree item.
     */
    void createReferencesForSingleMemoryMap(QSharedPointer<AbstractMemoryMap> memoryMap,
        QTreeWidgetItem* memoryRemapItem);

    /*!
     *  Create the references for a single address block.
     *
     *      @param [in] addressBlock                The address block.
     *      @param [in] middleAddressBlocksItem     The parent tree item of the new address block tree item.
     */
    void createReferencesForSingleAddressBlock(QSharedPointer<AddressBlock> addressBlock,
        QTreeWidgetItem* middleAddressBlocksItem);

    /*!
     *  Create the references for a single register.
     *
     *      @param [in] targetRegister  The register.
     *      @param [in] parentItem      The parent tree item of the new register tree item.
     */
    void createReferencesForSingleRegister(QSharedPointer<Register> targetRegister, QTreeWidgetItem* parentItem);

    /*!
     *  Check if a reference exists in bus interfaces.
     *
     *      @return True, if a reference exists in a bus interface, false otherwise.
     */
    bool referenceExistsInBusInterfaces() const;

    /*!
     *  Create references for bus interfaces.
     */
    void createReferencesForBusInterfaces();

    /*!
     *  Check if a reference exists in a single bus interface.
     *
     *      @param [in] busInterface    Pointer to the bus interface.
     */
    bool referenceExistsInSingleBusInterface(QSharedPointer<BusInterface> busInterface) const;

    /*!
     *  Check if a reference exists in mirrored slave.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool referenceExistsInMirroredSlave(QSharedPointer<MirroredSlaveInterface> mirrorSlave) const;

    /*!
     *  Check if a reference exists in a master interface.
     *
     *      @param [in] masterInterface     Pointer to the master interface.
     */
    bool referenceExistsInMasterInterface(QSharedPointer<MasterInterface> masterInterface) const;

    /*!
     *  Check if a reference exists in mirrored slaves remap address.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool mirroredSlaveRemapAddressHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave) const;

    /*!
     *  Check if a reference exists in mirrored slaves range.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool mirroredSlaveRangeHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave) const;

    /*!
     *  Check if a reference exists in remap states.
     *
     *      @return True, if a reference exists in remap states, false otherwise.
     */
    bool referenceExistsInRemapStates() const;

    /*!
     *  Check if a reference exists in a single remap state.
     *
     *      @param [in] remapState  Pointer to the remap state.
     *
     *      @return True, if a reference exists in a single remap state, false otherwise.
     */
    bool referenceExistsInSingleRemapState(QSharedPointer<RemapState> remapState) const;

    /*!
     *  Check if a reference exists in a single remap port.
     *
     *      @param [in] remapPort   Pointer to the remap port.
     *
     *      @return True, if a reference exists in a single remap port, flase otherwise.
     */
    bool referenceExistsInSingleRemapPort(QSharedPointer<RemapPort> remapPort) const;

    /*!
     *  Create references for remap states.
     */
    void createReferencesForRemapStates();

    /*!
     *  Create a tree widget item to the top of the tree.
     *
     *      @param [in] itemName    The name of the upcoming item.
     *
     *      @return A pointer to the new tree item.
     */
    QTreeWidgetItem* createTopItem(QString const& itemName);

    /*!
     *  Create a tree item between two items.
     *
     *      @param [in] itemnName   The name of the upcoming item.
     *      @param [in] parent      The parent of the upcoming item.
     *
     *      @return A pointer to the new tree item.
     */
    QTreeWidgetItem* createMiddleItem(QString const& itemnName, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for a parameter that is referencing this parameter.
     *
     *      @param [in] parameter   The referencing parameter.
     *      @param [in] parent      The parent of the upcoming item.
     */
    void createItemsForParameter(QSharedPointer<Parameter> parameter, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for a port that is referencing this parameter.
     *
     *      @param [in] port        The referencing port.
     *      @param [in] parent      The parent of the upcoming item.
     */
    void createItemsForPort(QSharedPointer<Port> port, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for an address block that is referencing this parameter.
     *
     *      @param [in] targetAddressBlock  The referencing address block.
     *      @param [in] parent              The parent for the upcoming item.
     */
    void createItemsForAddressBlock(QSharedPointer<AddressBlock> targetAddressBlock, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for a register that is referencing this parameter.
     *
     *      @param [in] targetRegister  The referencing register.
     *      @param [in] parent          The parent of the upcoming item.
     */
    void createItemsForRegister(QSharedPointer<Register> targetRegister, QTreeWidgetItem* parent);

    /*!
     *  Create tree items for a register field that is referencing this parameter.
     *
     *      @param [in] targetField     The referencing register field.
     *      @param [in] parent          The parent of the new item.
     */
    void createItemsForField(QSharedPointer<Field> targetField, QTreeWidgetItem* parent);

    /*!
     *  Create a referencing tree item.
     *
     *      @param [in] itemName        The name of the upcoming tree item.
     *      @param [in] expression      The expression where the upcoming tree item references this parameter.
     *      @param [in] parent          The parent of the upcoming item.
     */
    void createItem(QString const& itemName, QString const& expression, QTreeWidgetItem* parent);

    /*!
     *  Colour the selected items background in grey.
     *
     *      @param [in] item    The selected item.
     */
    void colourItemGrey(QTreeWidgetItem* item);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component in which this id is situated.
    QSharedPointer<Component> component_;

    //! The parameter whose references are being searched for.
    QString targetID_;

    //! The formatter for the expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // PARAMETERREFERENCETREE_H
