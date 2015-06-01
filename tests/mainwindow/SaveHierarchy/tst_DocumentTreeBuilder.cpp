//-----------------------------------------------------------------------------
// File: tst_DocumentTreeBuilder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.05.2015
//
// Description:
// Unit test for class DocumentTreeBuilder.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <mainwindow/SaveHierarchy/DocumentTreeBuilder.h>

#include <tests/MockObjects/LibraryMock.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/vlnv.h>

class tst_DocumentTreeBuilder : public QObject
{
    Q_OBJECT

public:
    tst_DocumentTreeBuilder();
    ~tst_DocumentTreeBuilder();
private slots:
    void cleanup();
    
    void testUnknownReferenceResultsInEmptyNode();
    void testReferenceResultsInListWithReference();

    void testHierarchicalReferenceToDesign();

    void testHierarchicalReferenceToDesignConfiguration();

    void testHierarchicalReferenceToInstanceInDesign();
    void testReferenceToInstanceInDesignWithoutDesignConfiguration();

    void testDesignInInstantiatedComponent();
    
    void testMultipleInstances();
    
    void testStartFromDesign();
    void testStartFromDesignConfiguration();

private:
    
    QSharedPointer<Component> createTopComponent();
    
    VLNV topComponentVLNV();

    QSharedPointer<Design> createDesign();

    VLNV topDesignVLNV();

    QSharedPointer<DesignConfiguration> createDesignConfiguration();
    VLNV topDesignConfigurationVLNV();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    LibraryMock* library_;

    DocumentTreeBuilder* builder_;
};

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::tst_DocumentTreeBuilder()
//-----------------------------------------------------------------------------
tst_DocumentTreeBuilder::tst_DocumentTreeBuilder(): library_(new LibraryMock(this)), 
    builder_(new DocumentTreeBuilder(library_))
{

}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::~tst_DocumentTreeBuilder()
//-----------------------------------------------------------------------------
tst_DocumentTreeBuilder::~tst_DocumentTreeBuilder()
{
    delete builder_;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::cleanup()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::cleanup()
{
    library_->clear();
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testUnknownReferenceResultsInEmptyNode()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testUnknownReferenceResultsInEmptyNode()
{
    VLNV nonExistingVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestComponent", "TestVersion");

    QObject* rootObject = builder_->createFrom(nonExistingVLNV);

    QCOMPARE(rootObject->objectName(), nonExistingVLNV.toString());
    QCOMPARE(rootObject->children().count(), 0);
    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testReferenceResultsInListWithReference()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testReferenceResultsInListWithReference()
{
    createTopComponent();

    QObject* rootObject = builder_->createFrom(topComponentVLNV());

    QCOMPARE(rootObject->objectName(), topComponentVLNV().toString());
    QCOMPARE(rootObject->children().count(), 0);
    QCOMPARE(rootObject->property("VLNVType").toString(), QString("Component"));

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testHierarchicalReferenceToDesign()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testHierarchicalReferenceToDesign()
{
    QSharedPointer<Component> component = createTopComponent();
    createDesign();

    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignVLNV());


    QObject* rootObject = builder_->createFrom(topComponentVLNV());

    QCOMPARE(rootObject->objectName(), topComponentVLNV().toString());
    QCOMPARE(rootObject->children().count(), 1);
    QCOMPARE(rootObject->property("VLNVType").toString(), QString("Component"));

    QObject* designObject = rootObject->children().first();
    QCOMPARE(designObject->objectName(), topDesignVLNV().toString());
    QCOMPARE(designObject->property("VLNVType").toString(), QString("Design"));

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testHierarchicalReferenceToDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testHierarchicalReferenceToDesignConfiguration()
{
    QSharedPointer<Component> component = createTopComponent();

    createDesign();
    createDesignConfiguration();

    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignConfigurationVLNV());

    QObject* rootObject = builder_->createFrom(topComponentVLNV());

    QCOMPARE(rootObject->objectName(), topComponentVLNV().toString());
    QCOMPARE(rootObject->children().count(), 1);
    QCOMPARE(rootObject->property("VLNVType").toString(), QString("Component"));

    QObject* designConfigurationObject = rootObject->children().first();
    QCOMPARE(designConfigurationObject->objectName(), topDesignConfigurationVLNV().toString());
    QCOMPARE(designConfigurationObject->children().count(), 1);
    QCOMPARE(designConfigurationObject->property("VLNVType").toString(), QString("DesignConfiguration"));

    QObject* designObject = designConfigurationObject->children().first();
    QCOMPARE(designObject->objectName(), topDesignVLNV().toString());
    QCOMPARE(designObject->children().count(), 0);
    QCOMPARE(designObject->property("VLNVType").toString(), QString("Design"));

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testHierarchicalReferenceToInstanceInDesign()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testHierarchicalReferenceToInstanceInDesign()
{
    QSharedPointer<Design> design = createDesign();
    
    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    QList<ComponentInstance> instances;
    instances.append(instance);
    design->setComponentInstances(instances);
    
    QSharedPointer<Component> component = createTopComponent();

    createDesignConfiguration();

    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignConfigurationVLNV());

    QObject* rootObject = builder_->createFrom(topComponentVLNV());
    QCOMPARE(rootObject->children().count(), 1);

    QObject* designConfigurationObject = rootObject->children().first();
    QCOMPARE(designConfigurationObject->children().count(), 1);

    QObject* designObject = designConfigurationObject->children().first();
    QCOMPARE(designObject->children().count(), 1);

    QObject* instanceObject = designObject->children().first();
    QCOMPARE(instanceObject->children().count(), 0);
    QCOMPARE(instanceObject->objectName(), instanceVLNV.toString());
    QCOMPARE(instanceObject->property("instanceName").toString(), QString("instance1"));

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testReferenceToInstanceInDesignWithoutDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testReferenceToInstanceInDesignWithoutDesignConfiguration()
{
    QSharedPointer<Component> component = createTopComponent();
    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignVLNV());

    QSharedPointer<Design> design = createDesign();

    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    QList<ComponentInstance> instances;
    instances.append(instance);
    design->setComponentInstances(instances);

    QObject* rootObject = builder_->createFrom(topComponentVLNV());
    QCOMPARE(rootObject->children().count(), 1);

    QObject* designObject = rootObject->children().first();
    QCOMPARE(designObject->children().count(), 1);

    QObject* instanceObject = designObject->children().first();
    QCOMPARE(instanceObject->children().count(), 0);
    QCOMPARE(instanceObject->objectName(), instanceVLNV.toString());
    QCOMPARE(instanceObject->property("instanceName").toString(), QString("instance1"));
    QCOMPARE(instanceObject->property("activeView").toString(), QString(""));

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testDesignInInstantiatedComponent()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testDesignInInstantiatedComponent()
{
    QSharedPointer<Component> component = createTopComponent();

    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignVLNV());

    QSharedPointer<Design> design = createDesign();

    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    QList<ComponentInstance> instances;
    instances.append(instance);
    design->setComponentInstances(instances);

    VLNV instanceDesignVLNV(VLNV::DESIGN, "TestVendor", "TestLibrary", "BottomDesign", "TestVersion");
    QSharedPointer<Design> instanceDesign(new Design(instanceDesignVLNV));
    library_->addComponent(instanceDesign);

    VLNV bottomInstanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestBottomInstance", "TestVersion");
    QSharedPointer<Component> bottomComponent(new Component(bottomInstanceVLNV));
    library_->addComponent(bottomComponent);

    ComponentInstance bottomInstance("bottomInstance", "", "", bottomInstanceVLNV, QPointF(), "");

    QList<ComponentInstance> bottomInstances;
    bottomInstances.append(bottomInstance);
    instanceDesign->setComponentInstances(bottomInstances);

    View* bottomView = instanceComponent->createView();
    bottomView->setHierarchyRef(instanceDesignVLNV);

    QObject* rootObject = builder_->createFrom(topComponentVLNV());

    QCOMPARE(rootObject->children().count(), 1);

    QObject* designObject = rootObject->children().first();
    QCOMPARE(designObject->children().count(), 1);

    QObject* instanceObject = designObject->children().first();
    QCOMPARE(instanceObject->children().count(), 1);

    QObject* instanceDesignObject = instanceObject->children().first();
    QCOMPARE(instanceDesignObject->objectName(), instanceDesignVLNV.toString());
    QCOMPARE(instanceDesignObject->children().count(), 1);

    QObject* bottomObject = instanceDesignObject->children().first();
    QCOMPARE(bottomObject->children().count(), 0);
    QCOMPARE(bottomObject->objectName(), bottomInstanceVLNV.toString());

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testMultipleInstances()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testMultipleInstances()
{
    QSharedPointer<Component> component = createTopComponent();

    View* hierarchicalView = component->createView();
    hierarchicalView->setHierarchyRef(topDesignVLNV());

    QSharedPointer<Design> design = createDesign();

    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    QList<ComponentInstance> instances;
    const int INSTANCE_COUNT = 3;
    for (int i = 0; i < INSTANCE_COUNT; i++)
    {
        ComponentInstance instance("instance" + QString::number(i), "", "", instanceVLNV, QPointF(), "");
        instances.append(instance);
    }
    design->setComponentInstances(instances);

    QObject* rootObject = builder_->createFrom(topComponentVLNV());

    QCOMPARE(rootObject->children().count(), 1);

    QObject* designObject = rootObject->children().first();
    QCOMPARE(designObject->children().count(), INSTANCE_COUNT);

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testStartFromDesign()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testStartFromDesign()
{
    QSharedPointer<Design> design = createDesign();

    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    QList<ComponentInstance> instances;
    instances.append(instance);
    design->setComponentInstances(instances);

    QObject* rootObject = builder_->createFrom(topDesignVLNV());
    QCOMPARE(rootObject->children().count(), 1);
    QCOMPARE(rootObject->objectName(), topDesignVLNV().toString());

    QObject* instanceObject = rootObject->children().first();
    QCOMPARE(instanceObject->children().count(), 0);
    QCOMPARE(instanceObject->objectName(), instanceVLNV.toString());

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::testStartFromDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DocumentTreeBuilder::testStartFromDesignConfiguration()
{
    QSharedPointer<Design> design = createDesign();

    VLNV instanceVLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestInstance", "TestVersion");
    QSharedPointer<Component> instanceComponent(new Component(instanceVLNV));
    library_->addComponent(instanceComponent);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    QList<ComponentInstance> instances;
    instances.append(instance);
    design->setComponentInstances(instances);

    createDesignConfiguration();

    QObject* rootObject = builder_->createFrom(topDesignConfigurationVLNV());
    QCOMPARE(rootObject->children().count(), 1);
    QCOMPARE(rootObject->objectName(), topDesignConfigurationVLNV().toString());

    QObject* designObject = rootObject->children().first();
    QCOMPARE(designObject->children().count(), 1);
    QCOMPARE(designObject->objectName(), topDesignVLNV().toString());

    QObject* instanceObject = designObject->children().first();
    QCOMPARE(instanceObject->children().count(), 0);
    QCOMPARE(instanceObject->objectName(), instanceVLNV.toString());

    delete rootObject;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::createTopComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_DocumentTreeBuilder::createTopComponent()
{
    QSharedPointer<Component> component(new Component(topComponentVLNV()));
    library_->addComponent(component);

    return component;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::topComponentVLNV()
//-----------------------------------------------------------------------------
VLNV tst_DocumentTreeBuilder::topComponentVLNV()
{
    return VLNV(VLNV::COMPONENT, "TestVendor", "TestLibrary", "TestComponent", "TestVersion");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_DocumentTreeBuilder::createDesign()
{
    QSharedPointer<Design> design(new Design(topDesignVLNV()));
    library_->addComponent(design);

    return design;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::topDesignVLNV()
//-----------------------------------------------------------------------------
VLNV tst_DocumentTreeBuilder::topDesignVLNV()
{
    return VLNV(VLNV::DESIGN, "TestVendor", "TestLibrary", "TestDesign", "TestVersion");
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::createDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> tst_DocumentTreeBuilder::createDesignConfiguration()
{
    QSharedPointer<DesignConfiguration> designConfiguration(new DesignConfiguration(topDesignConfigurationVLNV()));
    designConfiguration->setDesignRef(topDesignVLNV());
    library_->addComponent(designConfiguration);

    return designConfiguration;
}

//-----------------------------------------------------------------------------
// Function: tst_DocumentTreeBuilder::topDesignConfigurationVLNV()
//-----------------------------------------------------------------------------
VLNV tst_DocumentTreeBuilder::topDesignConfigurationVLNV()
{
    return VLNV(VLNV::DESIGNCONFIGURATION, "TestVendor", "TestLibrary", "TestDesignConfig", "TestVersion");
}

QTEST_APPLESS_MAIN(tst_DocumentTreeBuilder)

#include "tst_DocumentTreeBuilder.moc"
