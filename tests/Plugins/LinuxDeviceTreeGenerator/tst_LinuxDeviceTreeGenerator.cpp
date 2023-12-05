//-----------------------------------------------------------------------------
// File: tst_LinuxDeviceTreeGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Unit test for class LinuxDeviceTreeGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <IPXACTmodels/Component/Channel.h>

#include <IPXACTmodels/Design/Design.h>

#include <editors/MemoryDesigner/MasterSlavePathSearch.h>

#include <Plugins/LinuxDeviceTree/LinuxDeviceTreeGenerator.h>

#include <tests/MockObjects/LibraryMock.h>

class tst_LinuxDeviceTreeGenerator : public QObject
{
    Q_OBJECT

public:
    tst_LinuxDeviceTreeGenerator();

private slots:

    void init();
    void cleanup();

    void testWriteSimpleCPU();

    void testWriteSimpleConnection();

    void testConnectToMemory();
    void testConnectToMemoryAndMap();

    void testWriteSingleConnectionWithMultipleCPUs();
    void testWriteMultipleCPUs();

    void testSingleCPUInMultipleMasterInterfaces();

    void testAddressChange();
    void testAddressChangeInMirroredSlave();

    void testCPUInMiddleOfPath();

    void testBusPath();

    void testHierarchicalPath();

    void testMemoryInAddressBlock();
private:

    QString runGenerator(QString const& activeView, bool generateAddressBlocks = false);

    QString getFileContent(QString const& fileName);

    QSharedPointer<AddressSpace> createAddressSpace(QString const& spaceName, QString const& range,
        QString const& width, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<Cpu> createCPU(QString const& cpuName, QString const& spaceReference,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createMasterBusInterface(QString const& busName,
        QSharedPointer<AddressSpace> referencedSpace, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createSlaveBusInterface(QString const& busName,
        QSharedPointer<MemoryMap> referencedMap, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createMirroredSlaveBusInterface(QString const& busName,
        QString const& remapAddress, QString const& remapRange, QSharedPointer<Component> containingComponent)
        const;

    QSharedPointer<BusInterface> createMirroredMasterBusInterface(QString const& busName,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createBusInterface(QString const& busName, General::InterfaceMode mode,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<Channel> createChannel(QString const& channelName, QStringList const& channeledInterfaces,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<MemoryMap> createMemoryMap(QString const& mapName,
        QSharedPointer<Component> containingComponent) const;

    void createBlockForMap(QString const& blockName, QString const& blockBaseAddress, QString const& blockWidth,
        QString const& blockRange, QSharedPointer<MemoryMap> targetMap);

    QSharedPointer<ComponentInstance> createComponentInstance(VLNV const& vlnv, QString const& name,
        QString const& id, QSharedPointer<Design> design) const;

    void createInterconnection(QSharedPointer<ActiveInterface> startInterface,
        QSharedPointer<ActiveInterface> endInterface, QSharedPointer<Design> design);

    void createHierarchicalConnection(QSharedPointer<HierInterface> startInterface,
        QSharedPointer<ActiveInterface> endInterface, QSharedPointer<Design> design);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QSharedPointer<Component> topComponent_;

    QSharedPointer<View> hierarchicalView_;

    QSharedPointer<Design> design_;

    LibraryMock* library_;
};

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::tst_LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
tst_LinuxDeviceTreeGenerator::tst_LinuxDeviceTreeGenerator():
topComponent_(),
hierarchicalView_(),
design_(),
library_(new LibraryMock(this))
{

}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::init()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TopComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv, Document::Revision::Std14));

    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    design_ = QSharedPointer<Design>(new Design(designVlnv, Document::Revision::Std14));
    library_->addComponent(design_);

    hierarchicalView_ = QSharedPointer<View>(new View("hierarchical"));
    hierarchicalView_->setDesignInstantiationRef("design");
    topComponent_->getViews()->append(hierarchicalView_);

    QSharedPointer<DesignInstantiation> designInstantiation(new DesignInstantiation("design"));
    designInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(design_->getVlnv())));
    topComponent_->getDesignInstantiations()->append(designInstantiation);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::cleanup()
{
    library_->clear();

    QFile outputFile("./TopComponent_0.dts");
    outputFile.remove();
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteSimpleCPU()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteSimpleCPU()
{
    QString output = runGenerator("hierarchical");
    QString expectedOutput = "";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteSimpleConnection()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteSimpleConnection()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent (new Component(cpuVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent (new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testConnectToMemory()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testConnectToMemory()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<AddressBlock> newBlock(new AddressBlock("memoryBlock"));
    newBlock->setUsage(General::MEMORY);
    newBlock->setBaseAddress("0");
    newBlock->setWidth("8");
    newBlock->setRange("16");

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    testMap->getMemoryBlocks()->append(newBlock);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"        
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\tmemory@0 {\n"
                "\t\tdevice_type = \"memory\";\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x10>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testConnectToMemoryAndMap()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testConnectToMemoryAndMap()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    VLNV notMemoryVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "notMemoryComponent", "1.0");

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));
    QSharedPointer<Component> notMemoryComponent(new Component(notMemoryVLNV, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);
    library_->addComponent(notMemoryComponent);

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<AddressBlock> newBlock(new AddressBlock("memoryBlock"));
    newBlock->setUsage(General::MEMORY);
    newBlock->setBaseAddress("0");
    newBlock->setWidth("8");
    newBlock->setRange("16");

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    testMap->getMemoryBlocks()->append(newBlock);
    QSharedPointer<MemoryMap> notMemory = createMemoryMap("notMemory", notMemoryComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);
    QSharedPointer<BusInterface> notMemoryBus = createSlaveBusInterface("notMemoryBus", notMemory, notMemoryComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);
    QSharedPointer<ComponentInstance> notMemoryInstance =
        createComponentInstance(notMemoryVLNV, "notMemoryInstance", "notMemoryID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));
    QSharedPointer<ActiveInterface> notMapInterface(
        new ActiveInterface(notMemoryInstance->getInstanceName(), notMemoryBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);
    createInterconnection(cpuInterface, notMapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"        
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'notMemory' in instance 'notMemoryInstance' of component Test:TestLibrary:notMemoryComponent:1.0\n"
            "\tnotMemory@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\tmemory@0 {\n"
                "\t\tdevice_type = \"memory\";\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x10>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteSingleConnectionWithMultipleCPUs()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteSingleConnectionWithMultipleCPUs()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);
    QSharedPointer<Cpu> secondTestCPU = createCPU("secondTestCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
                "\t\t// 'secondTestCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@1 {\n"
                    "\t\t\treg = <1>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteMultipleCPUs()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteMultipleCPUs()
{
    //! 1st CPU
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);
    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("CPUBus", testSpace, cpuComponent);

    //! 2nd CPU
    VLNV cpuVLNV2(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU2", "1.0");
    QSharedPointer<Component> secondCpuComponent(new Component(cpuVLNV2, Document::Revision::Std14));

    QSharedPointer<AddressSpace> secondCPUSpace =
        createAddressSpace(QStringLiteral("secondCPUSpace"), "16", "16", secondCpuComponent);
    QSharedPointer<Cpu> secondTestCPU = createCPU("secondTestCPU", secondCPUSpace->name(), secondCpuComponent);
    QSharedPointer<BusInterface> secondCpuBus =
        createMasterBusInterface("secondCPUBus", secondCPUSpace, secondCpuComponent);

    //! 1st Map
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    //! 2nd Map
    VLNV secondMapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "SecondMapComponent", "1.0");
    QSharedPointer<Component> secondMapComponent(new Component(secondMapVlnv, Document::Revision::Std14));

    QSharedPointer<MemoryMap> secondTestMap = createMemoryMap("secondTestMap", secondMapComponent);
    QSharedPointer<BusInterface> secondMapBus =
        createSlaveBusInterface("secondMapBus", secondTestMap, secondMapComponent);

    library_->addComponent(cpuComponent);
    library_->addComponent(secondCpuComponent);
    library_->addComponent(mapComponent);
    library_->addComponent(secondMapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);
    QSharedPointer<ComponentInstance> secondCpuInstance =
        createComponentInstance(cpuVLNV2, "secondCpuInstance", "secondCpuID", design_);
    QSharedPointer<ComponentInstance> secondMapInstance =
        createComponentInstance(secondMapVlnv, "secondMapInstance", "secondMapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));
    QSharedPointer<ActiveInterface> secondCpuInterface(
        new ActiveInterface(secondCpuInstance->getInstanceName(), secondCpuBus->name()));
    QSharedPointer<ActiveInterface> secondMapInterface(
        new ActiveInterface(secondMapInstance->getInstanceName(), secondMapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);
    createInterconnection(secondCpuInterface, secondMapInterface, design_);

    QString output0 = runGenerator(hierarchicalView_->name());

    QString expectedOutput0 =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
        "\tcpus {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
        "\t\tcpu@0 {\n"
        "\t\t\treg = <0>;\n"
        "\t\t};\n"
        "\t};\n\n"
        "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
        "\ttestMap@0 {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <1>;\n"
        "\t\treg = <0x0 0x1>;\n"
        "\t};\n\n"
        "};\n\n";
        
    QCOMPARE(output0, expectedOutput0);

    QString expectedOutputFile1 =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
        "\tcpus {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <0>;\n\n"
        "\t\t// 'secondTestCPU' in component Test:TestLibrary:testCPU2:1.0\n"
        "\t\tcpu@0 {\n"
        "\t\t\treg = <0>;\n"
        "\t\t};\n"
        "\t};\n\n"
        "\t// Memory map 'secondTestMap' in instance 'secondMapInstance' of component Test:TestLibrary:SecondMapComponent:1.0\n"
        "\tsecondTestMap@0 {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <1>;\n"
        "\t\treg = <0x0 0x1>;\n"
        "\t};\n\n"
        "};\n\n";

    QString output1 = getFileContent("TopComponent_1.dts");
    QCOMPARE(output1, expectedOutputFile1);

}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testSingleCPUInMultipleMasterInterfaces()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testSingleCPUInMultipleMasterInterfaces()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<AddressSpace> secondTestSpace =
        createAddressSpace(QStringLiteral("testSpace2"), "8", "8", cpuComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);

    QSharedPointer<Cpu> testCPU = createCPU("testCPU", "", cpuComponent);
    testCPU->setAddressSpaceRefs(cpuComponent->getAddressSpaceNames());

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("cpuBus", testSpace, cpuComponent);
    QSharedPointer<BusInterface> cpu2B = createMasterBusInterface("cpuTwoBus", secondTestSpace, cpuComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);
    QSharedPointer<ComponentInstance> secondMapInstance =
        createComponentInstance(mapVlnv, "secondMapInstance", "secondMapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> secondCPUInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpu2B->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));
    QSharedPointer<ActiveInterface> map9S(
        new ActiveInterface(secondMapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);
    createInterconnection(secondCPUInterface, map9S, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n"
            "\n"
            "\t// Memory map 'testMap' in instance 'secondMapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testAddressChange()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testAddressChange()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    cpuBus->getMaster()->setBaseAddress(QString("10"));

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    createBlockForMap(QString("testBlock"), QString("0"), QString("8"), QString("16"), testMap);

    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@a {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0xa 0x10>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testAddressChangeInMirroredSlave()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testAddressChangeInMirroredSlave()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    VLNV channelVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "channelComponent", "1.0");

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));
    QSharedPointer<Component> channelComponent(new Component(channelVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    createBlockForMap(QString("testBlock"), QString("0"), QString("8"), QString("16"), testMap);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    cpuBus->getMaster()->setBaseAddress(QString("10"));
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<BusInterface> mirrorSlaveBus =
        createMirroredSlaveBusInterface("mirrorSlave", "1000", "100", channelComponent);
    QSharedPointer<BusInterface> mirrorMasterBus =
        createMirroredMasterBusInterface("mirrorMaster", channelComponent);

    QSharedPointer<Channel> mirrorChannel =
        createChannel("mirrorChannel", channelComponent->getBusInterfaceNames(), channelComponent);

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);
    library_->addComponent(channelComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);
    QSharedPointer<ComponentInstance> channelInstance =
        createComponentInstance(channelVLNV, "channelInstance", "channelID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));
    QSharedPointer<ActiveInterface> mirrorSlaveInterface(
        new ActiveInterface(channelInstance->getInstanceName(), mirrorSlaveBus->name()));
    QSharedPointer<ActiveInterface> mirrorMasterInterface(
        new ActiveInterface(channelInstance->getInstanceName(), mirrorMasterBus->name()));

    createInterconnection(cpuInterface, mirrorMasterInterface, design_);
    createInterconnection(mirrorSlaveInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
        "\t// Instance 'channelInstance' of channel component Test:TestLibrary:channelComponent:1.0\n"
        "\tchannelComponent {\n"
            "\t\tcompatible = \"simple-bus\";\n"
            "\t\t#address-cells = <1>;\n"
            "\t\t#size-cells = <1>;\n"
            "\n"
            "\t\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
                "\t\ttestMap@3f2 {\n"
                "\t\t\t#address-cells = <1>;\n"
                "\t\t\t#size-cells = <1>;\n"
                "\t\t\treg = <0x3f2 0x64>;\n"
                "\t\t};\n\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testCPUInMiddleOfPath()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testCPUInMiddleOfPath()
{
    //! First component: space, no CPU.
    VLNV startVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testStart", "1.0");
    QSharedPointer<Component> startComponent(new Component(startVLNV, Document::Revision::Std14));
    QSharedPointer<AddressSpace> startSpace =
        createAddressSpace(QStringLiteral("startSpace"), "8", "8", startComponent);
    QSharedPointer<BusInterface> startBus = createMasterBusInterface("startBus", startSpace, startComponent);

    //! Second component: bridged slave-master, space, CPU.
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);
    QSharedPointer<BusInterface> bridgedCpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    QSharedPointer<BusInterface> bridgedSlaveBus =
        createSlaveBusInterface("bridgeSlave", QSharedPointer<MemoryMap>(), cpuComponent);
    QSharedPointer<TransparentBridge> slaveCPUBridge(new TransparentBridge(bridgedCpuBus->name()));
    bridgedSlaveBus->getSlave()->getBridges()->append(slaveCPUBridge);

    //! Third component: slave map.
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));
    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    createBlockForMap(QString("testBlock"), QString("0"), QString("16"), QString("32"), testMap);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    library_->addComponent(startComponent);
    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<ComponentInstance> startInstance =
        createComponentInstance(startVLNV, "startInstance", "startID", design_);
    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);


    QSharedPointer<ActiveInterface> startInterface(
        new ActiveInterface(startInstance->getInstanceName(), startBus->name()));
    QSharedPointer<ActiveInterface> bridgeSlaveInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), bridgedSlaveBus->name()));
    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), bridgedCpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(startInterface, bridgeSlaveInterface, design_);
    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x20>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testBusPath()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testBusPath()
{
    VLNV cpuVLNV(VLNV::COMPONENT, QLatin1String("Test"), QLatin1String("TestLibrary"), QLatin1String("testCPU"),
        QLatin1String("1.0"));
    VLNV axiBusVLNV(VLNV::COMPONENT, QLatin1String("Test"), QLatin1String("TestLibrary"), QLatin1String("axe"),
        QLatin1String("1.0"));
    VLNV periphBusVLNV(VLNV::COMPONENT, QLatin1String("Test"), QLatin1String("TestLibrary"),
        QLatin1String("periph_bus_wrap"), QLatin1String("1.0"));
    VLNV mapVlnv(VLNV::COMPONENT, QLatin1String("Test"), QLatin1String("TestLibrary"),
        QLatin1String("MapComponent"), QLatin1String("1.0"));

    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));
    QSharedPointer<Component> axiComponent(new Component(axiBusVLNV, Document::Revision::Std14));
    QSharedPointer<Component> periphBusComponent(new Component(periphBusVLNV, Document::Revision::Std14));
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(axiComponent);
    library_->addComponent(periphBusComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QLatin1String("testSpace"), QLatin1String("16"), QLatin1String("16"), cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU(QLatin1String("testCPU"), testSpace->name(), cpuComponent);
    QSharedPointer<BusInterface> cpuBus =
        createMasterBusInterface(QLatin1String("topBus"), testSpace, cpuComponent);

    QSharedPointer<BusInterface> axiMaster =
        createMasterBusInterface(QLatin1String("axiMaster"), QSharedPointer<AddressSpace>(), axiComponent);
    QSharedPointer<BusInterface> axiSlave =
        createSlaveBusInterface(QLatin1String("axiSlave"), QSharedPointer<MemoryMap>(), axiComponent);
    QSharedPointer<TransparentBridge> axiSlaveBridge(new TransparentBridge(axiMaster->name()));
    axiSlave->getSlave()->getBridges()->append(axiSlaveBridge);

    QSharedPointer<BusInterface> wrapMaster =
        createMasterBusInterface(QLatin1String("wrapMaster"), QSharedPointer<AddressSpace>(), periphBusComponent);
    QSharedPointer<BusInterface> wrapSlave =
        createSlaveBusInterface(QLatin1String("wrapSlave"), QSharedPointer<MemoryMap>(), periphBusComponent);
    QSharedPointer<TransparentBridge> wrapBridge(new TransparentBridge(wrapMaster->name()));
    wrapSlave->getSlave()->getBridges()->append(wrapBridge);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> axiInstance =
        createComponentInstance(axiBusVLNV, "axiInstance", "axiID", design_);
    QSharedPointer<ComponentInstance> wrapInstance =
        createComponentInstance(periphBusVLNV, "wrapInstance", "wrapID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance_0", "mapID", design_);
    QSharedPointer<ComponentInstance> secondMapInstance =
        createComponentInstance(mapVlnv, "mapInstance_1", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> axiMasterInterface(
        new ActiveInterface(axiInstance->getInstanceName(), axiMaster->name()));
    QSharedPointer<ActiveInterface> axiSlaveInterface(
        new ActiveInterface(axiInstance->getInstanceName(), axiSlave->name()));
    QSharedPointer<ActiveInterface> wrapMasterInterface(
        new ActiveInterface(wrapInstance->getInstanceName(), wrapMaster->name()));
    QSharedPointer<ActiveInterface> wrapSlaveInterface(
        new ActiveInterface(wrapInstance->getInstanceName(), wrapSlave->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));
    QSharedPointer<ActiveInterface> secondMapInterface(
        new ActiveInterface(secondMapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, axiSlaveInterface, design_);
    createInterconnection(axiMasterInterface, wrapSlaveInterface, design_);
    createInterconnection(wrapMasterInterface, mapInterface, design_);
    createInterconnection(wrapMasterInterface, secondMapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n"
            "\n"
            "\t// Instance 'axiInstance' of bridge component Test:TestLibrary:axe:1.0\n"
            "\taxe {\n"
                "\t\tcompatible = \"simple-bus\";\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\n"
                "\t\t// Instance 'wrapInstance' of bridge component Test:TestLibrary:periph_bus_wrap:1.0\n"
                "\t\tperiph_bus_wrap {\n"
                    "\t\t\tcompatible = \"simple-bus\";\n"
                    "\t\t\t#address-cells = <1>;\n"
                    "\t\t\t#size-cells = <1>;\n"
                    "\n"
                    "\t\t\t// Memory map 'testMap' in instance 'mapInstance_0' of component Test:TestLibrary:MapComponent:1.0\n"
                    "\t\t\ttestMap@0 {\n"
                        "\t\t\t\t#address-cells = <1>;\n"
                        "\t\t\t\t#size-cells = <1>;\n"
                        "\t\t\t\treg = <0x0 0x1>;\n"
                    "\t\t\t};\n"
                    "\n"
                    "\t\t\t// Memory map 'testMap' in instance 'mapInstance_1' of component Test:TestLibrary:MapComponent:1.0\n"
                    "\t\t\ttestMap@0 {\n"
                        "\t\t\t\t#address-cells = <1>;\n"
                        "\t\t\t\t#size-cells = <1>;\n"
                        "\t\t\t\treg = <0x0 0x1>;\n"
                    "\t\t\t};\n"
                    "\n"
                "\t\t};\n"
                "\n"
            "\t};\n"
            "\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testHierarchicalPath()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testHierarchicalPath()
{
    VLNV middleComponentVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "hierarchicalComponent", "1.0");
    QSharedPointer<Component> middleComponent = QSharedPointer<Component>(new Component(middleComponentVLNV, 
        Document::Revision::Std14));

    VLNV middleDesignVLNV(VLNV::DESIGN, "Test", "TestLibrary", "hierarchicalDesign", "1.0");
    QSharedPointer<Design> middleDesign = QSharedPointer<Design>(new Design(middleDesignVLNV, Document::Revision::Std14));

    library_->addComponent(middleDesign);
    library_->addComponent(middleComponent);

    QSharedPointer<View> middleView = QSharedPointer<View>(new View("hierarchical"));
    middleView->setDesignInstantiationRef("design");
    middleComponent->getViews()->append(middleView);

    QSharedPointer<DesignInstantiation> middleDesignInstantiation(new DesignInstantiation("design"));
    middleDesignInstantiation->setDesignReference(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(middleDesign->getVlnv())));
    middleComponent->getDesignInstantiations()->append(middleDesignInstantiation);

    QSharedPointer<AddressSpace> middleSpace =
        createAddressSpace(QStringLiteral("middleSpace"), "0", "0", middleComponent);

    QSharedPointer<BusInterface> middleBus = createMasterBusInterface("middleBus", middleSpace, middleComponent);

    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("cpuBus", testSpace, cpuComponent);

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", middleDesign);
    QSharedPointer<ComponentInstance> middleIInstance =
        createComponentInstance(middleComponentVLNV, "middleInstance", "middleID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> middleInterface(
        new ActiveInterface(middleIInstance->getInstanceName(), middleBus->name()));
    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createHierarchicalConnection(middleInterface, cpuInterface, middleDesign);
    createInterconnection(middleInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <0>;\n\n"
                "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
            "\ttestMap@0 {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n"
                "\t\treg = <0x0 0x1>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testMemoryInAddressBlock()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testMemoryInAddressBlock()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV, Document::Revision::Std14));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv, Document::Revision::Std14));

    library_->addComponent(cpuComponent);
    library_->addComponent(mapComponent);

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    createBlockForMap("testBlock", "0", "32", "8", testMap);
    testMap->getMemoryBlocks()->first().dynamicCast<AddressBlock>()->setUsage(General::MEMORY);
    createBlockForMap("extraBlock", "8", "32", "8", testMap);

    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    QSharedPointer<ComponentInstance> cpuInstance =
        createComponentInstance(cpuVLNV, "cpuInstance", "cpuID", design_);
    QSharedPointer<ComponentInstance> mapInstance =
        createComponentInstance(mapVlnv, "mapInstance", "mapID", design_);

    QSharedPointer<ActiveInterface> cpuInterface(
        new ActiveInterface(cpuInstance->getInstanceName(), cpuBus->name()));
    QSharedPointer<ActiveInterface> mapInterface(
        new ActiveInterface(mapInstance->getInstanceName(), mapBus->name()));

    createInterconnection(cpuInterface, mapInterface, design_);

    QString output = runGenerator(hierarchicalView_->name(), true);

    QString expectedOutput =
        "/dts-v1/;\n\n"
        "/ {\n"
        "\t#address-cells = <1>;\n"
        "\t#size-cells = <1>;\n"
        "\tmodel = \"Test,TestDesign\";\n"
        "\tcompatible = \"Test,TestDesign\";\n"
        "\n"
        "\tcpus {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <0>;\n\n"
        "\t\t// 'testCPU' in component Test:TestLibrary:testCPU:1.0\n"
        "\t\tcpu@0 {\n"
        "\t\t\treg = <0>;\n"
        "\t\t};\n"
        "\t};\n\n"
        "\t// Memory map 'testMap' in instance 'mapInstance' of component Test:TestLibrary:MapComponent:1.0\n"
        "\ttestMap@0 {\n"
        "\t\t#address-cells = <1>;\n"
        "\t\t#size-cells = <1>;\n"
        "\t\treg = <0x0 0x10>;\n"
        "\n"
        "\t\t// Address block 'testBlock'\n"
        "\t\ttestBlock@0 {\n"
        "\t\t\tcompatible = \"Test,testBlock\";\n"
        "\t\t\tstatus = \"okay\";\n"       
        "\t\t\tdevice_type = \"memory\";\n"
        "\t\t};\n\n"
        "\t\t// Address block 'extraBlock'\n"
        "\t\textraBlock@8 {\n"
        "\t\t\tcompatible = \"Test,extraBlock\";\n"
        "\t\t\tstatus = \"okay\";\n"
        "\t\t};\n"
        "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::runGenerator()
//-----------------------------------------------------------------------------
QString tst_LinuxDeviceTreeGenerator::runGenerator(QString const& activeView, bool generateAddressBlocks)
{
    LinuxDeviceTreeGenerator generator(library_);
    QString outPutFileName = "TopComponent_0.dts";

    ConnectivityGraphFactory graphFactory(library_);
    QSharedPointer<ConnectivityGraph> graph = graphFactory.createConnectivityGraph(topComponent_, activeView);

    MasterSlavePathSearch searchAlgorithm;
    QVector < QSharedPointer<ConnectivityInterface> > masterRoots = searchAlgorithm.findMasterSlaveRoots(graph);

    auto cpuContainers =
        LinuxDeviceTreeCPUDetails::getCPUContainers(topComponent_->getVlnv().getName(), topComponent_, activeView, library_);

    generator.generate(topComponent_, activeView, generateAddressBlocks, cpuContainers, "./");

    return getFileContent(outPutFileName);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::getFileContent()
//-----------------------------------------------------------------------------
QString tst_LinuxDeviceTreeGenerator::getFileContent(QString const& fileName)
{
    QFile outputFile("./" + fileName);

    outputFile.open(QIODevice::ReadOnly);
    QString output = outputFile.readAll();
    outputFile.close();

    return output;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> tst_LinuxDeviceTreeGenerator::createAddressSpace(QString const& spaceName,
    QString const& range, QString const& width, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<AddressSpace> newSpace(new AddressSpace(spaceName, range, width));

    containingComponent->getAddressSpaces()->append(newSpace);

    return newSpace;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createCPU()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> tst_LinuxDeviceTreeGenerator::createCPU(QString const& cpuName, QString const& spaceReference,
    QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<Cpu> newCPU(new Cpu(cpuName));

    QSharedPointer<Cpu::AddressSpaceRef> newSpaceReference(new Cpu::AddressSpaceRef(spaceReference));
    newCPU->getAddressSpaceReferences()->append(newSpaceReference);

    containingComponent->getCpus()->append(newCPU);

    return newCPU;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createMasterBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_LinuxDeviceTreeGenerator::createMasterBusInterface(QString const& busName,
    QSharedPointer<AddressSpace> referencedSpace, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<BusInterface> newMasterBus = createBusInterface(busName, General::MASTER, containingComponent);

    if (referencedSpace)
    {
        QSharedPointer<InitiatorInterface> newMasterInterface(new InitiatorInterface());
        newMasterInterface->setAddressSpaceRef(referencedSpace->name());
        newMasterBus->setMaster(newMasterInterface);
    }

    return newMasterBus;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createSlaveBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_LinuxDeviceTreeGenerator::createSlaveBusInterface(QString const& busName,
    QSharedPointer<MemoryMap> referencedMap, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<BusInterface> newSlaveBus = createBusInterface(busName, General::SLAVE, containingComponent);

    QSharedPointer<TargetInterface> newSlaveInterface(new TargetInterface());

    if (referencedMap)
    {
        newSlaveInterface->setMemoryMapRef(referencedMap->name());
    }

    newSlaveBus->setSlave(newSlaveInterface);

    return newSlaveBus;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createMirroredSlaveBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_LinuxDeviceTreeGenerator::createMirroredSlaveBusInterface(QString const& busName,
    QString const& remapAddress, QString const& remapRange, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<BusInterface> newMirrorSlaveBus =
        createBusInterface(busName, General::MIRRORED_SLAVE, containingComponent);

    QSharedPointer<MirroredTargetInterface> newMirrorSlaveInterface(new MirroredTargetInterface());
    newMirrorSlaveInterface->setRange(remapRange);
    newMirrorSlaveInterface->setRemapAddress(remapAddress);

    newMirrorSlaveBus->setMirroredSlave(newMirrorSlaveInterface);

    return newMirrorSlaveBus;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createMirroredMasterBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_LinuxDeviceTreeGenerator::createMirroredMasterBusInterface(QString const& busName,
    QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<BusInterface> newMirrorMasterBus =
        createBusInterface(busName, General::MIRRORED_MASTER, containingComponent);

    return newMirrorMasterBus;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_LinuxDeviceTreeGenerator::createBusInterface(QString const& busName,
    General::InterfaceMode mode, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<BusInterface> newBus(new BusInterface());
    newBus->setName(busName);
    newBus->setInterfaceMode(mode);

    containingComponent->getBusInterfaces()->append(newBus);

    return newBus;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createChannel()
//-----------------------------------------------------------------------------
QSharedPointer<Channel> tst_LinuxDeviceTreeGenerator::createChannel(QString const& channelName,
    QStringList const& channeledInterfaces, QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<Channel> newChannel(new Channel());
    newChannel->setName(channelName);
    newChannel->setInterfaces(channeledInterfaces);

    containingComponent->getChannels()->append(newChannel);
    return newChannel;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> tst_LinuxDeviceTreeGenerator::createMemoryMap(QString const& mapName,
    QSharedPointer<Component> containingComponent) const
{
    QSharedPointer<MemoryMap> newMap(new MemoryMap(mapName));

    containingComponent->getMemoryMaps()->append(newMap);

    return newMap;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createBlockForMap()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::createBlockForMap(QString const& blockName, QString const& blockBaseAddress,
    QString const& blockWidth, QString const& blockRange, QSharedPointer<MemoryMap> targetMap)
{
    QSharedPointer<AddressBlock> newBlock(new AddressBlock(blockName));
    newBlock->setBaseAddress(blockBaseAddress);
    newBlock->setWidth(blockWidth);
    newBlock->setRange(blockRange);

    targetMap->getMemoryBlocks()->append(newBlock);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> tst_LinuxDeviceTreeGenerator::createComponentInstance(VLNV const& vlnv,
    QString const& name, QString const& id, QSharedPointer<Design> design) const
{
    QSharedPointer<ConfigurableVLNVReference> vlnvReference(new ConfigurableVLNVReference(vlnv));

    QSharedPointer<ComponentInstance> newInstance(new ComponentInstance(name, vlnvReference));
    newInstance->setUuid(id);

    design->getComponentInstances()->append(newInstance);

    return newInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createInterconnection()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::createInterconnection(QSharedPointer<ActiveInterface> startInterface,
    QSharedPointer<ActiveInterface> endInterface, QSharedPointer<Design> design)
{
    QSharedPointer<Interconnection> newConnection(new Interconnection());
    
    newConnection->setStartInterface(startInterface);
    newConnection->getActiveInterfaces()->append(endInterface);
    
    design->getInterconnections()->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::createHierarchicalConnection(QSharedPointer<HierInterface> startInterface,
    QSharedPointer<ActiveInterface> endInterface, QSharedPointer<Design> design)
{
    QSharedPointer<Interconnection> newConnection(new Interconnection());

    newConnection->setStartInterface(endInterface);
    newConnection->getHierInterfaces()->append(startInterface);

    design->getInterconnections()->append(newConnection);
}

QTEST_APPLESS_MAIN(tst_LinuxDeviceTreeGenerator)

#include "tst_LinuxDeviceTreeGenerator.moc"
