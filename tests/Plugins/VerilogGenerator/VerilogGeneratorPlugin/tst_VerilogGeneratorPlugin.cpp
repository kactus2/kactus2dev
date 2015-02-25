//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/veriloggeneratorplugin_global.h>
#include <Plugins/VerilogGenerator/VerilogGeneratorPlugin.h>

#include <Plugins/PluginSystem/PluginUtilityAdapter.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/fileset.h>

#include <tests/MockObjects/LibraryMock.h>

class tst_VerilogGenerator : public VerilogGeneratorPlugin
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testFilesetIsCreatedWhenRunForComponent();
    void testFilesetIsCreatedWhenRunForDesign();
    void testFilesetIsCreatedWhenRunForDesign_data();
   
    void testRTLViewIsCreatedWhenRunForComponent();
   
    void testStructuralViewIsCreatedWhenRunForDesign();

    void testStructuralViewIsCreatedWhenRunForDesign_data();
    void testStructuralViewIsCreatedWhenRunWithDesignConfiguration();
    void testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data();

    void testTopLevelViewIsSetWhenMultipleFlatViews();

    void testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView();
    void testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView();

    void testConsecutiveRunsCreateOnlyOneView();

    void testFlatViewsArePossibleForTopComponent();
    void testRefenecedDesignViewIsPossible();
    void testRefenecedDesignConfigurationViewIsPossible();
    void testDelegationToImplementationViewIsPossible();

protected:

    virtual bool couldConfigure(QStringList const& possibleViewNames) const;

    virtual QSharedPointer<GeneratorConfiguration> getConfiguration();

private:

   QSharedPointer<Component> createTestComponent();

   QSharedPointer<Design> createTestDesign() const;

   QSharedPointer<DesignConfiguration> createTestDesignConfig() const;

   void verifyRTLView(View* rtlView);

   void verifyHierarchicalView(View* hierView, QString const& viewName);    


    //! The test mock for library interface.
    LibraryMock library_;

    PluginUtilityAdapter utilityMock_;

    VerilogGeneratorPlugin plugin_;

    QWidget* activeWindow_;

    QSharedPointer<GeneratorConfiguration> configuration_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
tst_VerilogGenerator::tst_VerilogGenerator(): library_(this), 
    utilityMock_(&library_, 0, this), plugin_(), activeWindow_(0), configuration_(new GeneratorConfiguration())
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    library_.clear();
    configuration_ = QSharedPointer<GeneratorConfiguration>(new GeneratorConfiguration());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    QFile::remove("test.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForComponent()
{
    QSharedPointer<Component> targetComponent = createTestComponent();

    runGenerator(&utilityMock_, targetComponent);

    QVERIFY2(QFile::exists("test.v"), "No file created");
    QVERIFY2(targetComponent->hasFileSet("verilogSource"), "No file set created");
    QVERIFY2(targetComponent->getFileSet("verilogSource")->getFileNames().contains("test.v"), 
        "No file added to fileset");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    View* structuralView = new View(viewName);
    structuralView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<LibraryComponent>(), targetDesign);

    QVERIFY2(QFile::exists("test.v"), "No file created");
    QVERIFY2(targetComponent->hasFileSet(viewName + "_verilogSource"), "No file set created");
    QVERIFY2(targetComponent->getFileSet(viewName + "_verilogSource")->getFileNames().contains("test.v"), 
        "No file added to fileset");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign_data()
{
    QTest::addColumn<QString>("viewName");

    QTest::newRow("structural") << "structural";
    QTest::newRow("hierarchical") << "hierarchical";
    QTest::newRow("design") << "design";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponent()
{
    QSharedPointer<Component> targetComponent = createTestComponent();

    runGenerator(&utilityMock_, targetComponent);
    
    QVERIFY2(targetComponent->hasView("rtl"), "No rtl view created");

    View* rtlView = targetComponent->findView("rtl");
    verifyRTLView(rtlView);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    View* structuralView = new View(viewName);
    structuralView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<LibraryComponent>(0), targetDesign);

    QVERIFY2(targetComponent->hasView(viewName + "_verilog"), "No implementation view created");
    QCOMPARE(structuralView->getTopLevelView(), QString(viewName + "_verilog"));

    View* verilogView = targetComponent->findView(viewName + "_verilog");
    verifyHierarchicalView(verilogView, viewName);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign_data()
{
    testFilesetIsCreatedWhenRunForDesign_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

    targetDesignConfig->setDesignRef(*targetDesign->getVlnv());

    View* structuralView = new View(viewName);
    structuralView->setHierarchyRef(*targetDesignConfig->getVlnv());
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

    QVERIFY2(targetComponent->hasView(viewName + "_verilog"), "No implementation view created");
    QCOMPARE(structuralView->getTopLevelView(), QString(viewName + "_verilog"));

    View* verilogView = targetComponent->findView(viewName + "_verilog");
    verifyHierarchicalView(verilogView, viewName);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data()
{
    testFilesetIsCreatedWhenRunForDesign_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelViewIsSetWhenMultipleFlatViews()
{
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

    targetDesignConfig->setDesignRef(*targetDesign->getVlnv());
    
    targetComponent->createEmptyFlatView();

    View* structuralView = new View("structural");
    structuralView->setHierarchyRef(*targetDesignConfig->getVlnv());
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

    QVERIFY2(targetComponent->hasView("structural_verilog"), "No implementation view created");
    QCOMPARE(structuralView->getTopLevelView(), QString("structural_verilog"));

    View* verilogView = targetComponent->findView("structural_verilog");
    verifyHierarchicalView(verilogView, "structural");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView()
{
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

    View* structuralView = new View("structural");
    structuralView->setHierarchyRef(VLNV(VLNV::DESIGN, "", "", "", ""));
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");

    View* verilogView = targetComponent->findView("rtl");
    verifyRTLView(verilogView);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView()
{    
    QSharedPointer<Component> targetComponent = createTestComponent();

    View* structuralView = new View("structural");
    structuralView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT", "TestLib", "TestDesign", "1.0"));
    targetComponent->addView(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<LibraryComponent>(0), QSharedPointer<LibraryComponent>(0));

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");    

    View* verilogView = targetComponent->findView("rtl");
    verifyRTLView(verilogView);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveRunsCreateOnlyOneView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveRunsCreateOnlyOneView()
{    
    QSharedPointer<Component> targetComponent = createTestComponent();

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<LibraryComponent>(0), QSharedPointer<LibraryComponent>(0));
    runGenerator(&utilityMock_, targetComponent, QSharedPointer<LibraryComponent>(0), QSharedPointer<LibraryComponent>(0));

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");    
    QCOMPARE(targetComponent->viewCount(), 1);    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFlatViewsArePossibleForTopComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFlatViewsArePossibleForTopComponent() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    View* firstView = new View("View1");
    targetComponent->addView(firstView);

    View* secondView = new View("View2");
    targetComponent->addView(secondView);

    View* hierView = new View("hierView");
    hierView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(hierView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, QSharedPointer<LibraryComponent>(0), 
        QSharedPointer<LibraryComponent>(0));

    QCOMPARE(possibleViews.count(), 2);
    QVERIFY(possibleViews.contains("View1"));
    QVERIFY(possibleViews.contains("View2"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRefenecedDesignViewIsPossible()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRefenecedDesignViewIsPossible() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    View* firstView = new View("View1");
    targetComponent->addView(firstView);

    View* secondView = new View("View2");
    targetComponent->addView(secondView);

    View* hierView = new View("hierView");
    hierView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(hierView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, targetDesign, 
        QSharedPointer<LibraryComponent>(0));

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("hierView"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRefenecedDesignConfigurationViewIsPossible()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRefenecedDesignConfigurationViewIsPossible() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();
    QSharedPointer<DesignConfiguration> targetConfiguration = createTestDesignConfig();
    targetConfiguration->setDesignRef(*targetDesign->getVlnv());

    View* flatView = new View("flat");
    targetComponent->addView(flatView);

    View* designView = new View("designView");
    designView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(designView);

    View* designConfigView = new View("designConfigView");
    designConfigView->setHierarchyRef(*targetConfiguration->getVlnv());
    targetComponent->addView(designConfigView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, targetDesign, targetConfiguration);

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("designConfigView"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testDelegationToImplementationViewIsPossible()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testDelegationToImplementationViewIsPossible() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();
    QSharedPointer<DesignConfiguration> targetConfiguration = createTestDesignConfig();
    targetConfiguration->setDesignRef(*targetDesign->getVlnv());

    View* rtlView = new View("rtl");
    targetComponent->addView(rtlView);

    View* designView = new View("designView");
    designView->setHierarchyRef(*targetDesign->getVlnv());
    targetComponent->addView(designView);

    View* designConfigView = new View("designConfigView");
    designConfigView->setHierarchyRef(*targetConfiguration->getVlnv());
    designConfigView->setTopLevelView("rtl");
    targetComponent->addView(designConfigView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, targetDesign, targetConfiguration);

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("rtl"));

    designView->setTopLevelView("rtl");

    possibleViews = findPossibleViewNames(targetComponent, targetDesign, QSharedPointer<LibraryComponent>(0));

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("rtl"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::couldConfigure()
//-----------------------------------------------------------------------------
bool tst_VerilogGenerator::couldConfigure(QStringList const& possibleViewNames) const
{
    configuration_->setActiveView("");
    if (!possibleViewNames.isEmpty())
    {
        configuration_->setActiveView(possibleViewNames.first());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::configure()
//-----------------------------------------------------------------------------
QSharedPointer<GeneratorConfiguration> tst_VerilogGenerator::getConfiguration()
{
    configuration_->setSaveToFileset(true);
    configuration_->setOutputPath("test.v");

    return configuration_;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_VerilogGenerator::createTestComponent()
{
    QSharedPointer<Component> component(new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "test","1.0")));
    library_.writeModelToFile(".", component);
    return component;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_VerilogGenerator::createTestDesign() const
{
    return QSharedPointer<Design>(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "TestDesign", "1.0")));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestDesignConfig()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> tst_VerilogGenerator::createTestDesignConfig() const
{
    return QSharedPointer<DesignConfiguration>(new DesignConfiguration(VLNV(VLNV::DESIGNCONFIGURATION, 
        "TUT", "TestLibrary", "TestDesignConf", "1.0")));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyRTLView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyRTLView(View* rtlView)
{
    QCOMPARE(rtlView->getLanguage(), QString("verilog"));
    QCOMPARE(rtlView->getModelName(), QString("test"));
    QCOMPARE(rtlView->getFileSetRefs().first(), QString("verilogSource"));
    QCOMPARE(rtlView->getEnvIdentifiers().first(), QString("verilog:Kactus2:"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyHierarchicalView(View* hierView, QString const& viewName)
{
    QCOMPARE(hierView->getLanguage(), QString("verilog"));
    QCOMPARE(hierView->getModelName(), QString("test"));
    QCOMPARE(hierView->getFileSetRefs().first(), QString(viewName + "_verilogSource"));
    QCOMPARE(hierView->getEnvIdentifiers().first(), QString("verilog:Kactus2:"));
}

QTEST_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGeneratorPlugin.moc"
