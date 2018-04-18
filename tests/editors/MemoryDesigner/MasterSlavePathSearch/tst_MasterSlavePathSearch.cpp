//-----------------------------------------------------------------------------
// File: tst_MasterSlavePathSearch.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.03.2018
//
// Description:
// Unit test for class MasterSlavePathSearch.
//-----------------------------------------------------------------------------

#include <designEditors/MemoryDesigner/MasterSlavePathSearch.h>
#include <designEditors/MemoryDesigner/ConnectivityGraph.h>

#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>
#include <designEditors/MemoryDesigner/ConnectivityConnection.h>
#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <QtTest>

namespace PathSearchSpace
{
    QString const MASTERMODE = "master";
    QString const SLAVEMODE = "slave";
    QString const MIRROREDMASTERMODE = "mirroredMaster";
    QString const MIRROREDSLAVEMODE = "mirroredSlave";
}

class tst_MasterSlavePathSearch : public QObject
{
    Q_OBJECT

public:
    tst_MasterSlavePathSearch();

private slots:

    void init();
    void cleanup();

    void testItemsWithoutConnections();

    void testSpaceMapConnection();

    void testAddressSpaceConnection();

    void testSpaceChainConnectionWithChainedItemsToDifferentMemoryMaps();

    void testContainedConnectionsAreNotExamined();

private:

    QSharedPointer<ConnectivityGraph> testGraph_;

    MasterSlavePathSearch pathSearcher_;

    QSharedPointer<ConnectivityInterface> createInterfaceWithMemoryItem(QString const& interfaceName,
        QString const& mode, QSharedPointer<ConnectivityComponent> containingInstance, bool hierarchical) const;

    void addLocalConnection(QSharedPointer<ConnectivityInterface> masterInterface);
};

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::tst_MasterSlavePathSearch()
//-----------------------------------------------------------------------------
tst_MasterSlavePathSearch::tst_MasterSlavePathSearch()
{
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::init()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::init()
{
    testGraph_ = QSharedPointer<ConnectivityGraph>(new ConnectivityGraph());
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::cleanup()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::cleanup()
{
    testGraph_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testItemsWithoutConnections()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testItemsWithoutConnections()
{
    QSharedPointer<ConnectivityComponent> testComponent1(new ConnectivityComponent("testComponent1"));
    QSharedPointer<ConnectivityComponent> testComponent2(new ConnectivityComponent("testComponent2"));

    QSharedPointer<ConnectivityInterface> testSpaceInterface =
        createInterfaceWithMemoryItem("testInterface1", PathSearchSpace::MASTERMODE, testComponent1, false);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testInterface2", PathSearchSpace::SLAVEMODE, testComponent2, false);

    testGraph_->getInstances().append(testComponent1);
    testGraph_->getInstances().append(testComponent2);
    testGraph_->getInterfaces().append(testSpaceInterface);
    testGraph_->getInterfaces().append(testMapInterface);

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testSpaceMapConnection()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testSpaceMapConnection()
{
    QSharedPointer<ConnectivityComponent> testComponent1(new ConnectivityComponent("testComponent1"));
    QSharedPointer<ConnectivityComponent> testComponent2(new ConnectivityComponent("testComponent2"));

    QSharedPointer<ConnectivityInterface> testSpaceInterface =
        createInterfaceWithMemoryItem("testInterface1", PathSearchSpace::MASTERMODE, testComponent1, false);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testInterface2", PathSearchSpace::SLAVEMODE, testComponent2, false);

    testGraph_->getInstances().append(testComponent1);
    testGraph_->getInstances().append(testComponent2);
    testGraph_->getInterfaces().append(testSpaceInterface);
    testGraph_->getInterfaces().append(testMapInterface);

    QSharedPointer<ConnectivityConnection> testConnection(
        new ConnectivityConnection("testConnection", testSpaceInterface, testMapInterface));

    testGraph_->getConnections().append(testConnection);

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().count(), 2);
    QCOMPARE(paths.first().first(), QSharedPointer<ConnectivityInterface const>(testSpaceInterface));
    QCOMPARE(paths.first().last(), QSharedPointer<ConnectivityInterface const>(testMapInterface));
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testItemsWithAddressSpaceConnection()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testAddressSpaceConnection()
{
    QSharedPointer<ConnectivityComponent> testSpaceComponent1(new ConnectivityComponent("testSpaceComponent1"));
    QSharedPointer<ConnectivityComponent> testSpaceComponent2(new ConnectivityComponent("testSpaceComponent2"));
    QSharedPointer<ConnectivityComponent> testMapComponent(new ConnectivityComponent("testMapComponent"));

    QSharedPointer<ConnectivityInterface> testSpaceInterface1 = createInterfaceWithMemoryItem(
        "testSpaceInterface1", PathSearchSpace::MASTERMODE, testSpaceComponent1, false);

    QSharedPointer<ConnectivityInterface> testSpaceInterface2 = createInterfaceWithMemoryItem(
        "testSpaceInterface2", PathSearchSpace::MASTERMODE, testSpaceComponent2, true);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testMapInterface", PathSearchSpace::SLAVEMODE, testMapComponent, false);

    testGraph_->getInstances().append(testSpaceComponent1);
    testGraph_->getInstances().append(testSpaceComponent2);
    testGraph_->getInstances().append(testMapComponent);
    testGraph_->getInterfaces().append(testSpaceInterface1);
    testGraph_->getInterfaces().append(testSpaceInterface2);
    testGraph_->getInterfaces().append(testMapInterface);

    QSharedPointer<ConnectivityConnection> testHierarchicalConnection(new ConnectivityConnection(
        "instanceToHierarchicalSpaceConnection", testSpaceInterface1, testSpaceInterface2));
    QSharedPointer<ConnectivityConnection> testSpaceMapConnection(new ConnectivityConnection(
        "testSpaceMapConnection", testSpaceInterface2, testMapInterface));

    testGraph_->getConnections().append(testHierarchicalConnection);
    testGraph_->getConnections().append(testSpaceMapConnection);

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().count(), 3);
    QCOMPARE(paths.first().at(0), QSharedPointer<ConnectivityInterface const>(testSpaceInterface1));
    QCOMPARE(paths.first().at(1), QSharedPointer<ConnectivityInterface const>(testSpaceInterface2));
    QCOMPARE(paths.first().at(2), QSharedPointer<ConnectivityInterface const>(testMapInterface));
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testSpaceChainConnectionWithChainedItemsToDifferentMemoryMaps()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testSpaceChainConnectionWithChainedItemsToDifferentMemoryMaps()
{
    QSharedPointer<ConnectivityComponent> spaceComponent1(new ConnectivityComponent("spaceComponent1"));
    QSharedPointer<ConnectivityComponent> spaceComponent2(new ConnectivityComponent("spaceComponent2"));
    QSharedPointer<ConnectivityComponent> mapComponent1(new ConnectivityComponent("mapComponent1"));
    QSharedPointer<ConnectivityComponent> mapComponent2(new ConnectivityComponent("mapComponent2"));

    QSharedPointer<ConnectivityInterface> spaceInterface1 =
        createInterfaceWithMemoryItem("spaceInterface1", PathSearchSpace::MASTERMODE, spaceComponent1, false);

    QSharedPointer<ConnectivityInterface> spaceInterface2 =
        createInterfaceWithMemoryItem("spaceInterface2", PathSearchSpace::MASTERMODE, spaceComponent2, true);

    QSharedPointer<ConnectivityInterface> mapInterface1 =
        createInterfaceWithMemoryItem("mapInterface1", PathSearchSpace::SLAVEMODE, mapComponent1, false);

    QSharedPointer<ConnectivityInterface> mapInterface2 =
        createInterfaceWithMemoryItem("mapInterface2", PathSearchSpace::SLAVEMODE, mapComponent2, false);

    testGraph_->getInstances().append(spaceComponent1);
    testGraph_->getInstances().append(spaceComponent2);
    testGraph_->getInstances().append(mapComponent1);
    testGraph_->getInstances().append(mapComponent2);

    testGraph_->getInterfaces().append(spaceInterface1);
    testGraph_->getInterfaces().append(spaceInterface2);
    testGraph_->getInterfaces().append(mapInterface1);
    testGraph_->getInterfaces().append(mapInterface2);

    QSharedPointer<ConnectivityConnection> spaceChain(
        new ConnectivityConnection("spaceChain", spaceInterface1, spaceInterface2));
    QSharedPointer<ConnectivityConnection> spaceToMap(
        new ConnectivityConnection("spaceToMap", spaceInterface1, mapInterface1));
    QSharedPointer<ConnectivityConnection> hierarchicalToMap(
        new ConnectivityConnection("hierarchicalToMap", spaceInterface2, mapInterface2));

    testGraph_->getConnections().append(spaceChain);
    testGraph_->getConnections().append(spaceToMap);
    testGraph_->getConnections().append(hierarchicalToMap);
    addLocalConnection(spaceInterface2);

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 3);

    QVector<QSharedPointer<ConnectivityInterface const> > hierarchicalPath= paths.first();
    QVector<QSharedPointer<ConnectivityInterface const> > spaceMapPath = paths.at(1);
    QVector<QSharedPointer<ConnectivityInterface const> > localPath = paths.last();

    QCOMPARE(hierarchicalPath.count(), 3);
    QCOMPARE(hierarchicalPath.at(0), QSharedPointer<ConnectivityInterface const>(spaceInterface1));
    QCOMPARE(hierarchicalPath.at(1), QSharedPointer<ConnectivityInterface const>(spaceInterface2));
    QCOMPARE(hierarchicalPath.at(2), QSharedPointer<ConnectivityInterface const>(mapInterface2));
    QCOMPARE(spaceMapPath.count(), 3);
    QCOMPARE(spaceMapPath.at(0), QSharedPointer<ConnectivityInterface const>(spaceInterface2));
    QCOMPARE(spaceMapPath.at(1), QSharedPointer<ConnectivityInterface const>(spaceInterface1));
    QCOMPARE(spaceMapPath.at(2), QSharedPointer<ConnectivityInterface const>(mapInterface1));
    QCOMPARE(localPath.count(), 2);
    QCOMPARE(localPath.at(0), QSharedPointer<ConnectivityInterface const>(spaceInterface2));
    QCOMPARE(localPath.at(1), QSharedPointer<ConnectivityInterface const>(spaceInterface2));
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testContainedConnectionsAreNotExamined()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testContainedConnectionsAreNotExamined()
{
    QSharedPointer<ConnectivityComponent> spaceComponent1(new ConnectivityComponent("spaceComponent1"));
    QSharedPointer<ConnectivityComponent> spaceComponent2(new ConnectivityComponent("spaceComponent2"));
    QSharedPointer<ConnectivityComponent> mapComponent1(new ConnectivityComponent("mapComponent1"));

    QSharedPointer<ConnectivityInterface> spaceInterface1 =
        createInterfaceWithMemoryItem("spaceInterface1", PathSearchSpace::MASTERMODE, spaceComponent1, false);

    QSharedPointer<ConnectivityInterface> spaceInterface2 =
        createInterfaceWithMemoryItem("spaceInterface2", PathSearchSpace::MASTERMODE, spaceComponent2, true);

    QSharedPointer<ConnectivityInterface> mapInterface1 =
        createInterfaceWithMemoryItem("mapInterface1", PathSearchSpace::SLAVEMODE, mapComponent1, false);

    testGraph_->getInstances().append(spaceComponent1);
    testGraph_->getInstances().append(spaceComponent2);
    testGraph_->getInstances().append(mapComponent1);

    testGraph_->getInterfaces().append(spaceInterface1);
    testGraph_->getInterfaces().append(spaceInterface2);
    testGraph_->getInterfaces().append(mapInterface1);

    QSharedPointer<ConnectivityConnection> spaceChain(
        new ConnectivityConnection("spaceChain", spaceInterface1, spaceInterface2));
    QSharedPointer<ConnectivityConnection> spaceToMap(
        new ConnectivityConnection("spaceToMap", spaceInterface2, mapInterface1));

    testGraph_->getConnections().append(spaceChain);
    testGraph_->getConnections().append(spaceToMap);

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().size(), 3);
    QCOMPARE(paths.first().at(0), QSharedPointer<ConnectivityInterface const>(spaceInterface1));
    QCOMPARE(paths.first().at(1), QSharedPointer<ConnectivityInterface const>(spaceInterface2));
    QCOMPARE(paths.first().at(2), QSharedPointer<ConnectivityInterface const>(mapInterface1));
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::createInterfaceWithMemoryItem()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> tst_MasterSlavePathSearch::createInterfaceWithMemoryItem(
    QString const& interfaceName, QString const& mode, QSharedPointer<ConnectivityComponent> containingInstance,
    bool isHierarchical) const
{
    QString memoryType("memoryMap");
    if (mode.contains(PathSearchSpace::MASTERMODE, Qt::CaseInsensitive))
    {
        memoryType = "addressSpace";
    }

    QString memoryItemName = interfaceName + "_" + memoryType;
    QSharedPointer<MemoryItem> testMemoryItem(new MemoryItem(memoryItemName, memoryType));

    QSharedPointer<ConnectivityInterface> testInterface(new ConnectivityInterface(interfaceName));
    testInterface->setConnectedMemory(testMemoryItem);
    testInterface->setMode(mode);
    if (containingInstance)
    {
        testInterface->setInstance(containingInstance);
    }
    if (isHierarchical)
    {
        testInterface->setHierarchical();
    }

    return testInterface;
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::addLocalConnection()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::addLocalConnection(QSharedPointer<ConnectivityInterface> masterInterface)
{
    if (masterInterface->getMode() != PathSearchSpace::MASTERMODE)
    {
        return;
    }

    QString localConnectionName = masterInterface->getName() + QStringLiteral("_to_local_memory_map_");

    QSharedPointer<ConnectivityConnection> localConnection(
        new ConnectivityConnection(localConnectionName, masterInterface, masterInterface));
    testGraph_->getConnections().append(localConnection);
}

QTEST_APPLESS_MAIN(tst_MasterSlavePathSearch)

#include "tst_MasterSlavePathSearch.moc"
