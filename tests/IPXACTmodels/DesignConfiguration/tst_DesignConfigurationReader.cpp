//-----------------------------------------------------------------------------
// File: tst_DesignConfigurationReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.08.2015
//
// Description:
// Unit test for class DesignConfigurationReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/designConfiguration/DesignConfigurationReader.h>

#include <QtTest>
#include <QDomNode>

class tst_DesignConfigurationReader : public QObject
{
    Q_OBJECT

public:
    tst_DesignConfigurationReader();

private slots:

    void testReadSimpleDesignConfiguration();
    void testRead2022DesignConfiguration();

    void testReadTopComments();
    void testProcessingInstructionsAreParsed();

    void testReadDesignReference();
    void testReadGeneratorChainConfigurations();

    void testReadInterconnectionConfigurations();
    void testRead2022InterconnectionConfigurations();

    void testReadViewConfigurations();
    void testRead2022ViewConfigurations();
    
    void testReadParameters();
    void testReadAssertions();
    void testReadVendorExtensions();

    void testReadConfigurableElementValues();
    void testReadViewOverrides();
};

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::tst_DesignConfigurationReader()
//-----------------------------------------------------------------------------
tst_DesignConfigurationReader::tst_DesignConfigurationReader()
{
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::readSimpleDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadSimpleDesignConfiguration()
{
    QString documentContent(
        "<ipxact:designConfiguration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
        "</ipxact:designConfiguration>");

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getVlnv().getVendor(), QString("tuni.fi"));
    QCOMPARE(testDesignConfiguration->getVlnv().getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesignConfiguration->getVlnv().getName(), QString("TestDesignConfiguration"));
    QCOMPARE(testDesignConfiguration->getVlnv().getVersion(), QString("0.1"));

    QCOMPARE(testDesignConfiguration->getDescription(), QString("TestDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::readTopComments()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testRead2022DesignConfiguration()
{
    QString documentContent(
        "<ipxact:designConfiguration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>1.2022</ipxact:version>"
            "<ipxact:shortDescription>Short</ipxact:shortDescription>"
            "<ipxact:description>Full description</ipxact:description>"
        "</ipxact:designConfiguration>");

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std22);
    QCOMPARE(testDesignConfiguration->getVlnv().getVendor(), QString("tuni.fi"));
    QCOMPARE(testDesignConfiguration->getVlnv().getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesignConfiguration->getVlnv().getName(), QString("TestDesignConfiguration"));
    QCOMPARE(testDesignConfiguration->getVlnv().getVersion(), QString("1.2022"));

    QCOMPARE(testDesignConfiguration->getShortDescription(), QString("Short"));
    QCOMPARE(testDesignConfiguration->getDescription(), QString("Full description"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::readTopComments()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadTopComments()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<!--testComment 1-->"
        "<!--testComment 2-->"
        "<!--testComment 3-->"
        "<ipxact:designConfiguration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
        "</ipxact:designConfiguration>");

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getTopComments().size(), 3);
    QCOMPARE(testDesignConfiguration->getTopComments().first(), QString("testComment 1"));
    QCOMPARE(testDesignConfiguration->getTopComments().last(), QString("testComment 3"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testProcessingInstructionsAreParsed()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testProcessingInstructionsAreParsed()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<ipxact:designConfiguration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
        "</ipxact:designConfiguration>");

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getXmlProcessingInstructions().count(), 1);

    QPair<QString, QString> styleInstruction = testDesignConfiguration->getXmlProcessingInstructions().first();
    QCOMPARE(styleInstruction.first, QString("xml-stylesheet"));
    QCOMPARE(styleInstruction.second, QString("href=\"style.css\""));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadDesignReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadDesignReference()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:designRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"DesignReference\" version=\"1.1\"/>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getDesignRef().getVendor(), QString("tuni.fi"));
    QCOMPARE(testDesignConfiguration->getDesignRef().getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesignConfiguration->getDesignRef().getName(), QString("DesignReference"));
    QCOMPARE(testDesignConfiguration->getDesignRef().getVersion(), QString("1.1"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadGeneratorChainConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadGeneratorChainConfigurations()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:generatorChainConfiguration vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testGenCon\" "
                "version=\"1.0\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"testReferenceID\">12"
                    "</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:generatorChainConfiguration>"
            "<ipxact:generatorChainConfiguration vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testGenCon2\" "
                "version=\"1.5\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"testReferenceID2\">20"
                    "</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:generatorChainConfiguration>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->size(), 2);
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->first()->getVendor(), QString("tuni.fi"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->first()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->first()->getName(), QString("testGenCon"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->first()->getVersion(), QString("1.0"));

    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->first()->getConfigurableElementValues()->size(), 1);

    QSharedPointer<ConfigurableElementValue> testConfElement = testDesignConfiguration->getGeneratorChainConfs()->
        first()->getConfigurableElementValues()->first();

    QCOMPARE(testConfElement->getReferenceId(), QString("testReferenceID"));
    QCOMPARE(testConfElement->getConfigurableValue(), QString("12"));

    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->last()->getVendor(), QString("tuni.fi"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->last()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->last()->getName(), QString("testGenCon2"));
    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->last()->getVersion(), QString("1.5"));

    QCOMPARE(testDesignConfiguration->getGeneratorChainConfs()->last()->getConfigurableElementValues()->size(), 1);

    QSharedPointer<ConfigurableElementValue> otherConfElement = testDesignConfiguration->getGeneratorChainConfs()->
        last()->getConfigurableElementValues()->first();

    QCOMPARE(otherConfElement->getReferenceId(), QString("testReferenceID2"));
    QCOMPARE(otherConfElement->getConfigurableValue(), QString("20"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadInterconnectionConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadInterconnectionConfigurations()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:interconnectionConfiguration>"
                "<ipxact:isPresent>0</ipxact:isPresent>"
                "<ipxact:interconnectionRef>testInterconnectionReference</ipxact:interconnectionRef>"
                "<ipxact:abstractorInstances>"
                    "<ipxact:isPresent>1</ipxact:isPresent>"
                    "<ipxact:interfaceRef componentRef=\"testComponent\" busRef=\"testBus\">"
                        "<ipxact:isPresent>1</ipxact:isPresent>"
                    "</ipxact:interfaceRef>"
                    "<ipxact:abstractorInstance>"
                        "<ipxact:instanceName>testAbstractorInstance</ipxact:instanceName>"
                        "<ipxact:displayName>abstractorDisplay</ipxact:displayName>"
                        "<ipxact:description>abstractorDescription</ipxact:description>"
                        "<ipxact:abstractorRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testAbstractor\" "
                        "version=\"1.1\">"
                            "<ipxact:configurableElementValues>"
                                "<ipxact:configurableElementValue referenceId=\"testReferenceID\">12"
                                "</ipxact:configurableElementValue>"
                            "</ipxact:configurableElementValues>"
                        "</ipxact:abstractorRef>"
                        "<ipxact:viewName>abstractorView</ipxact:viewName>"
                    "</ipxact:abstractorInstance>"
                "</ipxact:abstractorInstances>"
            "</ipxact:interconnectionConfiguration>"
        "</ipxact:designConfiguration>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getInterconnectionConfs()->size(), 1);

    QSharedPointer<InterconnectionConfiguration> testInterconnectionConf =
        testDesignConfiguration->getInterconnectionConfs()->first();

    QCOMPARE(testInterconnectionConf->getIsPresent(), QString("0"));
    QCOMPARE(testInterconnectionConf->getInterconnectionReference(), QString("testInterconnectionReference"));
    QCOMPARE(testInterconnectionConf->getAbstractorInstances()->size(), 1);

    QSharedPointer<MultipleAbstractorInstances> testMultipleAbsInstance = 
        testInterconnectionConf->getAbstractorInstances()->first();
    QCOMPARE(testMultipleAbsInstance->getIsPresent(), QString("1"));
    QCOMPARE(testMultipleAbsInstance->getInterfaceReferences()->size(), 1);
    QCOMPARE(testMultipleAbsInstance->getAbstractorInstances()->size(), 1);

    QSharedPointer<InterfaceRef> testInterfaceRef = testMultipleAbsInstance->getInterfaceReferences()->first();
    QCOMPARE(testInterfaceRef->getComponentRef(), QString("testComponent"));
    QCOMPARE(testInterfaceRef->getBusRef(), QString("testBus"));
    QCOMPARE(testInterfaceRef->getIsPresent(), QString("1"));

    QSharedPointer<AbstractorInstance> testAbstractorInstance =
        testMultipleAbsInstance->getAbstractorInstances()->first();
    QCOMPARE(testAbstractorInstance->getInstanceName(), QString("testAbstractorInstance"));
    QCOMPARE(testAbstractorInstance->getDisplayName(), QString("abstractorDisplay"));
    QCOMPARE(testAbstractorInstance->getDescription(), QString("abstractorDescription"));
    QCOMPARE(testAbstractorInstance->getViewName(), QString("abstractorView"));

    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getVendor(), QString("tuni.fi"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getName(), QString("testAbstractor"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getVersion(), QString("1.1"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getConfigurableElementValues()->size(), 1);

    QSharedPointer<ConfigurableElementValue> abstractorRefCEV =
        testAbstractorInstance->getAbstractorRef()->getConfigurableElementValues()->first();

    QCOMPARE(abstractorRefCEV->getReferenceId(), QString("testReferenceID"));
    QCOMPARE(abstractorRefCEV->getConfigurableValue(), QString("12"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testRead2022InterconnectionConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testRead2022InterconnectionConfigurations()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
                "<ipxact:interconnectionConfiguration>"
                    "<ipxact:interconnectionRef>testInterconnectionReference</ipxact:interconnectionRef>"
                    "<ipxact:abstractorInstances>"
                    "<ipxact:interfaceRef componentRef=\"testComponent\" busRef=\"testBus\">"
                        "<ipxact:vendorExtensions>"
                            "<interfaceExtension testAttribute=\"extension\">testValue</interfaceExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:interfaceRef>"
                    "<ipxact:abstractorInstance>"
                        "<ipxact:instanceName>testAbstractorInstance</ipxact:instanceName>"
                        "<ipxact:displayName>abstractorDisplay</ipxact:displayName>"
                        "<ipxact:shortDescription>short</ipxact:shortDescription>"
                        "<ipxact:description>abstractorDescription</ipxact:description>"
                        "<ipxact:abstractorRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testAbstractor\" version=\"1.1\">"
                            "<ipxact:configurableElementValues>"
                                "<ipxact:configurableElementValue referenceId=\"testReferenceID\">12"
                                "</ipxact:configurableElementValue>"
                            "</ipxact:configurableElementValues>"
                        "</ipxact:abstractorRef>"
                        "<ipxact:viewName>abstractorView</ipxact:viewName>"
                        "<ipxact:vendorExtensions>"
                            "<abstractorExtension testAttribute=\"extension\">testValue</abstractorExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:abstractorInstance>"
                "</ipxact:abstractorInstances>"
                "<ipxact:vendorExtensions>"
                    "<interconnectExtension testAttribute=\"extension\">testValue</interconnectExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:interconnectionConfiguration>"
        "</ipxact:designConfiguration>\n"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std22);
    QCOMPARE(testDesignConfiguration->getInterconnectionConfs()->size(), 1);

    QSharedPointer<InterconnectionConfiguration> testInterconnectionConf =
        testDesignConfiguration->getInterconnectionConfs()->first();

    QCOMPARE(testInterconnectionConf->getIsPresent(), QString());
    QCOMPARE(testInterconnectionConf->getInterconnectionReference(), QString("testInterconnectionReference"));
    QCOMPARE(testInterconnectionConf->getAbstractorInstances()->size(), 1);
    QCOMPARE(testInterconnectionConf->getVendorExtensions()->count(), 1);
    QCOMPARE(testInterconnectionConf->getVendorExtensions()->first()->type(), QString("interconnectExtension"));

    QSharedPointer<MultipleAbstractorInstances> testMultipleAbsInstance =
        testInterconnectionConf->getAbstractorInstances()->first();
    QCOMPARE(testMultipleAbsInstance->getIsPresent(), QString());
    QCOMPARE(testMultipleAbsInstance->getInterfaceReferences()->size(), 1);
    QCOMPARE(testMultipleAbsInstance->getAbstractorInstances()->size(), 1);

    QSharedPointer<InterfaceRef> testInterfaceRef = testMultipleAbsInstance->getInterfaceReferences()->first();
    QCOMPARE(testInterfaceRef->getComponentRef(), QString("testComponent"));
    QCOMPARE(testInterfaceRef->getBusRef(), QString("testBus"));
    QCOMPARE(testInterfaceRef->getIsPresent(), QString());
    QCOMPARE(testInterfaceRef->getVendorExtensions()->count(), 1);
    QCOMPARE(testInterfaceRef->getVendorExtensions()->first()->type(), QString("interfaceExtension"));

    QSharedPointer<AbstractorInstance> testAbstractorInstance =
        testMultipleAbsInstance->getAbstractorInstances()->first();
    QCOMPARE(testAbstractorInstance->getInstanceName(), QString("testAbstractorInstance"));
    QCOMPARE(testAbstractorInstance->getDisplayName(), QString("abstractorDisplay"));
    QCOMPARE(testAbstractorInstance->getShortDescription(), QString("short"));
    QCOMPARE(testAbstractorInstance->getDescription(), QString("abstractorDescription"));
    QCOMPARE(testAbstractorInstance->getViewName(), QString("abstractorView"));
    QCOMPARE(testAbstractorInstance->getVendorExtensions()->count(), 1);
    QCOMPARE(testAbstractorInstance->getVendorExtensions()->first()->type(), QString("abstractorExtension"));

    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getVendor(), QString("tuni.fi"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getName(), QString("testAbstractor"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getVersion(), QString("1.1"));
    QCOMPARE(testAbstractorInstance->getAbstractorRef()->getConfigurableElementValues()->size(), 1);

    QSharedPointer<ConfigurableElementValue> abstractorRefCEV =
        testAbstractorInstance->getAbstractorRef()->getConfigurableElementValues()->first();

    QCOMPARE(abstractorRefCEV->getReferenceId(), QString("testReferenceID"));
    QCOMPARE(abstractorRefCEV->getConfigurableValue(), QString("12"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadViewConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadViewConfigurations()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:viewConfiguration>"
                "<ipxact:instanceName>testViewConfig</ipxact:instanceName>"
                "<ipxact:isPresent>1</ipxact:isPresent>"
                "<ipxact:view viewRef=\"referenceToTestView\">"
                    "<ipxact:configurableElementValues>"
                        "<ipxact:configurableElementValue referenceId=\"testReferenceID\">2"
                        "</ipxact:configurableElementValue>"
                    "</ipxact:configurableElementValues>"
                "</ipxact:view>"
            "</ipxact:viewConfiguration>"
            "<ipxact:description>Design description</ipxact:description>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getDescription(), QString("Design description"));
    QCOMPARE(testDesignConfiguration->getViewConfigurations()->size(), 1);

    QSharedPointer<ViewConfiguration> testViewConfiguration =
        testDesignConfiguration->getViewConfigurations()->first();

    QCOMPARE(testViewConfiguration->getInstanceName(), QString("testViewConfig"));
    QCOMPARE(testViewConfiguration->getIsPresent(), QString("1"));
    QCOMPARE(testViewConfiguration->getViewReference(), QString("referenceToTestView"));
    QCOMPARE(testViewConfiguration->getViewConfigurableElements()->size(), 1);

    QSharedPointer<ConfigurableElementValue> testConfigurableElementValue =
        testViewConfiguration->getViewConfigurableElements()->first();

    QCOMPARE(testConfigurableElementValue->getConfigurableValue(), QString("2"));
    QCOMPARE(testConfigurableElementValue->getReferenceId(), QString("testReferenceID"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testRead2022ViewConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testRead2022ViewConfigurations()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:viewConfiguration>"
                "<ipxact:instanceName>testViewConfig</ipxact:instanceName>"
                "<ipxact:view viewRef=\"referenceToTestView\">"
                    "<ipxact:configurableElementValues>"
                        "<ipxact:configurableElementValue referenceId=\"testReferenceID\">2"
                    "</ipxact:configurableElementValue>"
                    "</ipxact:configurableElementValues>"
                "</ipxact:view>"
                "<ipxact:vendorExtensions>"
                    "<viewExtension testAttribute=\"extension\">testValue</viewExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:viewConfiguration>"
        "</ipxact:designConfiguration>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std22);
    QCOMPARE(testDesignConfiguration->getViewConfigurations()->size(), 1);

    QSharedPointer<ViewConfiguration> testViewConfiguration =
        testDesignConfiguration->getViewConfigurations()->first();

    QCOMPARE(testViewConfiguration->getInstanceName(), QString("testViewConfig"));
    QCOMPARE(testViewConfiguration->getIsPresent(), QString());
    QCOMPARE(testViewConfiguration->getViewReference(), QString("referenceToTestView"));
    QCOMPARE(testViewConfiguration->getViewConfigurableElements()->size(), 1);
    QCOMPARE(testViewConfiguration->getVendorExtensions()->count(), 1);
    QCOMPARE(testViewConfiguration->getVendorExtensions()->first()->type(), QString("viewExtension"));

    QSharedPointer<ConfigurableElementValue> testConfigurableElementValue =
        testViewConfiguration->getViewConfigurableElements()->first();

    QCOMPARE(testConfigurableElementValue->getConfigurableValue(), QString("2"));
    QCOMPARE(testConfigurableElementValue->getReferenceId(), QString("testReferenceID"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::readParameters()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadParameters()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:designConfiguration>");

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getParameters()->size(), 1);
    QCOMPARE(testDesignConfiguration->getParameters()->first()->getValue(), QString("1"));
    QCOMPARE(testDesignConfiguration->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testDesignConfiguration->getParameters()->first()->name(), QString("testParameter"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadAssertions()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadAssertions()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion</ipxact:name>"
                    "<ipxact:displayName>assertionDisplay</ipxact:displayName>"
                    "<ipxact:description>assertionDescription</ipxact:description>"
                    "<ipxact:assert>13</ipxact:assert>"
                "</ipxact:assertion>"
            "</ipxact:assertions>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getAssertions()->size(), 1);
    QCOMPARE(testDesignConfiguration->getAssertions()->first()->name(), QString("testAssertion"));
    QCOMPARE(testDesignConfiguration->getAssertions()->first()->displayName(), QString("assertionDisplay"));
    QCOMPARE(testDesignConfiguration->getAssertions()->first()->description(), QString("assertionDescription"));
    QCOMPARE(testDesignConfiguration->getAssertions()->first()->getAssert(), QString("13"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadVendorExtensions()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:vendorExtensions>"                
                "<kactus2:version>3.0.0</kactus2:version>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesignConfiguration->getVendorExtensions()->last()->type(), QString("testExtension"));
    QCOMPARE(testDesignConfiguration->getVersion(), QString("3.0.0"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadConfigurableElementValues()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadConfigurableElementValues()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:vendorExtensions>"
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:configurableElementValues>"
                    "<kactus2:componentInstance>"
                        "<kactus2:uuid>testInstance</kactus2:uuid>"
                        "<kactus2:configurableElementValue referenceId=\"otherID\" value=\"4-4-4\"/>"
                        "<kactus2:configurableElementValue referenceId=\"referencedID\""
                            " value=\"referencedValue\"/>"
                    "</kactus2:componentInstance>"
                "</kactus2:configurableElementValues>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesignConfiguration->getConfigurableElementValues("testInstance").size(), 2);

    QMap<QString, QString> elements = testDesignConfiguration->getConfigurableElementValues("testInstance");
    QCOMPARE(elements.count(), 2);
    QCOMPARE(elements.firstKey(), QString("otherID"));
    QCOMPARE(elements.first(), QString("4-4-4"));
    QCOMPARE(elements.lastKey(), QString("referencedID"));
    QCOMPARE(elements.last(), QString("referencedValue"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationReader::testReadViewOverrides()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationReader::testReadViewOverrides()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:designConfiguration "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>tuni.fi</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesignConfiguration</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:vendorExtensions>"
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:viewOverrides>"
                    "<kactus2:instanceView id=\"instanceId\" viewName=\"testView\"/>"
                    "<kactus2:instanceView id=\"otherId\" viewName=\"otherView\"/>"
                "</kactus2:viewOverrides>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designConfiguration>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QSharedPointer<DesignConfiguration> testDesignConfiguration =
        DesignConfigurationReader::createDesignConfigurationFrom(document);

    QCOMPARE(testDesignConfiguration->getRevision(), Document::Revision::Std14);
    QCOMPARE(testDesignConfiguration->getVendorExtensions()->size(), 2);

    QCOMPARE(testDesignConfiguration->getKactus2ViewOverrides().count(), 2);
    QCOMPARE(testDesignConfiguration->getKactus2ViewOverrides().firstKey(), QString("instanceId"));
    QCOMPARE(testDesignConfiguration->getKactus2ViewOverrides().first(), QString("testView"));
    QCOMPARE(testDesignConfiguration->getKactus2ViewOverrides().lastKey(), QString("otherId"));
    QCOMPARE(testDesignConfiguration->getKactus2ViewOverrides().last(), QString("otherView"));
}

QTEST_APPLESS_MAIN(tst_DesignConfigurationReader)

#include "tst_DesignConfigurationReader.moc"
