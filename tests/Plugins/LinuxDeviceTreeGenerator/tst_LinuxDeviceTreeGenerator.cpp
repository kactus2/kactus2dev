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
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/Cpu.h>

#include <IPXACTmodels/Design/Design.h>

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
    
    void testWriteMultipleCPUs();

    void testAddressChange();

private:

    QString runGenerator(QString const& activeView);

    QSharedPointer<AddressSpace> createAddressSpace(QString const& spaceName, QString const& range,
        QString const& width, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<Cpu> createCPU(QString const& cpuName, QString const& spaceReference,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createMasterBusInterface(QString const& busName,
        QSharedPointer<AddressSpace> referencedSpace, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createSlaveBusInterface(QString const& busName,
        QSharedPointer<MemoryMap> referencedMap, QSharedPointer<Component> containingComponent) const;

    QSharedPointer<BusInterface> createBusInterface(QString const& busName, General::InterfaceMode mode,
        QSharedPointer<Component> containingComponent) const;

    QSharedPointer<MemoryMap> createMemoryMap(QString const& mapName,
        QSharedPointer<Component> containingComponent) const;

    void createBlockForMap(QString const& blockName, QString const& blockBaseAddress, QString const& blockWidth,
        QString const& blockRange, QSharedPointer<MemoryMap> targetMap);

    QSharedPointer<ComponentInstance> createComponentInstance(VLNV const& vlnv, QString const& name,
        QString const& id, QSharedPointer<Design> design) const;

    void createInterconnection(QString const& instance1, QString const& interface1, QString const& instance2,
        QString const& interface2, QSharedPointer<Design> design) const;

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
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    design_ = QSharedPointer<Design>(new Design(designVlnv));
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

    QFile outputFile("./output.dts");
    outputFile.remove();
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteSimpleCPU()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteSimpleCPU()
{
    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", topComponent_);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), topComponent_);

    QSharedPointer<View> testView(new View("testView"));
    topComponent_->getViews()->append(testView);

    QString output = runGenerator(testView->name());
    QString expectedOutPut = "/dts-v0/;\n\n"
        "/ {\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n"
        "};\n\n";

    QCOMPARE(output, expectedOutPut);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteSimpleConnection()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteSimpleConnection()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent (new Component(cpuVLNV));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent (new Component(mapVlnv));

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

    QSharedPointer<Interconnection> testConnection(new Interconnection());
    testConnection->setStartInterface(cpuInterface);
    testConnection->getActiveInterfaces()->append(mapInterface);
    design_->getInterconnections()->append(testConnection);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutPut =
        "/dts-v0/;\n\n"
        "/ {\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\ttestMap@0 {\n"
            "\t\treg = <0x0 0x0>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutPut);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testWriteMultipleCPUs()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testWriteMultipleCPUs()
{
    //! 1st CPU
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);
    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("CPUBus", testSpace, cpuComponent);

    //! 2nd CPU
    VLNV cpuVLNV2(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU2", "1.0");
    QSharedPointer<Component> secondCpuComponent(new Component(cpuVLNV2));

    QSharedPointer<AddressSpace> secondCPUSpace =
        createAddressSpace(QStringLiteral("secondCPUSpace"), "16", "16", secondCpuComponent);
    QSharedPointer<Cpu> secondTestCPU = createCPU("secondTestCPU", secondCPUSpace->name(), secondCpuComponent);
    QSharedPointer<BusInterface> secondCpuBus =
        createMasterBusInterface("secondCPUBus", secondCPUSpace, secondCpuComponent);

    //! 1st Map
    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv));

    QSharedPointer<MemoryMap> testMap = createMemoryMap("testMap", mapComponent);
    QSharedPointer<BusInterface> mapBus = createSlaveBusInterface("mapBus", testMap, mapComponent);

    //! 2nd Map
    VLNV secondMapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "SecondMapComponent", "1.0");
    QSharedPointer<Component> secondMapComponent(new Component(secondMapVlnv));

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

    QSharedPointer<Interconnection> testConnection(new Interconnection());
    testConnection->setStartInterface(cpuInterface);
    testConnection->getActiveInterfaces()->append(mapInterface);

    QSharedPointer<Interconnection> secondTestConnection(new Interconnection());
    secondTestConnection->setStartInterface(secondCpuInterface);
    secondTestConnection->getActiveInterfaces()->append(secondMapInterface);

    design_->getInterconnections()->append(testConnection);
    design_->getInterconnections()->append(secondTestConnection);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutPut =
        "/dts-v0/;\n\n"
        "/ {\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\ttestMap@0 {\n"
                "\t\treg = <0x0 0x0>;\n"
            "\t};\n\n"
        "};\n\n"
        "/dts-v1/;\n\n"
        "/ {\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
            "\tsecondTestMap@0 {\n"
                "\t\treg = <0x0 0x0>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutPut);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::testAddressChange()
//-----------------------------------------------------------------------------
void tst_LinuxDeviceTreeGenerator::testAddressChange()
{
    VLNV cpuVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "testCPU", "1.0");
    QSharedPointer<Component> cpuComponent(new Component(cpuVLNV));

    QSharedPointer<AddressSpace> testSpace =
        createAddressSpace(QStringLiteral("testSpace"), "16", "16", cpuComponent);
    QSharedPointer<Cpu> testCPU = createCPU("testCPU", testSpace->name(), cpuComponent);

    QSharedPointer<BusInterface> cpuBus = createMasterBusInterface("topBus", testSpace, cpuComponent);
    cpuBus->getMaster()->setBaseAddress(QString("10"));

    VLNV mapVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "MapComponent", "1.0");
    QSharedPointer<Component> mapComponent(new Component(mapVlnv));

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

    QSharedPointer<Interconnection> testConnection(new Interconnection());
    testConnection->setStartInterface(cpuInterface);
    testConnection->getActiveInterfaces()->append(mapInterface);
    design_->getInterconnections()->append(testConnection);

    QString output = runGenerator(hierarchicalView_->name());

    QString expectedOutPut =
        "/dts-v0/;\n\n"
        "/ {\n"
            "\tcpus {\n"
                "\t\t#address-cells = <1>;\n"
                "\t\t#size-cells = <1>;\n\n"
                "\t\tcpu@0 {\n"
                    "\t\t\treg = <0>;\n"
                "\t\t};\n"
            "\t};\n\n"
                "\ttestMap@a {\n"
                "\t\treg = <0xa 0x19>;\n"
            "\t};\n\n"
        "};\n\n";

    QCOMPARE(output, expectedOutPut);
}

//-----------------------------------------------------------------------------
// Function: tst_LinuxDeviceTreeGenerator::runGenerator()
//-----------------------------------------------------------------------------
QString tst_LinuxDeviceTreeGenerator::runGenerator(QString const& activeView)
{
    LinuxDeviceTreeGenerator generator(library_);
    QString outPutFileName = "output.dts";
    generator.generate(topComponent_, activeView, outPutFileName);

    QFile outputFile("./" + outPutFileName);

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
        QSharedPointer<MasterInterface> newMasterInterface(new MasterInterface());
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
    
    if (referencedMap)
    {
        QSharedPointer<SlaveInterface> newSlaveInterface(new SlaveInterface());
        newSlaveInterface->setMemoryMapRef(referencedMap->name());
        newSlaveBus->setSlave(newSlaveInterface);
    }

    return newSlaveBus;
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

QTEST_APPLESS_MAIN(tst_LinuxDeviceTreeGenerator)

#include "tst_LinuxDeviceTreeGenerator.moc"
