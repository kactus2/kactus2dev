//-----------------------------------------------------------------------------
// File: tst_DesignConfigurationWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 3.8.2015
//
// Description:
// Unit test for class DesignConfiguration.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QXmlStreamWriter>

#include <IPXACTmodels/DesignConfiguration/DesignConfigurationWriter.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QDomElement>

class tst_DesignConfigurationWriter : public QObject
{
    Q_OBJECT

public:
    tst_DesignConfigurationWriter();

private slots:

    void init();
    void cleanup();

    void testSimpleDesignConfiguration();
    void test2022DesignConfiguration();

    void testTopCommentsAreWritten();
    void testProcessingInstructionsAreWritten();

    void testDesignReference();
    void testGeneratorChainConfiguration();

    void testInterconnectionConfiguration();
    void test2022InterconnectionConfiguration();

    void testViewConfigurations();
    void test2022ViewConfigurations();

    void testParameters();
    void testAssertion();
    void testVendorExtensions();

    void testWriteViewOverrides();

private:

    void compareOutputToExpected(QString const& output, QString const& expectedOutput);

    QSharedPointer<DesignConfiguration> designConfiguration_;
};

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::tst_DesignConfigurationWriter()
//-----------------------------------------------------------------------------
tst_DesignConfigurationWriter::tst_DesignConfigurationWriter():
designConfiguration_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::init()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::init()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::cleanup()
{
    designConfiguration_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testSimpleDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testSimpleDesignConfiguration()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));
    designConfiguration_->setDescription("TestDescription");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    
    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    
    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput = 
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:description>TestDescription</ipxact:description>\n"
        "</ipxact:designConfiguration>\n";

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::test2022DesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::test2022DesignConfiguration()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std22));
    designConfiguration_->setShortDescription("Short");
    designConfiguration_->setDescription("TestDescription");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput =
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:shortDescription>Short</ipxact:shortDescription>\n"
        "\t<ipxact:description>TestDescription</ipxact:description>\n"
        "</ipxact:designConfiguration>\n";

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testTopCommentsAreWritten()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testTopCommentsAreWritten()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));
    designConfiguration_->setTopComments("Comments");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<!--Comments-->\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "</ipxact:designConfiguration>\n");
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testProcessingInstructionsAreWritten()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testProcessingInstructionsAreWritten()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));
    designConfiguration_->addXmlProcessingInstructions("xml-stylesheet", "href=\"style.css\"");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<?xml-stylesheet href=\"style.css\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
        "</ipxact:designConfiguration>\n");
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testDesignReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testDesignReference()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));
    designConfiguration_->setDescription("This is design");

    VLNV designReference(VLNV::DESIGN, "tuni.fi", "TestLibrary", "DesignReference", "1.1");
    designConfiguration_->setDesignRef(designReference);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:designRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"DesignReference\" version=\"1.1\"/>\n"
        "\t<ipxact:description>This is design</ipxact:description>\n"
        "</ipxact:designConfiguration>\n");

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testGeneratorChainConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testGeneratorChainConfiguration()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QSharedPointer<ConfigurableVLNVReference> generatorChainConf (
        new ConfigurableVLNVReference(VLNV::GENERATORCHAIN, "tuni.fi", "TestLibrary", "testGenCon", "1.0"));

    QSharedPointer<ConfigurableElementValue> generatorChainConfigurable (
        new ConfigurableElementValue("10", "testReferenceID"));
    generatorChainConf->getConfigurableElementValues()->append(generatorChainConfigurable);

    designConfiguration_->getGeneratorChainConfs()->append(generatorChainConf);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:generatorChainConfiguration vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testGenCon\" "
            "version=\"1.0\">\n"
            "\t\t<ipxact:configurableElementValues>\n"
                "\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                    "</ipxact:configurableElementValue>\n"
            "\t\t</ipxact:configurableElementValues>\n"
            "\t</ipxact:generatorChainConfiguration>\n"
        "</ipxact:designConfiguration>\n");

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testInterconnectionConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testInterconnectionConfiguration()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QSharedPointer<ConfigurableVLNVReference> testAbstractorRef (
        new ConfigurableVLNVReference(VLNV::ABSTRACTOR, "tuni.fi", "TestLibrary", "testAbstractor", "1.1"));
    QSharedPointer<ConfigurableElementValue> abstractorCEV (new ConfigurableElementValue("12", "testReferenceID"));
    testAbstractorRef->getConfigurableElementValues()->append(abstractorCEV);

    QSharedPointer<AbstractorInstance> testAbstractorInstance (new AbstractorInstance());
    testAbstractorInstance->setInstanceName("testAbstractorInstance");
    testAbstractorInstance->setDisplayName("abstractorDisplay");
    testAbstractorInstance->setDescription("abstractorDescription");
    testAbstractorInstance->setViewName("abstractorView");
    testAbstractorInstance->setAbstractorRef(testAbstractorRef);

    QSharedPointer<InterfaceRef> testInterfaceReference(new InterfaceRef());
    testInterfaceReference->setComponentRef("testComponent");
    testInterfaceReference->setBusRef("testBus");
    testInterfaceReference->setIsPresent("1");

    QSharedPointer<MultipleAbstractorInstances> multipleAbstractors (new MultipleAbstractorInstances());
    multipleAbstractors->setIsPresent("1");
    multipleAbstractors->getInterfaceReferences()->append(testInterfaceReference);
    multipleAbstractors->getAbstractorInstances()->append(testAbstractorInstance);

    QSharedPointer<InterconnectionConfiguration> testInterconnectionConfig (new InterconnectionConfiguration());
    testInterconnectionConfig->setIsPresent("0");
    testInterconnectionConfig->setInterconnectionReference("testInterconnectionReference");
    testInterconnectionConfig->getAbstractorInstances()->append(multipleAbstractors);

    designConfiguration_->getInterconnectionConfs()->append(testInterconnectionConfig);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:interconnectionConfiguration>\n"
            "\t\t<ipxact:isPresent>0</ipxact:isPresent>\n"
            "\t\t<ipxact:interconnectionRef>testInterconnectionReference</ipxact:interconnectionRef>\n"
            "\t\t<ipxact:abstractorInstances>\n"
                "\t\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
                "\t\t\t<ipxact:interfaceRef componentRef=\"testComponent\" busRef=\"testBus\">\n"
                    "\t\t\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
                "\t\t\t</ipxact:interfaceRef>\n"
                "\t\t\t<ipxact:abstractorInstance>\n"
                    "\t\t\t\t<ipxact:instanceName>testAbstractorInstance</ipxact:instanceName>\n"
                    "\t\t\t\t<ipxact:displayName>abstractorDisplay</ipxact:displayName>\n"
                    "\t\t\t\t<ipxact:description>abstractorDescription</ipxact:description>\n"
                    "\t\t\t\t<ipxact:abstractorRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testAbstractor\" "
                        "version=\"1.1\">\n"
                        "\t\t\t\t\t<ipxact:configurableElementValues>\n"
                            "\t\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">12"
                                        "</ipxact:configurableElementValue>\n"
                        "\t\t\t\t\t</ipxact:configurableElementValues>\n"
                    "\t\t\t\t</ipxact:abstractorRef>\n"
                    "\t\t\t\t<ipxact:viewName>abstractorView</ipxact:viewName>\n"
                "\t\t\t</ipxact:abstractorInstance>\n"
            "\t\t</ipxact:abstractorInstances>\n"
        "\t</ipxact:interconnectionConfiguration>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}


//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::test2022InterconnectionConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::test2022InterconnectionConfiguration()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std22));

    QSharedPointer<ConfigurableVLNVReference> testAbstractorRef(
        new ConfigurableVLNVReference(VLNV::ABSTRACTOR, "tuni.fi", "TestLibrary", "testAbstractor", "1.1"));
    QSharedPointer<ConfigurableElementValue> abstractorCEV(new ConfigurableElementValue("12", "testReferenceID"));
    testAbstractorRef->getConfigurableElementValues()->append(abstractorCEV);

    QSharedPointer<AbstractorInstance> testAbstractorInstance(new AbstractorInstance());
    testAbstractorInstance->setInstanceName("testAbstractorInstance");
    testAbstractorInstance->setDisplayName("abstractorDisplay");
    testAbstractorInstance->setShortDescription("brief");
    testAbstractorInstance->setDescription("abstractorDescription");
    testAbstractorInstance->setViewName("abstractorView");
    testAbstractorInstance->setAbstractorRef(testAbstractorRef);
    testAbstractorInstance->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
        new Kactus2Value("abstractorExtension", "On")));

    QSharedPointer<InterfaceRef> testInterfaceReference(new InterfaceRef());
    testInterfaceReference->setComponentRef("testComponent");
    testInterfaceReference->setBusRef("testBus");
    testInterfaceReference->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
        new Kactus2Value("interfaceExtension", "On")));

    QSharedPointer<MultipleAbstractorInstances> multipleAbstractors(new MultipleAbstractorInstances());
    multipleAbstractors->getInterfaceReferences()->append(testInterfaceReference);
    multipleAbstractors->getAbstractorInstances()->append(testAbstractorInstance);

    QSharedPointer<InterconnectionConfiguration> testInterconnectionConfig(new InterconnectionConfiguration());
    testInterconnectionConfig->setInterconnectionReference("testInterconnectionReference");
    testInterconnectionConfig->getAbstractorInstances()->append(multipleAbstractors);
    testInterconnectionConfig->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
        new Kactus2Value("interconnectExtension", "On")));

    designConfiguration_->getInterconnectionConfs()->append(testInterconnectionConfig);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:interconnectionConfiguration>\n"
        "\t\t<ipxact:interconnectionRef>testInterconnectionReference</ipxact:interconnectionRef>\n"
        "\t\t<ipxact:abstractorInstances>\n"
        "\t\t\t<ipxact:interfaceRef componentRef=\"testComponent\" busRef=\"testBus\">\n"
        "\t\t\t\t<ipxact:vendorExtensions>\n"
        "\t\t\t\t\t<interfaceExtension>On</interfaceExtension>\n"
        "\t\t\t\t</ipxact:vendorExtensions>\n"
        "\t\t\t</ipxact:interfaceRef>\n"
        "\t\t\t<ipxact:abstractorInstance>\n"
        "\t\t\t\t<ipxact:instanceName>testAbstractorInstance</ipxact:instanceName>\n"
        "\t\t\t\t<ipxact:displayName>abstractorDisplay</ipxact:displayName>\n"
        "\t\t\t\t<ipxact:shortDescription>brief</ipxact:shortDescription>\n"
        "\t\t\t\t<ipxact:description>abstractorDescription</ipxact:description>\n"
        "\t\t\t\t<ipxact:abstractorRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testAbstractor\" "
        "version=\"1.1\">\n"
        "\t\t\t\t\t<ipxact:configurableElementValues>\n"
        "\t\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">12"
        "</ipxact:configurableElementValue>\n"
        "\t\t\t\t\t</ipxact:configurableElementValues>\n"
        "\t\t\t\t</ipxact:abstractorRef>\n"
        "\t\t\t\t<ipxact:viewName>abstractorView</ipxact:viewName>\n"
        "\t\t\t\t<ipxact:vendorExtensions>\n"
        "\t\t\t\t\t<abstractorExtension>On</abstractorExtension>\n"
        "\t\t\t\t</ipxact:vendorExtensions>\n"
        "\t\t\t</ipxact:abstractorInstance>\n"
        "\t\t</ipxact:abstractorInstances>\n"
        "\t\t<ipxact:vendorExtensions>\n"
        "\t\t\t<interconnectExtension>On</interconnectExtension>\n"
        "\t\t</ipxact:vendorExtensions>\n"
        "\t</ipxact:interconnectionConfiguration>\n"
        "</ipxact:designConfiguration>\n"
    );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testViewConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testViewConfigurations()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QSharedPointer<ViewConfiguration> testViewConfiguration (new ViewConfiguration("testViewConfig"));
    testViewConfiguration->setIsPresent("1");
    testViewConfiguration->setViewReference("referenceToTestView");

    QSharedPointer<ConfigurableElementValue> viewConfigCEV (new ConfigurableElementValue("2", "testReferenceID"));
    testViewConfiguration->getViewConfigurableElements()->append(viewConfigCEV);

    designConfiguration_->getViewConfigurations()->append(testViewConfiguration);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:viewConfiguration>\n"
            "\t\t<ipxact:instanceName>testViewConfig</ipxact:instanceName>\n"
            "\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
            "\t\t<ipxact:view viewRef=\"referenceToTestView\">\n"
                "\t\t\t<ipxact:configurableElementValues>\n"
                    "\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">2"
                        "</ipxact:configurableElementValue>\n"
                "\t\t\t</ipxact:configurableElementValues>\n"
            "\t\t</ipxact:view>\n"
        "\t</ipxact:viewConfiguration>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::test2022ViewConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::test2022ViewConfigurations()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std22));

    QSharedPointer<ViewConfiguration> testViewConfiguration(new ViewConfiguration("testViewConfig"));
    testViewConfiguration->setViewReference("referenceToTestView");
    testViewConfiguration->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
        new Kactus2Value("viewExtension", "Yes")));

    QSharedPointer<ConfigurableElementValue> viewConfigCEV(new ConfigurableElementValue("2", "testReferenceID"));
    testViewConfiguration->getViewConfigurableElements()->append(viewConfigCEV);

    designConfiguration_->getViewConfigurations()->append(testViewConfiguration);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:viewConfiguration>\n"
        "\t\t<ipxact:instanceName>testViewConfig</ipxact:instanceName>\n"
        "\t\t<ipxact:view viewRef=\"referenceToTestView\">\n"
        "\t\t\t<ipxact:configurableElementValues>\n"
        "\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">2</ipxact:configurableElementValue>\n"
        "\t\t\t</ipxact:configurableElementValues>\n"
        "\t\t</ipxact:view>\n"
        "\t\t<ipxact:vendorExtensions>\n"
        "\t\t\t<viewExtension>Yes</viewExtension>\n"
        "\t\t</ipxact:vendorExtensions>\n"
        "\t</ipxact:viewConfiguration>\n"
        "</ipxact:designConfiguration>\n"
    );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testParameters()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testParameters()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    designConfiguration_->getParameters()->append(testParameter);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:parameters>\n"
            "\t\t<ipxact:parameter parameterId=\"testID\">\n"
                "\t\t\t<ipxact:name>testParameter</ipxact:name>\n"
                "\t\t\t<ipxact:value>1</ipxact:value>\n"
            "\t\t</ipxact:parameter>\n"
        "\t</ipxact:parameters>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testAssertion()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testAssertion()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QSharedPointer<Assertion> testAssertion(new Assertion());
    testAssertion->setName("testAssertion");
    testAssertion->setDisplayName("assertionDisplay");
    testAssertion->setDescription("assertionDescription");
    testAssertion->setAssert("13");

    designConfiguration_->getAssertions()->append(testAssertion);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:assertions>\n"
            "\t\t<ipxact:assertion>\n"
                "\t\t\t<ipxact:name>testAssertion</ipxact:name>\n"
                "\t\t\t<ipxact:displayName>assertionDisplay</ipxact:displayName>\n"
                "\t\t\t<ipxact:description>assertionDescription</ipxact:description>\n"
                "\t\t\t<ipxact:assert>13</ipxact:assert>\n"
            "\t\t</ipxact:assertion>\n"
        "\t</ipxact:assertions>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testVendorExtensions()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    designConfiguration_->getVendorExtensions()->append(testExtension);
    designConfiguration_->setVersion("3.0.0");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\""
        ">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:vendorExtensions>\n"
            "\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
            "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
        "\t</ipxact:vendorExtensions>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::testWriteViewOverrides()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::testWriteViewOverrides()
{
    VLNV vlnv(VLNV::DESIGNCONFIGURATION, "tuni.fi", "TestLibrary", "TestDesignConfiguration", "0.1");
    designConfiguration_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(vlnv, Document::Revision::Std14));

    QMap<QString, QString> viewOverrides;
    viewOverrides.insert("instanceId", "testView");
    viewOverrides.insert("otherId", "otherView");

    designConfiguration_->setVersion("3.0.0");
    designConfiguration_->setKactus2ViewOverrides(viewOverrides);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignConfigurationWriter::writeDesignConfiguration(xmlStreamWriter, designConfiguration_);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesignConfiguration</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:viewOverrides>\n"
                    "\t\t\t<kactus2:instanceView id=\"instanceId\" viewName=\"testView\"/>\n"
                    "\t\t\t<kactus2:instanceView id=\"otherId\" viewName=\"otherView\"/>\n"
                "\t\t</kactus2:viewOverrides>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:designConfiguration>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationWriter::compareOutputToExpected()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationWriter::compareOutputToExpected(QString const& output, QString const& expectedOutput)
{
    if (!output.contains(expectedOutput))
    {
        QStringList outputLines = output.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            QCOMPARE(output, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

QTEST_APPLESS_MAIN(tst_DesignConfigurationWriter)

#include "tst_DesignConfigurationWriter.moc"
