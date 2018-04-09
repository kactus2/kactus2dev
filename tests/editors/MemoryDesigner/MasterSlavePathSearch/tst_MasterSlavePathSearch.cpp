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

private:

    QSharedPointer<ConnectivityGraph> testGraph_;

    MasterSlavePathSearch pathSearcher_;

    QSharedPointer<ConnectivityInterface> createInterfaceWithMemoryItem(QString const& interfaceName,
        QString const& mode) const;

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
        createInterfaceWithMemoryItem("testInterface1", PathSearchSpace::MASTERMODE);
    testSpaceInterface->setInstance(testComponent1);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testInterface2", PathSearchSpace::SLAVEMODE);
    testMapInterface->setInstance(testComponent2);

    testGraph_->addInstance(testComponent1);
    testGraph_->addInstance(testComponent2);
    testGraph_->addInterface(testSpaceInterface);
    testGraph_->addInterface(testMapInterface);

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().count(), 1);
    QCOMPARE(paths.first().first(), testSpaceInterface);
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testSpaceMapConnection()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testSpaceMapConnection()
{
    QSharedPointer<ConnectivityComponent> testComponent1(new ConnectivityComponent("testComponent1"));
    QSharedPointer<ConnectivityComponent> testComponent2(new ConnectivityComponent("testComponent2"));

    QSharedPointer<ConnectivityInterface> testSpaceInterface =
        createInterfaceWithMemoryItem("testInterface1", PathSearchSpace::MASTERMODE);
    testSpaceInterface->setInstance(testComponent1);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testInterface2", PathSearchSpace::SLAVEMODE);
    testMapInterface->setInstance(testComponent2);

    testGraph_->addInstance(testComponent1);
    testGraph_->addInstance(testComponent2);
    testGraph_->addInterface(testSpaceInterface);
    testGraph_->addInterface(testMapInterface);

    testGraph_->addConnection("testConnection", testSpaceInterface, testMapInterface);

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().count(), 2);
    QCOMPARE(paths.first().first(), testSpaceInterface);
    QCOMPARE(paths.first().last(), testMapInterface);
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::testItemsWithAddressSpaceConnection()
//-----------------------------------------------------------------------------
void tst_MasterSlavePathSearch::testAddressSpaceConnection()
{
    QSharedPointer<ConnectivityComponent> testSpaceComponent1(new ConnectivityComponent("testSpaceComponent1"));
    QSharedPointer<ConnectivityComponent> testSpaceComponent2(new ConnectivityComponent("testSpaceComponent2"));
    QSharedPointer<ConnectivityComponent> testMapComponent(new ConnectivityComponent("testMapComponent"));

    QSharedPointer<ConnectivityInterface> testSpaceInterface1 =
        createInterfaceWithMemoryItem("testSpaceInterface1", PathSearchSpace::MASTERMODE);
    testSpaceInterface1->setInstance(testSpaceComponent1);

    QSharedPointer<ConnectivityInterface> testSpaceInterface2 =
        createInterfaceWithMemoryItem("testSpaceInterface2", PathSearchSpace::MASTERMODE);
    testSpaceInterface2->setHierarchical();
    testSpaceInterface2->setInstance(testSpaceComponent2);

    QSharedPointer<ConnectivityInterface> testMapInterface =
        createInterfaceWithMemoryItem("testMapInterface", PathSearchSpace::SLAVEMODE);
    testMapInterface->setInstance(testMapComponent);

    testGraph_->addInstance(testSpaceComponent1);
    testGraph_->addInstance(testSpaceComponent2);
    testGraph_->addInstance(testMapComponent);
    testGraph_->addInterface(testSpaceInterface1);
    testGraph_->addInterface(testSpaceInterface2);
    testGraph_->addInterface(testMapInterface);

    testGraph_->addConnection("instanceToHierarchicalSpaceConnection", testSpaceInterface1, testSpaceInterface2);
    testGraph_->addConnection("testSpaceMapConnection", testSpaceInterface2, testMapInterface);

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 1);
    QCOMPARE(paths.first().count(), 3);
    QCOMPARE(paths.first().at(0), testSpaceInterface1);
    QCOMPARE(paths.first().at(1), testSpaceInterface2);
    QCOMPARE(paths.first().at(2), testMapInterface);
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
        createInterfaceWithMemoryItem("spaceInterface1", PathSearchSpace::MASTERMODE);
    spaceInterface1->setInstance(spaceComponent1);

    QSharedPointer<ConnectivityInterface> spaceInterface2 =
        createInterfaceWithMemoryItem("spaceInterface2", PathSearchSpace::MASTERMODE);
    spaceInterface2->setInstance(spaceComponent2);
    spaceInterface2->setHierarchical();

    QSharedPointer<ConnectivityInterface> mapInterface1 =
        createInterfaceWithMemoryItem("mapInterface1", PathSearchSpace::SLAVEMODE);
    mapInterface1->setInstance(mapComponent1);

    QSharedPointer<ConnectivityInterface> mapInterface2 =
        createInterfaceWithMemoryItem("mapInterface2", PathSearchSpace::SLAVEMODE);
    mapInterface2->setInstance(mapComponent2);

    testGraph_->addInstance(spaceComponent1);
    testGraph_->addInstance(spaceComponent2);
    testGraph_->addInstance(mapComponent1);
    testGraph_->addInstance(mapComponent2);

    testGraph_->addInterface(spaceInterface1);
    testGraph_->addInterface(spaceInterface2);
    testGraph_->addInterface(mapInterface1);
    testGraph_->addInterface(mapInterface2);

    testGraph_->addConnection("spaceChain", spaceInterface1, spaceInterface2);
    testGraph_->addConnection("spaceToMap", spaceInterface1, mapInterface1);
    testGraph_->addConnection("hierarchicalToMap", spaceInterface2, mapInterface2);
    addLocalConnection(spaceInterface2);

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > paths =
        pathSearcher_.findMasterSlavePaths(testGraph_);

    QCOMPARE(paths.count(), 3);

    QVector<QSharedPointer<ConnectivityInterface> > hierarchicalPath= paths.first();
    QVector<QSharedPointer<ConnectivityInterface> > spaceMapPath = paths.at(1);
    QVector<QSharedPointer<ConnectivityInterface> > localPath = paths.last();

    QCOMPARE(hierarchicalPath.count(), 3);
    QCOMPARE(hierarchicalPath.at(0), spaceInterface1);
    QCOMPARE(hierarchicalPath.at(1), spaceInterface2);
    QCOMPARE(hierarchicalPath.at(2), mapInterface2);
    QCOMPARE(spaceMapPath.count(), 2);
    QCOMPARE(spaceMapPath.at(0), spaceInterface1);
    QCOMPARE(spaceMapPath.at(1), mapInterface1);
    QCOMPARE(localPath.count(), 2);
    QCOMPARE(localPath.at(0), spaceInterface2);
    QCOMPARE(localPath.at(1), spaceInterface2);
}

//-----------------------------------------------------------------------------
// Function: tst_MasterSlavePathSearch::createInterfaceWithMemoryItem()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> tst_MasterSlavePathSearch::createInterfaceWithMemoryItem(
    QString const& interfaceName, QString const& mode) const
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
    testGraph_->addConnection(localConnectionName, masterInterface, masterInterface);
}

QTEST_APPLESS_MAIN(tst_MasterSlavePathSearch)

#include "tst_MasterSlavePathSearch.moc"
