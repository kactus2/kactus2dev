//-----------------------------------------------------------------------------
// File: tst_MemoryViewGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.04.2016
//
// Description:
// Unit test for class MemoryViewGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/MemoryViewGenerator/MemoryViewGenerator.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <tests/MockObjects/LibraryMock.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/SlaveInterface.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

class tst_MemoryViewGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryViewGenerator();

private slots:

    void init();
    void cleanup();
    
    void testConnectedSlaveAddressBlocksAreWritten();

    void testMultipleReferencesToSameDesign();
  
    void testConnectedSlaveRegistersAndFieldsAreWritten();
   
    void testConnectedRegisterDimensionsAreWritten();
   
    void testMasterOffsetIsCalculated();

    void testRegisterWithLongFields();

    void testExpressionsWithoutReferences();

    void testIsPresent();

    void testRemapOnBusComponent();

    void testMultipleChannels();

    void testHierarchicalDesign();

    void testActiveViewConfiguration();

    void testMasterInHierarchy();

    void testRemapStatesOnSlave();

    void testSegmentsWithinAddressSpace();

    void testMultipleConnectionsFromMaster();

    void testBridge();

private:
    
    QString runGenerator();

    QSharedPointer<Component> createMasterComponent(VLNV masterVLNV);

    void connectMasterAndSlaveInstance();

    void createMasterAndSlaveInstances(VLNV masterVLNV, VLNV slaveVLNV);

    QSharedPointer<Component> createSlaveComponent(VLNV slaveVLNV);

    void createComponentInstance(VLNV const& vlnv, QString const& name, QString const& id, QSharedPointer<Design> design);

    void createInterconnection(QString const& instance1, QString const& interface1, QString const& instance2,
        QString const& interface2, QSharedPointer<Design> design);

    void createHierarchicalConnection(QString const& topInterface, QString const& componentInstance, 
        QString const& componentInterface, QSharedPointer<Design> design);

    QSharedPointer<AddressBlock> addAddressBlock(QString const& name, QString const& baseAddress, QString const& range,
        QString const& width, QSharedPointer<MemoryMap> containingMemoryMap);

    LibraryMock* library_;

    QSharedPointer<Component> topComponent_;

    QSharedPointer<Design> design_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::tst_MemoryViewGenerator()
//-----------------------------------------------------------------------------
tst_MemoryViewGenerator::tst_MemoryViewGenerator(): library_(new LibraryMock(this)), topComponent_(), design_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::init()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::init()
{        
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TopComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    design_ = QSharedPointer<Design>(new Design(designVlnv));
    library_->addComponent(design_);
    
    QSharedPointer<View> hierarchicalView(new View("hierarchical"));
    hierarchicalView->setDesignInstantiationRef("design");
    topComponent_->getViews()->append(hierarchicalView);

    QSharedPointer<DesignInstantiation> designInstantiation(new DesignInstantiation("design"));
    designInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(design_->getVlnv())));
    topComponent_->getDesignInstantiations()->append(designInstantiation);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::cleanup()
{
    library_->clear();

    QFile outputFile("./output.csv");
    outputFile.remove();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testConnectedSlaveAddressBlocksAreWritten()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testConnectedSlaveAddressBlocksAreWritten()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    addAddressBlock("firstBlock", "0", "8", "32", slaveMemoryMap);
    addAddressBlock("secondBlock", "8", "4", "32", slaveMemoryMap);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.firstBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.secondBlock;addressBlock;0x8;4;32;;;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testMultipleReferencesToSameDesign()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testMultipleReferencesToSameDesign()
{
    QSharedPointer<View> hierarchicalView = topComponent_->getViews()->first();
    hierarchicalView->setDesignConfigurationInstantiationRef("config");

    VLNV configVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLib", "TestConfiguration", "1.0");

    QSharedPointer<DesignConfiguration> designConfiguration(new DesignConfiguration(configVLNV));
    designConfiguration->setDesignRef(design_->getVlnv());
    library_->addComponent(designConfiguration);

    QSharedPointer<DesignConfigurationInstantiation> configInstantiation(
        new DesignConfigurationInstantiation("config"));
    configInstantiation->setDesignConfigurationReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(configVLNV)));

    topComponent_->getDesignConfigurationInstantiations()->append(configInstantiation);

    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    addAddressBlock("firstBlock", "0", "8", "32", slaveMemoryMap);
    addAddressBlock("secondBlock", "8", "4", "32", slaveMemoryMap);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.firstBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.secondBlock;addressBlock;0x8;4;32;;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testConnectedSlaveRegistersAndFieldsAreWritten()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testConnectedSlaveRegistersAndFieldsAreWritten()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "0", "32"));
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<Register> secondRegister(new Register("secondRegister", "2", "32"));
    slaveAddressBlock->getRegisterData()->append(secondRegister);

    QSharedPointer<Field> firstField(new Field("firstField"));
    firstField->setBitWidth("16");
    firstField->setBitOffset("1");

    QSharedPointer<Field> secondField(new Field("secondField"));
    secondField->setBitWidth("1");
    secondField->setBitOffset("0");

    secondRegister->getFields()->append(firstField);
    secondRegister->getFields()->append(secondField);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister;register;0x0;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister;register;0x2;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister.firstField;field;0x2;;16;;1;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister.secondField;field;0x2;;1;;0;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testConnectedRegisterDimensionsAreWritten()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testConnectedRegisterDimensionsAreWritten()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveMemoryMap->setAddressUnitBits("32");
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "0", "32"));
    firstRegister->setDimension("2");
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<Field> firstField(new Field("firstField"));
    firstField->setBitWidth("16");
    firstField->setBitOffset("1");

    firstRegister->getFields()->append(firstField);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[0];register;0x0;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[0].firstField;field;0x0;;16;;1;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[1];register;0x1;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[1].firstField;field;0x1;;16;;1;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testMasterOffsetIsCalculated()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testMasterOffsetIsCalculated()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);

    QSharedPointer<BusInterface> masterInterface = masterComponent->getBusInterface("masterIf");
    masterInterface->getMaster()->setBaseAddress("256");

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "16", "8", "32", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "8", "32"));
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<Field> firstField(new Field("firstField"));
    firstField->setBitWidth("16");
    firstField->setBitOffset("0");

    firstRegister->getFields()->append(firstField);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x100;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x110;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister;register;0x118;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister.firstField;field;0x118;;16;;0;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testRegisterWithLongFields()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testRegisterWithLongFields()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveMemoryMap->setAddressUnitBits("16");
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "16", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "0", "32"));
    firstRegister->setDimension("2");
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<Register> secondRegister(new Register("secondRegister", "4", "64"));
    slaveAddressBlock->getRegisterData()->append(secondRegister);

    QSharedPointer<Field> firstField(new Field("firstField"));
    firstField->setBitWidth("16");
    firstField->setBitOffset("0");
    secondRegister->getFields()->append(firstField);

    QSharedPointer<Field> secondField(new Field("secondField"));
    secondField->setBitWidth("16");
    secondField->setBitOffset("16");
    secondRegister->getFields()->append(secondField);

    QSharedPointer<Field> thirdField(new Field("thirdField"));
    thirdField->setBitWidth("4");
    thirdField->setBitOffset("40");
    secondRegister->getFields()->append(thirdField);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);

    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;16;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[0];register;0x0;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister[1];register;0x2;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister;register;0x4;;;64;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister.firstField;field;0x4;;16;;0;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister.secondField;field;0x5;;16;;0;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister.thirdField;field;0x6;;4;;8;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testExpressionsWithoutReferences()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testExpressionsWithoutReferences()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveMemoryMap->setAddressUnitBits("4+4");
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "2+2", "2*6", "4*4", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "1+1", "2*8"));
    slaveAddressBlock->getRegisterData()->append(firstRegister);
 
    QSharedPointer<Field> firstField(new Field("firstField"));
    firstField->setBitWidth("4+4+4+4");
    firstField->setBitOffset("1+2");
    firstRegister->getFields()->append(firstField);

    QSharedPointer<Register> secondRegister(new Register("secondRegister", "'h4", "'d16"));
    secondRegister->setDimension("1+1");
    slaveAddressBlock->getRegisterData()->append(secondRegister);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x4;12;16;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister;register;0x6;;;16;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister.firstField;field;0x6;;16;;3;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister[0];register;0x8;;;16;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.secondRegister[1];register;0xa;;;16;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testIsPresent()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);
    slaveAddressBlock->setIsPresent("true");

    QSharedPointer<AddressBlock> nonPresentBlock = addAddressBlock("nonPresentBlock", "0", "8", "32", slaveMemoryMap);
    nonPresentBlock->setIsPresent("false");

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "0", "32"));
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<Register> nonPresentRegister(new Register("nonPresentRegister", "2", "32"));
    nonPresentRegister->setIsPresent("1-1");
    slaveAddressBlock->getRegisterData()->append(nonPresentRegister);

    QSharedPointer<Field> nonPresentField(new Field("nonPresentField"));
    nonPresentField->setIsPresent("false");
    nonPresentField->setBitWidth("1");
    nonPresentField->setBitOffset("0");

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister;register;0x0;;;32;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testRemapOnBusComponent()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testRemapOnBusComponent()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV busVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestBus", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);
    masterComponent->getBusInterface("masterIf")->getMaster()->setBaseAddress("4");

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);
    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<Component> busComponent(new Component(busVLNV));
    library_->addComponent(busComponent);

    QSharedPointer<BusInterface> mirroredMasterInterface(new BusInterface());
    mirroredMasterInterface->setName("mirroredMasterIf");
    mirroredMasterInterface->setInterfaceMode(General::MIRROREDMASTER);
    busComponent->getBusInterfaces()->append(mirroredMasterInterface);

    QSharedPointer<BusInterface> mirroredSlaveInterface(new BusInterface());
    mirroredSlaveInterface->setName("mirroredSlaveIf");
    mirroredSlaveInterface->setInterfaceMode(General::MIRROREDSLAVE);
    mirroredSlaveInterface->getMirroredSlave()->setRemapAddress("'h10");
    busComponent->getBusInterfaces()->append(mirroredSlaveInterface);

    QStringList channelInterfaces;
    channelInterfaces << "mirroredMasterIf" << "mirroredSlaveIf";

    QSharedPointer<Channel> testChannel(new Channel());
    testChannel->setName("testChannel");
    testChannel->setInterfaces(channelInterfaces);
    busComponent->getChannels()->append(testChannel);

    createComponentInstance(busVLNV, "busInstance", "busID", design_);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);

    createInterconnection("masterInstance", "masterIf", "busInstance", "mirroredMasterIf", design_);
    createInterconnection("slaveInstance", "slaveIf", "busInstance", "mirroredSlaveIf", design_);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x14;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x14;8;32;;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testMultipleChannels()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testMultipleChannels()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV busVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestBus", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);
    masterComponent->getBusInterface("masterIf")->getMaster()->setBaseAddress("8");

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);
    
    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);
    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);


    QSharedPointer<Component> busComponent(new Component(busVLNV));
    library_->addComponent(busComponent);

    QSharedPointer<BusInterface> mirroredMasterInterface(new BusInterface());
    mirroredMasterInterface->setName("mirroredMasterIf");
    mirroredMasterInterface->setInterfaceMode(General::MIRROREDMASTER);
    busComponent->getBusInterfaces()->append(mirroredMasterInterface);

    QSharedPointer<BusInterface> mirroredSlaveInterface(new BusInterface());
    mirroredSlaveInterface->setName("mirroredSlaveIf");
    mirroredSlaveInterface->setInterfaceMode(General::MIRROREDSLAVE);
    mirroredSlaveInterface->getMirroredSlave()->setRemapAddress("8");
    busComponent->getBusInterfaces()->append(mirroredSlaveInterface);

    QSharedPointer<BusInterface> duplicateSlaveInterface(new BusInterface());
    duplicateSlaveInterface->setName("duplicateSlaveIf");
    duplicateSlaveInterface->setInterfaceMode(General::MIRROREDSLAVE);
    duplicateSlaveInterface->getMirroredSlave()->setRemapAddress("16");
    busComponent->getBusInterfaces()->append(duplicateSlaveInterface);

    QStringList channelInterfaces;
    channelInterfaces << "mirroredMasterIf" << "mirroredSlaveIf" << "duplicateSlaveIf";

    QSharedPointer<Channel> testChannel(new Channel());
    testChannel->setName("testChannel");
    testChannel->setInterfaces(channelInterfaces);
    busComponent->getChannels()->append(testChannel);

    createComponentInstance(busVLNV, "busInstance", "busID", design_);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);

    createComponentInstance(slaveVLNV, "duplicateSlave", "duplicateID", design_);

    createInterconnection("masterInstance", "masterIf", "busInstance", "mirroredMasterIf", design_);
    createInterconnection("slaveInstance", "slaveIf", "busInstance", "mirroredSlaveIf", design_);
    createInterconnection("duplicateSlave", "slaveIf", "busInstance", "duplicateSlaveIf", design_);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x10;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x10;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.duplicateID.duplicateSlave.slaveMemoryMap;memoryMap;0x18;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.duplicateID.duplicateSlave.slaveMemoryMap.slaveBlock;addressBlock;0x18;8;32;;;\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testHierarchicalDesign()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testHierarchicalDesign()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV hierarchicalSlaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "HierarchicalSlave", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");
    VLNV designVLNV(VLNV::DESIGN, "tut.fi", "TestLib", "SlaveDesign", "1.0");
    
    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);
    masterComponent->getBusInterface("masterIf")->getMaster()->setBaseAddress("2");

    QSharedPointer<Component> hierarchicalComponent(new Component(hierarchicalSlaveVLNV));
    QSharedPointer<BusInterface> slaveIf(new BusInterface());
    slaveIf->setName("slaveIf");
    slaveIf->setInterfaceMode(General::SLAVE);
    hierarchicalComponent->getBusInterfaces()->append(slaveIf);

    library_->addComponent(hierarchicalComponent);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);
    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<Design> slaveDesign(new Design(designVLNV));
    library_->addComponent(slaveDesign);

    createComponentInstance(slaveVLNV, "slaveInstance", "slaveID", slaveDesign);

    createHierarchicalConnection("slaveIf", "slaveInstance", "slaveIf", slaveDesign);

    QSharedPointer<DesignInstantiation> hierarchyInstantiation(new DesignInstantiation("instantiation"));
    hierarchyInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(designVLNV)));
    hierarchicalComponent->getDesignInstantiations()->append(hierarchyInstantiation);

    QSharedPointer<View> hierarchicalView(new View("hierarchical"));
    hierarchicalView->setDesignInstantiationRef("instantiation");
    hierarchicalComponent->getViews()->append(hierarchicalView);

    createComponentInstance(masterVLNV, "masterInstance", "masterID", design_);
    createComponentInstance(hierarchicalSlaveVLNV, "hierarchicalInstance", "hierID", design_);

    createInterconnection("masterInstance", "masterIf", "hierarchicalInstance", "slaveIf", design_);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x2;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x2;8;32;;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testActiveViewConfiguration()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testActiveViewConfiguration()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV hierarchicalSlaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "HierarchicalSlave", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");
    VLNV designConfigVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLib", "SlaveDesignConfig", "1.0");
    VLNV designVLNV(VLNV::DESIGN, "tut.fi", "TestLib", "SlaveDesign", "1.0");

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);

    QSharedPointer<Component> hierarchicalComponent(new Component(hierarchicalSlaveVLNV));
    QSharedPointer<BusInterface> hierarchicalIf(new BusInterface());
    hierarchicalIf->setName("slaveIf");
    hierarchicalIf->setInterfaceMode(General::SLAVE);    
    hierarchicalIf->getSlave()->setMemoryMapRef("hierarchicalMemoryMap");
    hierarchicalComponent->getBusInterfaces()->append(hierarchicalIf);

    library_->addComponent(hierarchicalComponent);

    QSharedPointer<MemoryMap> hierarchicalMemoryMap(new MemoryMap("hierarchicalMemoryMap"));
    hierarchicalComponent->getMemoryMaps()->append(hierarchicalMemoryMap);
    addAddressBlock("hierarchicalBlock", "8", "16", "32", hierarchicalMemoryMap);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);
    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<DesignConfiguration> slaveDesignConfig(new DesignConfiguration(designConfigVLNV));
    slaveDesignConfig->setDesignRef(design_->getVlnv());

    QSharedPointer<ViewConfiguration> activeView(new ViewConfiguration());
    activeView->setInstanceName("hierarchicalInstance");
    activeView->setViewReference("hierarchical");
    slaveDesignConfig->getViewConfigurations()->append(activeView);

    library_->addComponent(slaveDesignConfig);

    QSharedPointer<Design> slaveDesign(new Design(designVLNV));
    library_->addComponent(slaveDesign);

    createComponentInstance(slaveVLNV, "slaveInstance", "slaveID", slaveDesign);

    createHierarchicalConnection("slaveIf", "slaveInstance", "slaveIf", slaveDesign);

    topComponent_->getViews()->first()->setDesignConfigurationInstantiationRef("instantiation");

    QSharedPointer<DesignConfigurationInstantiation> hierarchyInstantiation(
        new DesignConfigurationInstantiation("instantiation"));
    hierarchyInstantiation->setDesignConfigurationReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(designConfigVLNV)));
    topComponent_->getDesignConfigurationInstantiations()->append(hierarchyInstantiation);

    QSharedPointer<View> flatView(new View("flat"));
    hierarchicalComponent->getViews()->append(flatView);

    QSharedPointer<View> hierarchicalView(new View("hierarchical"));
    hierarchicalView->setDesignInstantiationRef("instantiation");
    hierarchicalComponent->getViews()->append(hierarchicalView);

    QSharedPointer<DesignInstantiation> designInstantiation(new DesignInstantiation("instantiation"));
    designInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(designVLNV)));
    hierarchicalComponent->getDesignInstantiations()->append(designInstantiation);

    createComponentInstance(masterVLNV, "masterInstance", "masterID", design_);
    createComponentInstance(hierarchicalSlaveVLNV, "hierarchicalInstance", "hierarchyID", design_);

    createInterconnection("masterInstance", "masterIf", "hierarchicalInstance", "slaveIf", design_);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        ));

    activeView->setViewReference("flat");
    output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.HierarchicalSlave.1.0.hierarchyID.hierarchicalInstance.hierarchicalMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.HierarchicalSlave.1.0.hierarchyID.hierarchicalInstance.hierarchicalMemoryMap.hierarchicalBlock;addressBlock;0x8;16;32;;;\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testMasterInHierarchy()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testMasterInHierarchy()
{
    VLNV topConfigurationVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLib", "TopConfig", "1.0");
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");
    VLNV masterConfigVLNV(VLNV::DESIGNCONFIGURATION, "tut.fi", "TestLib", "MasterConfiguration", "1.0");
    VLNV masterDesignVLNV(VLNV::DESIGN, "tut.fi", "TestLib", "MasterDesign", "1.0");
    VLNV subMasterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "SubMaster", "1.0");

    QSharedPointer<DesignConfiguration> topConfig(new DesignConfiguration(topConfigurationVLNV));
    topConfig->setDesignRef(design_->getVlnv());
    topConfig->addViewConfiguration("master", "hier");
    library_->addComponent(topConfig);

    topComponent_->getViews()->first()->setDesignConfigurationInstantiationRef("topDesign");

    QSharedPointer<DesignConfigurationInstantiation> topInstantiation(
        new DesignConfigurationInstantiation("topDesign"));
    QSharedPointer<ConfigurableVLNVReference> topDesignReference(new ConfigurableVLNVReference(topConfigurationVLNV));
    topInstantiation->setDesignConfigurationReference(topDesignReference);
    topComponent_->getDesignConfigurationInstantiations()->append(topInstantiation);

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);
    QSharedPointer<View> hierarchyView(new View("hierarchical"));
    masterComponent->getViews()->append(hierarchyView);

    QSharedPointer<DesignConfigurationInstantiation> hierarchyInstance(
        new DesignConfigurationInstantiation("masterDesignConfig"));
    QSharedPointer<ConfigurableVLNVReference> masterConfigReference(new ConfigurableVLNVReference(masterConfigVLNV));
    hierarchyInstance->setDesignConfigurationReference(masterConfigReference);

    hierarchyView->setDesignConfigurationInstantiationRef("masterDesignConfig");
    masterComponent->getDesignConfigurationInstantiations()->append(hierarchyInstance);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "8", "8", "32", slaveMemoryMap);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QSharedPointer<DesignConfiguration> masterConfiguration(new DesignConfiguration(masterConfigVLNV));
    masterConfiguration->setDesignRef(masterDesignVLNV);
    library_->addComponent(masterConfiguration);

    QSharedPointer<Design> masterDesign(new Design(masterDesignVLNV));
    library_->addComponent(masterDesign);

    QSharedPointer<Component> subMaster = createMasterComponent(subMasterVLNV);
    subMaster->getBusInterface("masterIf")->getMaster()->setBaseAddress("2");
    library_->addComponent(subMaster);

    createComponentInstance(subMasterVLNV, "master", "masterID", masterDesign);
    createHierarchicalConnection("masterIf", "master", "masterIf", masterDesign);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x2;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0xa;8;32;;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testRemapStatesOnSlave()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testRemapStatesOnSlave()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    QSharedPointer<Register> firstRegister(new Register("firstRegister", "0", "32"));
    slaveAddressBlock->getRegisterData()->append(firstRegister);

    QSharedPointer<RemapState> testState(new RemapState("testState"));
    slaveComponent->getRemapStates()->append(testState);

    QSharedPointer<MemoryRemap> testRemap(new MemoryRemap("testRemap"));
    testRemap->setRemapState("testState");
    slaveMemoryMap->getMemoryRemaps()->append(testRemap);

    QSharedPointer<AddressBlock> remapAddressBlock(new AddressBlock("remapAddressBlock"));
    remapAddressBlock->setBaseAddress("16");
    remapAddressBlock->setRange("4");
    remapAddressBlock->setWidth("32");
    
    testRemap->getMemoryBlocks()->append(remapAddressBlock);

    QSharedPointer<Register> remapRegister(new Register("remapRegister", "0", "32"));
    remapAddressBlock->getRegisterData()->append(remapRegister);

    createMasterAndSlaveInstances(masterVLNV, slaveVLNV);
    connectMasterAndSlaveInstance();

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.slaveMemoryMap.slaveBlock.firstRegister;register;0x0;;;32;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.testRemap;memoryRemap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.testRemap.remapAddressBlock;addressBlock;0x10;4;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slaveInstance.testRemap.remapAddressBlock.remapRegister;register;0x10;;;32;;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testSegmentsWithinAddressSpace()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testSegmentsWithinAddressSpace()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");

    QSharedPointer<Component> masterComponent = createMasterComponent(masterVLNV);
    masterComponent->getBusInterface("masterIf")->getMaster()->setAddressSpaceRef("masterAddressSpace");
    createComponentInstance(masterVLNV, "masterInstance", "masterID", design_);

    QSharedPointer<AddressSpace> masterSpace(new AddressSpace("masterAddressSpace"));
    masterComponent->getAddressSpaces()->append(masterSpace);

    QSharedPointer<Segment> firstSegment(new Segment("firstSegment"));
    firstSegment->setRange("2");    
    firstSegment->setOffset("8");
    
    masterSpace->getSegments()->append(firstSegment);

    QSharedPointer<Segment> secondSegment(new Segment("secondSegment"));
    secondSegment->setRange("'h10");    
    secondSegment->setOffset("'h10");

    masterSpace->getSegments()->append(secondSegment);

    QSharedPointer<Segment> disabledSegment(new Segment("disabledSegment"));
    disabledSegment->setIsPresent("0");
    disabledSegment->setRange("8");    
    disabledSegment->setOffset("10");
    
    masterSpace->getSegments()->append(disabledSegment);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestMaster.1.0.masterID.masterInstance.masterAddressSpace;addressSpace;0x0;;;;;\n"
        "tut.fi.TestLib.TestMaster.1.0.masterID.masterInstance.masterAddressSpace.firstSegment;segment;0x0;2;;;8;\n"
        "tut.fi.TestLib.TestMaster.1.0.masterID.masterInstance.masterAddressSpace.secondSegment;segment;0x0;16;;;16;\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testMultipleConnectionsFromMaster()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testMultipleConnectionsFromMaster()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    createComponentInstance(masterVLNV, "master", "masterID", design_);
    createComponentInstance(slaveVLNV, "slave1", "slave1_id", design_);
    createComponentInstance(slaveVLNV, "slave2", "slave2_id", design_);

    createInterconnection("master", "masterIf", "slave1", "slaveIf", design_);
    createInterconnection("master", "masterIf", "slave2", "slaveIf", design_);

    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slave1_id.slave1.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slave1_id.slave1.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slave2_id.slave2.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slave2_id.slave2.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::testBridge()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::testBridge()
{
    VLNV masterVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestMaster", "1.0");
    VLNV bridgeVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestBridge", "1.0");
    VLNV slaveVLNV(VLNV::COMPONENT, "tut.fi", "TestLib", "TestSlave", "1.0");

    createMasterComponent(masterVLNV);

    QSharedPointer<Component> bridgeComponent(new Component(bridgeVLNV));

    QSharedPointer<BusInterface> bridgeMaster(new BusInterface());
    bridgeMaster->setName("bridgeMaster");
    bridgeMaster->setInterfaceMode(General::MASTER);
    bridgeComponent->getBusInterfaces()->append(bridgeMaster);
    
    QSharedPointer<BusInterface> bridgeSlave(new BusInterface());
    bridgeSlave->setName("bridgeSlave");
    bridgeSlave->setInterfaceMode(General::SLAVE);
    
    QSharedPointer<SlaveInterface::Bridge> bridge(new SlaveInterface::Bridge());
    bridge->masterRef_ = "bridgeMaster";
    bridgeSlave->getSlave()->getBridges()->append(bridge);

    bridgeComponent->getBusInterfaces()->append(bridgeSlave);
    library_->addComponent(bridgeComponent);

    QSharedPointer<Component> slaveComponent = createSlaveComponent(slaveVLNV);

    QSharedPointer<MemoryMap> slaveMemoryMap(new MemoryMap("slaveMemoryMap"));
    slaveComponent->getMemoryMaps()->append(slaveMemoryMap);

    QSharedPointer<AddressBlock> slaveAddressBlock = addAddressBlock("slaveBlock", "0", "8", "32", slaveMemoryMap);

    createComponentInstance(masterVLNV, "master", "masterID", design_);
    createComponentInstance(bridgeVLNV, "bridge", "bridgeID", design_);
    createComponentInstance(slaveVLNV, "slave", "slaveID", design_);
    
    createInterconnection("master", "masterIf", "bridge", "bridgeSlave", design_);
    createInterconnection("bridge", "bridgeMaster", "slave", "slaveIf", design_);
    
    QString output = runGenerator();

    QCOMPARE(output, QString("Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slave.slaveMemoryMap;memoryMap;0x0;;;;;\n"
        "tut.fi.TestLib.TestSlave.1.0.slaveID.slave.slaveMemoryMap.slaveBlock;addressBlock;0x0;8;32;;;\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::runGenerator()
//-----------------------------------------------------------------------------
QString tst_MemoryViewGenerator::runGenerator()
{
    MemoryViewGenerator generator(library_);
    generator.generate(topComponent_, "", "output.csv");

    QFile outputFile("./output.csv");

    outputFile.open(QIODevice::ReadOnly);
    QString output = outputFile.readAll();
    outputFile.close();

    return output;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createMasterComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryViewGenerator::createMasterComponent(VLNV masterVLNV)
{
    QSharedPointer<Component> masterComponent(new Component(masterVLNV));
    QSharedPointer<BusInterface> masterIf(new BusInterface());
    masterIf->setName("masterIf");
    masterIf->setInterfaceMode(General::MASTER);
    masterComponent->getBusInterfaces()->append(masterIf);

    library_->addComponent(masterComponent);

    return masterComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createSlaveComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryViewGenerator::createSlaveComponent(VLNV slaveVLNV)
{
    QSharedPointer<Component> slaveComponent(new Component(slaveVLNV));

    QSharedPointer<BusInterface> slaveIf(new BusInterface());
    slaveIf->setName("slaveIf");
    slaveIf->setInterfaceMode(General::SLAVE);
    slaveIf->getSlave()->setMemoryMapRef("slaveMemoryMap");
    slaveComponent->getBusInterfaces()->append(slaveIf);

    library_->addComponent(slaveComponent);

    return slaveComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createMasterAndSlaveInstances()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::createMasterAndSlaveInstances(VLNV masterVLNV, VLNV slaveVLNV)
{
    createComponentInstance(masterVLNV, "masterInstance", "masterID", design_);
    createComponentInstance(slaveVLNV, "slaveInstance", "slaveID", design_);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::connectMasterAndSlaveInstance()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::connectMasterAndSlaveInstance()
{
    QSharedPointer<Interconnection> masterSlaveConnection(new Interconnection());

    QSharedPointer<ActiveInterface> masterInterface(new ActiveInterface("masterInstance", "masterIf"));
    masterSlaveConnection->setStartInterface(masterInterface);

    QSharedPointer<ActiveInterface> slaveInterface(new ActiveInterface("slaveInstance", "slaveIf"));
    masterSlaveConnection->getActiveInterfaces()->append(slaveInterface);
    design_->getInterconnections()->append(masterSlaveConnection);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createComponentInstance()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::createComponentInstance(VLNV const& vlnv, QString const& name,
    QString const& id, QSharedPointer<Design> design)
{
    QSharedPointer<ConfigurableVLNVReference> vlnvReference(new ConfigurableVLNVReference(vlnv));

    QSharedPointer<ComponentInstance> newInstance(new ComponentInstance(name, vlnvReference));
    newInstance->setUuid(id);

    design->getComponentInstances()->append(newInstance);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createInterconnection()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::createInterconnection(QString const& instance1, QString const& interface1,
    QString const& instance2, QString const& interface2, QSharedPointer<Design> design)
{
    QSharedPointer<Interconnection> connection(new Interconnection());

    QSharedPointer<ActiveInterface> slaveInterface(new ActiveInterface(instance1, interface1));
    connection->setStartInterface(slaveInterface);

    QSharedPointer<ActiveInterface> mirroredSlave(new ActiveInterface(instance2, interface2));
    connection->getActiveInterfaces()->append(mirroredSlave);

    design_->getInterconnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_MemoryViewGenerator::createHierarchicalConnection(QString const& topInterface,
    QString const& componentInstance, QString const& componentInterface, QSharedPointer<Design> design)
{
    QSharedPointer<Interconnection> hierarchicalConnection(new Interconnection());
    QSharedPointer<ActiveInterface> startInterface(new ActiveInterface(componentInstance, componentInterface));
    hierarchicalConnection->setStartInterface(startInterface);

    QSharedPointer<HierInterface> hierInterface(new HierInterface(topInterface));
    hierarchicalConnection->getHierInterfaces()->append(hierInterface);
    design->getInterconnections()->append(hierarchicalConnection);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryViewGenerator::addAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_MemoryViewGenerator::addAddressBlock(QString const& name,
    QString const& baseAddress, QString const& range, QString const& width, 
    QSharedPointer<MemoryMap> containingMemoryMap)
{
    QSharedPointer<AddressBlock> block(new AddressBlock(name, baseAddress));
    block->setRange(range);
    block->setWidth(width);
    containingMemoryMap->getMemoryBlocks()->append(block);

    return block;
}


QTEST_APPLESS_MAIN(tst_MemoryViewGenerator)

#include "tst_MemoryViewGenerator.moc"
