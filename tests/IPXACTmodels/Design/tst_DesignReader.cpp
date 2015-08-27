//-----------------------------------------------------------------------------
// File: tst_DesignReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class DesignReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Design/DesignReader.h>

#include <QtTest>
#include <QDomNode>

class tst_DesignReader : public QObject
{
    Q_OBJECT

public:
    tst_DesignReader();

private slots:
    void testReadSimpleDesign();
    void testReadComponentInstances();
    void testReadInterconnections();
    void testReadMonitorInterconnections();
    void testReadAdHocConnections();

    void testReadParameters();
    void testReadAssertions();
    void testReadVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::tst_DesignReader()
//-----------------------------------------------------------------------------
tst_DesignReader::tst_DesignReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadSimpleDesign()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadSimpleDesign()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
        "</ipxact:design>");


    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;

    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVlnv().getVendor(), QString("TUT"));
    QCOMPARE(testDesign->getVlnv().getLibrary(), QString("TestLibrary"));
    QCOMPARE(testDesign->getVlnv().getName(), QString("TestDesign"));
    QCOMPARE(testDesign->getVlnv().getVersion(), QString("0.1"));
    QCOMPARE(testDesign->getDescription(), QString("TestDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadComponentInstances()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadComponentInstances()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:componentInstances>"
                "<ipxact:componentInstance>"
                    "<ipxact:instanceName>testInstance</ipxact:instanceName>"
                    "<ipxact:displayName>displayName</ipxact:displayName>"
                    "<ipxact:description>described</ipxact:description>"
                    "<ipxact:isPresent>2-1</ipxact:isPresent>"
                    "<ipxact:componentRef vendor=\"TUT\" library=\"TestLibrary\" name=\"testComponent\""
                        " version=\"1.0\">"
                        "<ipxact:configurableElementValues>"
                            "<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                                "</ipxact:configurableElementValue>"
                        "</ipxact:configurableElementValues>"
                    "</ipxact:componentRef>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:componentInstance>"
            "</ipxact:componentInstances>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader reader;
    QSharedPointer<Design> testDesign = reader.createDesignFrom(document);

    QCOMPARE(testDesign->getComponentInstances()->size(), 1);

    QSharedPointer<ComponentInstance> testInstance = testDesign->getComponentInstances()->first();

    QCOMPARE(testInstance->getInstanceName(), QString("testInstance"));
    QCOMPARE(testInstance->getDisplayName(), QString("displayName"));
    QCOMPARE(testInstance->getDescription(), QString("described"));
    QCOMPARE(testInstance->getIsPresent(), QString("2-1"));

    QSharedPointer<ConfigurableVLNVReference> componentRef =
        testDesign->getComponentInstances()->first()->getComponentRef();
    QCOMPARE(componentRef->getVendor(), QString("TUT"));
    QCOMPARE(componentRef->getLibrary(), QString("TestLibrary"));
    QCOMPARE(componentRef->getName(), QString("testComponent"));
    QCOMPARE(componentRef->getVersion(), QString("1.0"));
    QCOMPARE(componentRef->getConfigurableElementValues()->size(), 1);
    QCOMPARE(componentRef->getConfigurableElementValues()->first()->getReferenceId(), QString("testReferenceID"));
    QCOMPARE(componentRef->getConfigurableElementValues()->first()->getConfigurableValue(), QString("10"));

    QCOMPARE(testInstance->getVendorExtensions()->size(), 1);
    QCOMPARE(testInstance->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadInterconnections()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
           "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:interconnections>"
                "<ipxact:interconnection>"
                    "<ipxact:name>testConnection</ipxact:name>"
                    "<ipxact:displayName>interconnectionDisplay</ipxact:displayName>"
                    "<ipxact:description>interconnectionDescription</ipxact:description>"
                    "<ipxact:isPresent>4-3</ipxact:isPresent>"
                    "<ipxact:activeInterface componentRef=\"startComponent\" busRef=\"startBus\">"
                        "<ipxact:isPresent>1</ipxact:isPresent>"
                        "<ipxact:description>interfaceDescription</ipxact:description>"
                        "<ipxact:excludePorts>"
                            "<ipxact:excludePort>testExcludePort</ipxact:excludePort>"
                        "</ipxact:excludePorts>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:activeInterface>"
                    "<ipxact:activeInterface componentRef=\"otherComponent\" busRef=\"otherBus\"/>"
                    "<ipxact:hierInterface busRef=\"hierBusRef\">"
                        "<ipxact:isPresent>2-1</ipxact:isPresent>"
                        "<ipxact:description>hierDescription</ipxact:description>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:hierInterface>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:interconnection>"
            "</ipxact:interconnections>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader reader;
    QSharedPointer<Design> testDesign = reader.createDesignFrom(document);

    QCOMPARE(testDesign->getInterconnections()->size(), 1);
    
    QSharedPointer<Interconnection> testConnection = testDesign->getInterconnections()->first();

    QCOMPARE(testConnection->name(), QString("testConnection"));
    QCOMPARE(testConnection->displayName(), QString("interconnectionDisplay"));
    QCOMPARE(testConnection->description(), QString("interconnectionDescription"));
    QCOMPARE(testConnection->getIsPresent(), QString("4-3"));

    QCOMPARE(testConnection->getStartInterface()->getComponentReference(), QString("startComponent"));
    QCOMPARE(testConnection->getStartInterface()->getBusReference(), QString("startBus"));
    QCOMPARE(testConnection->getStartInterface()->getIsPresent(), QString("1"));
    QCOMPARE(testConnection->getStartInterface()->getDescription(), QString("interfaceDescription"));
    QCOMPARE(testConnection->getStartInterface()->getExcludePorts()->size(), 1);
    QCOMPARE(testConnection->getStartInterface()->getExcludePorts()->first(), QString("testExcludePort"));
    QCOMPARE(testConnection->getStartInterface()->getVendorExtensions()->size(), 1);
    QCOMPARE(testConnection->getStartInterface()->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(testConnection->getActiveInterfaces()->size(), 1);

    QSharedPointer<ActiveInterface> active = testConnection->getActiveInterfaces()->first();
    QCOMPARE(active->getComponentReference(), QString("otherComponent"));
    QCOMPARE(active->getBusReference(), QString("otherBus"));
    QCOMPARE(active->getIsPresent(), QString(""));
    QCOMPARE(active->getDescription(), QString(""));
    QCOMPARE(active->getExcludePorts()->size(), 0);
    QCOMPARE(active->getVendorExtensions()->size(), 0);

    QCOMPARE(testConnection->getHierInterfaces()->size(), 1);

    QSharedPointer<HierInterface> hierarchy = testConnection->getHierInterfaces()->first();
    QCOMPARE(hierarchy->getBusReference(), QString("hierBusRef"));
    QCOMPARE(hierarchy->getIsPresent(), QString("2-1"));
    QCOMPARE(hierarchy->getDescription(), QString("hierDescription"));
    QCOMPARE(hierarchy->getVendorExtensions()->size(), 1);
    QCOMPARE(hierarchy->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(testConnection->getVendorExtensions()->size(), 1);
    QCOMPARE(testConnection->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadMonitorInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadMonitorInterconnections()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:interconnections>"
                "<ipxact:monitorInterconnection>"
                    "<ipxact:name>monitorInterconnection</ipxact:name>"
                    "<ipxact:displayName>monitorDisplay</ipxact:displayName>"
                    "<ipxact:description>monitorDescription</ipxact:description>"
                    "<ipxact:isPresent>2-1</ipxact:isPresent>"
                    "<ipxact:monitoredActiveInterface componentRef=\"componentRef\" busRef=\"busRef\""
                        " path=\"/path/to/test\">"
                        "<ipxact:description>monitoredActiveDescription</ipxact:description>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:monitoredActiveInterface>"
                    "<ipxact:monitorInterface componentRef=\"otherComponent\" busRef=\"otherBus\" "
                        "path=\"/path/to/other/test\">"
                        "<ipxact:description>monitorInterfaceDescription</ipxact:description>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                        "<ipxact:isPresent>1</ipxact:isPresent>"
                    "</ipxact:monitorInterface>"
                "</ipxact:monitorInterconnection>"
            "</ipxact:interconnections>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader reader;
    QSharedPointer<Design> testDesign = reader.createDesignFrom(document);

    QCOMPARE(testDesign->getMonitorInterconnecions()->size(), 1);

    QSharedPointer<MonitorInterconnection> monitorConnection = testDesign->getMonitorInterconnecions()->first();

    QCOMPARE(monitorConnection->name(), QString("monitorInterconnection"));
    QCOMPARE(monitorConnection->displayName(), QString("monitorDisplay"));
    QCOMPARE(monitorConnection->description(), QString("monitorDescription"));
    QCOMPARE(monitorConnection->getIsPresent(), QString("2-1"));

    QSharedPointer<MonitorInterface> monitoredActive = monitorConnection->getMonitoredActiveInterface();

    QCOMPARE(monitoredActive->getComponentReference(), QString("componentRef"));
    QCOMPARE(monitoredActive->getBusReference(), QString("busRef"));
    QCOMPARE(monitoredActive->getPath(), QString("/path/to/test"));
    QCOMPARE(monitoredActive->getDescription(), QString("monitoredActiveDescription"));
    QCOMPARE(monitoredActive->getVendorExtensions()->size(), 1);
    QCOMPARE(monitoredActive->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(monitorConnection->getMonitorInterfaces()->size(), 1);

    QSharedPointer<MonitorInterface> monitorInterface = monitorConnection->getMonitorInterfaces()->first();

    QCOMPARE(monitorInterface->getComponentReference(), QString("otherComponent"));
    QCOMPARE(monitorInterface->getBusReference(), QString("otherBus"));
    QCOMPARE(monitorInterface->getPath(), QString("/path/to/other/test"));
    QCOMPARE(monitorInterface->getDescription(), QString("monitorInterfaceDescription"));
    QCOMPARE(monitorInterface->getIsPresent(), QString("1"));
    QCOMPARE(monitorInterface->getVendorExtensions()->size(), 1);
    QCOMPARE(monitorInterface->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadAdHocConnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadAdHocConnections()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:adHocConnections>"
                "<ipxact:adHocConnection>"
                    "<ipxact:name>adHoc</ipxact:name>"
                    "<ipxact:displayName>displayAd</ipxact:displayName>"
                    "<ipxact:description>describeAd</ipxact:description>"
                    "<ipxact:isPresent>4-3</ipxact:isPresent>"
                    "<ipxact:tiedValue>default</ipxact:tiedValue>"
                    "<ipxact:portReferences>"
                        "<ipxact:internalPortReference componentRef=\"componentInstance\" portRef=\"internalPort\">"
                            "<ipxact:isPresent>1</ipxact:isPresent>"
                            "<ipxact:partSelect>"
                                "<ipxact:range>"
                                    "<ipxact:left>1</ipxact:left>"
                                    "<ipxact:right>11</ipxact:right>"
                                "</ipxact:range>"
                                "<ipxact:indices>"
                                    "<ipxact:index>8</ipxact:index>"
                                    "<ipxact:index>4+4</ipxact:index>"
                                "</ipxact:indices>"
                            "</ipxact:partSelect>"
                        "</ipxact:internalPortReference>"
                        "<ipxact:externalPortReference portRef=\"externalPort\">"
                            "<ipxact:isPresent>0</ipxact:isPresent>"
                            "<ipxact:partSelect>"
                                "<ipxact:range>"
                                    "<ipxact:left>0</ipxact:left>"
                                    "<ipxact:right>4</ipxact:right>"
                                "</ipxact:range>"
                            "</ipxact:partSelect>"
                        "</ipxact:externalPortReference>"
                    "</ipxact:portReferences>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:adHocConnection>"
            "</ipxact:adHocConnections>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader reader;
    QSharedPointer<Design> testDesign = reader.createDesignFrom(document);

    QCOMPARE(testDesign->getAdHocConnections()->size(), 1);

    QSharedPointer<AdHocConnection> adHocConnection = testDesign->getAdHocConnections()->first();

    QCOMPARE(adHocConnection->name(), QString("adHoc"));
    QCOMPARE(adHocConnection->displayName(), QString("displayAd"));
    QCOMPARE(adHocConnection->description(), QString("describeAd"));
    QCOMPARE(adHocConnection->getIsPresent(), QString("4-3"));
    QCOMPARE(adHocConnection->getTiedValue(), QString("default"));
    QCOMPARE(adHocConnection->getVendorExtensions()->size(), 1);
    QCOMPARE(adHocConnection->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(adHocConnection->getInternalPortReferences()->size(), 1);

    QSharedPointer<PortReference> internalRef = adHocConnection->getInternalPortReferences()->first();
    QCOMPARE(internalRef->getComponentRef(), QString("componentInstance"));
    QCOMPARE(internalRef->getPortRef(), QString("internalPort"));
    QCOMPARE(internalRef->getIsPresent(), QString("1"));

    QSharedPointer<PartSelect> internalPartSelect = internalRef->getPartSelect();
    QCOMPARE(internalPartSelect->getLeftRange(), QString("1"));
    QCOMPARE(internalPartSelect->getRightRange(), QString("11"));
    QCOMPARE(internalPartSelect->getIndices()->size(), 2);
    QCOMPARE(internalPartSelect->getIndices()->first(), QString("8"));
    QCOMPARE(internalPartSelect->getIndices()->last(), QString("4+4"));

    QCOMPARE(adHocConnection->getExternalPortReferences()->size(), 1);

    QSharedPointer<PortReference> externalRef = adHocConnection->getExternalPortReferences()->first();
    QCOMPARE(externalRef->getPortRef(), QString("externalPort"));
    QCOMPARE(externalRef->getIsPresent(), QString("0"));

    QSharedPointer<PartSelect> externalPartSelect = externalRef->getPartSelect();
    QCOMPARE(externalPartSelect->getLeftRange(), QString("0"));
    QCOMPARE(externalPartSelect->getRightRange(), QString("4"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadParameters()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>designParameter</ipxact:name>"
                    "<ipxact:value>10+10</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;

    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getParameters()->size(), 1);
    QCOMPARE(testDesign->getParameters()->first()->name(), QString("designParameter"));
    QCOMPARE(testDesign->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testDesign->getParameters()->first()->getValue(), QString("10+10"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadAssertions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadAssertions()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion</ipxact:name>"
                    "<ipxact:displayName>assertionDisplay</ipxact:displayName>"
                    "<ipxact:description>assertionDescription</ipxact:description>"
                    "<ipxact:assert>13</ipxact:assert>"
                "</ipxact:assertion>"
            "</ipxact:assertions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;

    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getAssertions()->size(), 1);
    QCOMPARE(testDesign->getAssertions()->first()->name(), QString("testAssertion"));
    QCOMPARE(testDesign->getAssertions()->first()->displayName(), QString("assertionDisplay"));
    QCOMPARE(testDesign->getAssertions()->first()->description(), QString("assertionDescription"));
    QCOMPARE(testDesign->getAssertions()->first()->getAssert(), QString("13"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadVendorExtensions()
{
    QString documentContent(
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
            "<ipxact:description>TestDescription</ipxact:description>"
            "<ipxact:vendorExtensions>"
                "<kactus2:version>3.0.0</kactus2:version>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"                
            "</ipxact:vendorExtensions>\n"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVendorExtensions()->last()->type(), QString("testExtension"));
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));
}

QTEST_APPLESS_MAIN(tst_DesignReader)

#include "tst_DesignReader.moc"
