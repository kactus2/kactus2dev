//-----------------------------------------------------------------------------
// File: tst_HierarchicalSaveBuildStrategy.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.05.2015
//
// Description:
// Unit test for class HierarchicalSaveBuildStrategy.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <tests/MockObjects/LibraryMock.h>

#include <mainwindow/SaveHierarchy/HierarchicalSaveBuildStrategy.h>
#include <mainwindow/SaveHierarchy/HierarchicalSaveColumns.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/vlnv.h>

class tst_HierarchicalSaveBuildStrategy : public QObject
{
    Q_OBJECT

public:
    tst_HierarchicalSaveBuildStrategy();

private slots:

    void cleanup();

    void testNullParameter();

    void testVLNVIsSetAndVersionIsUpdated();

    void testVLNVIsSetAndVersionIsUpdated_data();

    void testSuggestedNameDoesNotYetExistInLibrary();

    void testItemsAreCreatedOnlyForComponentsAndDesign();

    void testMultipleInstancesCreatesOneItem();

    void testDesignAndDesignConfiguration();

    void testCheckingIsRecursive();

    void testItemCannotBeCheckedIfParentNotChecked();

private:

    QTreeWidgetItem* buildFrom(QObject* rootObject);

    LibraryMock* library_;
    
};

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::tst_HierarchicalSaveBuildStrategy()
//-----------------------------------------------------------------------------
tst_HierarchicalSaveBuildStrategy::tst_HierarchicalSaveBuildStrategy(): library_(new LibraryMock(this))
{
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::tst_HierarchicalSaveBuildStrategy()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::cleanup()
{
    library_->clear();
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testNullParameter()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testNullParameter()
{
    HierarchicalSaveBuildStrategy strategy(0);

    QTreeWidgetItem* rootItem = strategy.build(0);

    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testVLNVIsSetAndVersionIsUpdated()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testVLNVIsSetAndVersionIsUpdated()
{
    QFETCH(QString, vlnv);
    QFETCH(QString, expectedItemName);
    QFETCH(QString, expectedUpdatedVlnv);

    QObject* rootObject = new QObject(0);
    rootObject->setObjectName(vlnv);
    rootObject->setProperty("VLNVType", "Component");

    VLNV existingVLNV(VLNV::COMPONENT, vlnv);
    QSharedPointer<Component> existingComponent(new Component(existingVLNV));
    library_->addComponent(existingComponent);

    QTreeWidgetItem* rootItem = buildFrom(rootObject);
        
    QCOMPARE(rootItem->columnCount(), 2);

    QCOMPARE(rootItem->text(HierarchicalSaveColumns::VLNV), expectedItemName);
    QCOMPARE(rootItem->text(HierarchicalSaveColumns::SAVE_AS_VLNV), expectedUpdatedVlnv);

    QCOMPARE(rootItem->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Checked);
    QVERIFY(rootItem->flags() & Qt::ItemIsEditable);
    
    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testVLNVIsSetAndVersionIsUpdated_data()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testVLNVIsSetAndVersionIsUpdated_data()
{
    QTest::addColumn<QString>("vlnv");
    QTest::addColumn<QString>("expectedItemName");
    QTest::addColumn<QString>("expectedUpdatedVlnv");

    QTest::newRow("empty string") << "" << "Item not found" << ":::";
    QTest::newRow("empty vlnv") << ":::" << "Item not found" << ":::";

    QTest::newRow("version with number is incremented") << "TestVendor:TestLibrary:TestVLNV:1.0" << 
        "TestVendor:TestLibrary:TestVLNV:1.0" << "TestVendor:TestLibrary:TestVLNV:1.1";

    QTest::newRow("version with increasing amount of digits") << "TestVendor:TestLibrary:TestVLNV:1.9" << 
        "TestVendor:TestLibrary:TestVLNV:1.9" << "TestVendor:TestLibrary:TestVLNV:1.10";

    QTest::newRow("version with characters appends a number") << "TestVendor:TestLibrary:TestVLNV:draft" << 
        "TestVendor:TestLibrary:TestVLNV:draft" << "TestVendor:TestLibrary:TestVLNV:draft_1";
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testSuggestedNameDoesNotYetExistInLibrary()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testSuggestedNameDoesNotYetExistInLibrary()
{
    VLNV originalVLNV(VLNV::COMPONENT, "TestVendor:TestLibrary:TestComponent:1.0");
    QSharedPointer<Component> originalComponent(new Component(originalVLNV));
    library_->addComponent(originalComponent);

    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestComponent:1.0");
    rootObject->setProperty("VLNVType", "Component");

    VLNV existingVLNV(VLNV::COMPONENT, "TestVendor:TestLibrary:TestComponent:1.1");
    QSharedPointer<Component> existingComponent(new Component(existingVLNV));
    library_->addComponent(existingComponent);

    QTreeWidgetItem* rootItem = buildFrom(rootObject);

    rootItem->setData(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::CheckStateRole, Qt::Unchecked);

    QCOMPARE(rootItem->text(HierarchicalSaveColumns::SAVE_AS_VLNV), QString("TestVendor:TestLibrary:TestComponent:1.2"));

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testItemsAreCreatedOnlyForComponentsAndDesign()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testItemsAreCreatedOnlyForComponentsAndDesign()
{
    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestComponent:1.0");
    rootObject->setProperty("VLNVType", "Component");

    QObject* designConfigurationObject = new QObject(rootObject);
    designConfigurationObject->setObjectName("TestVendor:TestLibrary:TestConfiguration:1.0");
    designConfigurationObject->setProperty("VLNVType", "DesignConfiguration");

    QObject* designObject = new QObject(designConfigurationObject);
    designObject->setObjectName("TestVendor:TestLibrary:TestDesign:1.0");
    designObject->setProperty("VLNVType", "Design");
    
    QObject* instanceObject = new QObject(designObject);
    instanceObject->setObjectName("TestVendor:TestLibrary:TestInstance:1.0");
    instanceObject->setProperty("VLNVType", "Component");

    QTreeWidgetItem* rootItem = buildFrom(rootObject);

    QCOMPARE(rootItem->childCount(), 1);
   
    QTreeWidgetItem* designItem = rootItem->child(0);
    QCOMPARE(designItem->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestDesign:1.0"));
    QCOMPARE(designItem->childCount(), 1);

    QTreeWidgetItem* instanceItem = designItem->child(0);
    QCOMPARE(instanceItem->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestInstance:1.0"));
    QCOMPARE(instanceItem->childCount(), 0);

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testItemsAreCreatedOnlyForComponentsAndDesign()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testMultipleInstancesCreatesOneItem()
{
    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestDesign:1.0");
    rootObject->setProperty("VLNVType", "Design");

    QObject* instanceObject = new QObject(rootObject);
    instanceObject->setObjectName("TestVendor:TestLibrary:TestInstance:1.0");
    instanceObject->setProperty("VLNVType", "Component");

    QObject* instanceDoubleObject = new QObject(rootObject);
    instanceDoubleObject->setObjectName("TestVendor:TestLibrary:TestInstance:1.0");
    instanceDoubleObject->setProperty("VLNVType", "Component");

    QObject* otherInstance = new QObject(rootObject);
    otherInstance->setObjectName("TestVendor:TestLibrary:TestInstance:1.1");
    otherInstance->setProperty("VLNVType", "Component");  
    
    QTreeWidgetItem* rootItem = buildFrom(rootObject);

    QCOMPARE(rootItem->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestDesign:1.0"));
    QCOMPARE(rootItem->childCount(), 2);

    QTreeWidgetItem* combinedItem = rootItem->child(0);
    QCOMPARE(combinedItem->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestInstance:1.0"));
    QCOMPARE(combinedItem->childCount(), 0);

    QTreeWidgetItem* otherInstanceItem = rootItem->child(1);
    QCOMPARE(otherInstanceItem->text(HierarchicalSaveColumns::VLNV), 
        QString("TestVendor:TestLibrary:TestInstance:1.1"));
    QCOMPARE(otherInstanceItem->childCount(), 0);

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testDesignAndDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testDesignAndDesignConfiguration()
{
    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestComponent:1.0");
    rootObject->setProperty("VLNVType", "Component");

    QObject* designConfigurationObject = new QObject(rootObject);
    designConfigurationObject->setObjectName("TestVendor:TestLibrary:TestConfiguration:1.0");
    designConfigurationObject->setProperty("VLNVType", "DesignConfiguration");

    QObject* designObject = new QObject(designConfigurationObject);
    designObject->setObjectName("TestVendor:TestLibrary:TestDesign:1.0");
    designObject->setProperty("VLNVType", "Design");

    QObject* otherDesign = new QObject(rootObject);
    otherDesign->setObjectName("TestVendor:TestLibrary:TestDesign:2.1");
    otherDesign->setProperty("VLNVType", "Design");

    QTreeWidgetItem* rootItem = buildFrom(rootObject);

    QCOMPARE(rootItem->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestComponent:1.0"));
    QCOMPARE(rootItem->childCount(), 2);

    QTreeWidgetItem* configuredDesign = rootItem->child(0);
    QCOMPARE(configuredDesign->text(HierarchicalSaveColumns::VLNV), QString("TestVendor:TestLibrary:TestDesign:1.0"));
    QCOMPARE(configuredDesign->childCount(), 0);

    QTreeWidgetItem* otherDesignItem = rootItem->child(1);
    QCOMPARE(otherDesignItem->text(HierarchicalSaveColumns::VLNV), 
        QString("TestVendor:TestLibrary:TestDesign:2.1"));
    QCOMPARE(otherDesignItem->childCount(), 0);

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testCheckingIsRecursive()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testCheckingIsRecursive()
{
    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestComponent:1.0");
    rootObject->setProperty("VLNVType", "Component");

    QObject* designObject = new QObject(rootObject);
    designObject->setObjectName("TestVendor:TestLibrary:TestDesign:1.0");
    designObject->setProperty("VLNVType", "Design");

    QObject* otherDesign = new QObject(rootObject);
    otherDesign->setObjectName("TestVendor:TestLibrary:TestDesign:2.1");
    otherDesign->setProperty("VLNVType", "Design");

    QTreeWidgetItem* rootItem = buildFrom(rootObject);
    QCOMPARE(rootItem->childCount(), 2);

    rootItem->setData(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::CheckStateRole, Qt::Unchecked);
    QCOMPARE(rootItem->child(0)->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Unchecked);
    QCOMPARE(rootItem->child(1)->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Unchecked);

    rootItem->setData(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::CheckStateRole, Qt::Checked);
    QCOMPARE(rootItem->child(0)->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Checked);
    QCOMPARE(rootItem->child(1)->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Checked);

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::testItemCannotBeCheckedIfParentNotChecked()
//-----------------------------------------------------------------------------
void tst_HierarchicalSaveBuildStrategy::testItemCannotBeCheckedIfParentNotChecked()
{
    QObject* rootObject = new QObject(0);
    rootObject->setObjectName("TestVendor:TestLibrary:TestComponent:1.0");
    rootObject->setProperty("VLNVType", "Component");

    QObject* designObject = new QObject(rootObject);
    designObject->setObjectName("TestVendor:TestLibrary:TestDesign:1.0");
    designObject->setProperty("VLNVType", "Design");

    QTreeWidgetItem* rootItem = buildFrom(rootObject);

    rootItem->setData(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::CheckStateRole, Qt::Unchecked);

    QTreeWidgetItem* childItem = rootItem->child(0);
    childItem->setData(HierarchicalSaveColumns::SAVE_AS_VLNV, Qt::CheckStateRole, Qt::Checked);

    QCOMPARE(childItem->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV), Qt::Unchecked);

    delete rootObject;
    delete rootItem;
}

//-----------------------------------------------------------------------------
// Function: tst_HierarchicalSaveBuildStrategy::buildFrom()
//-----------------------------------------------------------------------------
QTreeWidgetItem* tst_HierarchicalSaveBuildStrategy::buildFrom(QObject* rootObject)
{
    HierarchicalSaveBuildStrategy strategy(library_);
    return strategy.build(rootObject);
}

QTEST_MAIN(tst_HierarchicalSaveBuildStrategy)

#include "tst_HierarchicalSaveBuildStrategy.moc"
