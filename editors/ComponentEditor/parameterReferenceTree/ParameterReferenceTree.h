//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying parameter references.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREFERENCETREE_H
#define PARAMETERREFERENCETREE_H

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/mirroredslaveinterface.h>

#include <QTreeWidget>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Tree widget for displaying differences of two components.
//-----------------------------------------------------------------------------
class ParameterReferenceTree : public QTreeWidget 
{
    Q_OBJECT
public:

    //! Enumeration of the two columns.
    enum COLUMNS
    {
        ITEM_NAME=0,
        ITEM_EXPRESSION,
        COLUMN_COUNT
    };
    
    //! The constructor.
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
    bool referenceExistsInParameters(QList <QSharedPointer<Parameter> > parameterList);

    /*!
     *  Check if a parameter has a reference to this parameter.
     *
     *      @param [in] parameter   The parameter being studied.
     *
     *      @return True, if a reference exists in this parameter, false otherwise.
     */
    bool parameterHasReference(QSharedPointer<Parameter> parameter);

    /*!
     *  Create the references for parameters.
     */
    void createParameterReferences();

    /*!
     *  Check if a reference exists in components model parameters.
     *
     *      @return True, if there is a reference in model parameters to this parameter, false otherwise.
     */
    bool referenceExistsInModelParameters();

    /*!
     *  Create the references for model parameters.
     */
    void createReferencesForModelParameters();

    /*!
     *  Check if a reference exists in views' parameters.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInViews();

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
     *  Check if a reference exists in an address block.
     *
     *      @param [in] addressBlock    The address block whose references are being looked at.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool referenceExistsInAddressBlock(QSharedPointer<AddressBlock> addressBlock);

    /*!
     *  Check if a register has a reference.
     *
     *      @param [in] targetRegister      The register being studied.
     *
     *      @return True, if there is a reference to this parameter, false otherwise.
     */
    bool registerHasReference(QSharedPointer<Register> targetRegister);

    /*!
     *  Check if any of the register fields has a reference.
     *
     *      @param [in] targetRegister      The register, whose fields are being checked.
     */
    bool referenceExistsInRegisterFields(QSharedPointer<Register> targetRegister);

    /*!
     *  Check if a single field has a reference.
     *
     *      @param [in] targetField         The target field.
     */
    bool registerFieldHasReference(QSharedPointer<Field> targetField);

    /*!
     *  Create the references for memory maps.
     */
    void createReferencesForMemoryMaps();

    /*!
     *  Create the references for a single memory map.
     *
     *      @param [in] memoryMap           The memory map.
     *      @param [in] topMemoryMapItem    The parent tree item of the new memory map tree item.
     */
    void createReferencesForSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap, QTreeWidgetItem* topMemoryMapItem);

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
    bool referenceExistsInBusInterfaces();

    /*!
     *  Create references for bus interfaces.
     */
    void createReferencesForBusInterfaces();

    /*!
     *  Check if a reference exists in a single bus interface.
     *
     *      @param [in] busInterface    Pointer to the bus interface.
     */
    bool referenceExistsInSingleBusInterface(QSharedPointer<BusInterface> busInterface);

    /*!
     *  Check if a reference exists in mirrored slave.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool referenceExistsInMirroredSlave(QSharedPointer<MirroredSlaveInterface> mirrorSlave);

    /*!
     *  Check if a reference exists in mirrored slaves remap address.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool mirroredSlaveRemapAddressHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave);

    /*!
     *  Check if a reference exists in mirrored slaves range.
     *
     *      @param [in] mirrorSlave     Pointer to the mirrored slave interface.
     */
    bool mirroredSlaveRangeHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave);

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

    //! The component in which this id is situated.
    QSharedPointer<Component> component_;

    //! The parameter whose references are being searched for.
    QString targetID_;

    //! The formatter for the expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // PARAMETERREFERENCETREE_H
