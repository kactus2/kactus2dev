//-----------------------------------------------------------------------------
// File: tst_DesignReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// Unit test for class DesignReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Design/DesignReader.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <designEditors/common/ColumnTypes.h>

#include <QtTest>
#include <QDomNode>

class tst_DesignReader : public QObject
{
    Q_OBJECT

public:
    tst_DesignReader();

private slots:
    void testReadSimpleDesign();
    void testReadProcessingInstructions();

    void testReadComponentInstances();
    void testReadComponentInstanceExtensions();

    void testReadInterconnections();
    void testReadInterconnectionExtensions();

    void testReadMonitorInterconnections();
    
    void testReadAdHocConnections();
    void testReadAdHocConnectionExtensions();

    void testReadParameters();
    void testReadAssertions();
    void testReadVendorExtensions();

    void testReadColumns();
    void testReadSWInstances();
    void testReadPortAdHocVisibilitiesAndPositions();
    void testReadApiConnections();
    void testReadHierApiConnections();
    void testReadComConnections();
    void testReadHierComConnections();
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
// Function: tst_DesignReader::testReadProcessingInstructions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadProcessingInstructions()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<!-- Header comment -->"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<ipxact:design xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDesign</ipxact:name>"
            "<ipxact:version>0.1</ipxact:version>"
        "</ipxact:design>");


    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;

    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getXmlProcessingInstructions().count(), 1);

    QPair<QString, QString> styleInstruction = testDesign->getXmlProcessingInstructions().first();
    QCOMPARE(styleInstruction.first, QString("xml-stylesheet"));
    QCOMPARE(styleInstruction.second, QString("href=\"style.css\""));

    QCOMPARE(testDesign->getTopComments().first(), QString(" Header comment "));
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
                        "<kactus2:uuid>testUUID</kactus2:uuid>"
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
    QCOMPARE(testInstance->getVendorExtensions()->first()->type(), QString("kactus2:uuid"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadComponentInstanceExtensions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadComponentInstanceExtensions()
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
                    "<ipxact:componentRef vendor=\"TUT\" library=\"TestLibrary\" name=\"testComponent\""
                        " version=\"1.0\">"
                        "<ipxact:configurableElementValues>"
                            "<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                            "</ipxact:configurableElementValue>"
                        "</ipxact:configurableElementValues>"
                    "</ipxact:componentRef>"
                    "<ipxact:vendorExtensions>"
                        "<kactus2:position x=\"10\" y=\"10\"/>"
                        "<kactus2:uuid>testUUID</kactus2:uuid>"
                        "<kactus2:imported importRef=\"importSource\"/>"
                        "<kactus2:portPositions>"
                            "<kactus2:portPosition busRef=\"testInterface\" x=\"4\" y=\"4\"/>"
                        "</kactus2:portPositions>"
                        "<kactus2:adHocVisibilities>"
                            "<kactus2:adHocVisible portName=\"adHocPort\" x=\"3\" y=\"3\"/>"
                        "</kactus2:adHocVisibilities>"
                        "<kactus2:apiInterfacePositions>"
                            "<kactus2:apiInterfacePosition apiRef=\"apiInterface\" x=\"2\" y=\"2\"/>"
                        "</kactus2:apiInterfacePositions>"
                        "<kactus2:comInterfacePositions>"
                            "<kactus2:comInterfacePosition comRef=\"comInterface\" x=\"1\" y=\"1\"/>"
                        "</kactus2:comInterfacePositions>"
                        "<kactus2:propertyValues>"
                            "<kactus2:propertyValue name=\"testSWProperty\" value=\"8\"/>"
                        "</kactus2:propertyValues>"
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

    QSharedPointer<ConfigurableVLNVReference> componentRef =
        testDesign->getComponentInstances()->first()->getComponentRef();
    QCOMPARE(componentRef->getName(), QString("testComponent"));

    QCOMPARE(testInstance->getVendorExtensions()->size(), 8);
    QCOMPARE(testInstance->getUuid(), QString("testUUID"));
    QCOMPARE(testInstance->getPosition().x(), qreal(10));
    QCOMPARE(testInstance->getPosition().y(), qreal(10));

    QCOMPARE(testInstance->isImported(), true);
    QCOMPARE(testInstance->getImportRef(), QString("importSource"));

    QCOMPARE(testInstance->getBusInterfacePositions().count(), 1);
    QCOMPARE(testInstance->getBusInterfacePositions().firstKey(), QString("testInterface"));
    QCOMPARE(testInstance->getBusInterfacePositions().first().x(), qreal(4));
    QCOMPARE(testInstance->getBusInterfacePositions().first().y(), qreal(4));

    QCOMPARE(testInstance->getAdHocPortPositions().count(), 1);
    QCOMPARE(testInstance->getAdHocPortPositions().firstKey(), QString("adHocPort"));
    QCOMPARE(testInstance->getAdHocPortPositions().first().x(), qreal(3));
    QCOMPARE(testInstance->getAdHocPortPositions().first().y(), qreal(3));

    QCOMPARE(testInstance->getApiInterfacePositions().count(), 1);
    QCOMPARE(testInstance->getApiInterfacePositions().firstKey(), QString("apiInterface"));
    QCOMPARE(testInstance->getApiInterfacePositions().first().x(), qreal(2));
    QCOMPARE(testInstance->getApiInterfacePositions().first().y(), qreal(2));

    QCOMPARE(testInstance->getComInterfacePositions().count(), 1);
    QCOMPARE(testInstance->getComInterfacePositions().firstKey(), QString("comInterface"));
    QCOMPARE(testInstance->getComInterfacePositions().first().x(), qreal(1));
    QCOMPARE(testInstance->getComInterfacePositions().first().y(), qreal(1));

    QCOMPARE(testInstance->getPropertyValues().count(), 1);
    QCOMPARE(testInstance->getPropertyValues().firstKey(), QString("testSWProperty"));
    QCOMPARE(testInstance->getPropertyValues().first(), QString("8"));
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
// Function: tst_DesignReader::testReadInterconnectionExtensions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadInterconnectionExtensions()
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
                    "<ipxact:name>testActiveHier</ipxact:name>"
                    "<ipxact:activeInterface componentRef=\"componentRef\" busRef=\"busRef\"/>"
                    "<ipxact:hierInterface busRef=\"hierBusRef\">"
                        "<ipxact:vendorExtensions>"
                            "<kactus2:route>"
                                "<kactus2:position x=\"1\" y=\"1\"/>"
                                "<kactus2:position x=\"1\" y=\"2\"/>"
                            "</kactus2:route>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:hierInterface>"
                    "<ipxact:vendorExtensions>"
                        "<kactus2:offPage/>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:interconnection>"
            "</ipxact:interconnections>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader reader;
    QSharedPointer<Design> testDesign = reader.createDesignFrom(document);

    QCOMPARE(testDesign->getInterconnections()->size(), 1);

    QSharedPointer<Interconnection> interconnection = testDesign->getInterconnections()->first();
    QCOMPARE(interconnection->name(), QString("testActiveHier"));

    QCOMPARE(interconnection->getStartInterface()->getComponentReference(), QString("componentRef"));
    QCOMPARE(interconnection->getStartInterface()->getBusReference(), QString("busRef"));
    QCOMPARE(interconnection->isOffPage(), true);

    QCOMPARE(interconnection->getActiveInterfaces()->size(), 0);

    QCOMPARE(interconnection->getHierInterfaces()->size(), 1);
    QCOMPARE(interconnection->getHierInterfaces()->first()->getBusReference(), QString("hierBusRef"));

    QCOMPARE(interconnection->getHierInterfaces()->first()->getRoute().size(), 2);
    QCOMPARE(interconnection->getHierInterfaces()->first()->getRoute().first().x(), qreal(1));
    QCOMPARE(interconnection->getHierInterfaces()->first()->getRoute().first().x(), qreal(1));
    QCOMPARE(interconnection->getHierInterfaces()->first()->getRoute().last().x(), qreal(1));
    QCOMPARE(interconnection->getHierInterfaces()->first()->getRoute().last().y(), qreal(2));
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
// Function: tst_DesignReader::testReadAdHocConnectionExtensions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadAdHocConnectionExtensions()
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
                    "<ipxact:portReferences>"
                        "<ipxact:internalPortReference componentRef=\"componentInstance\" portRef=\"internalPort\"/>"
                        "<ipxact:externalPortReference portRef=\"externalPort\"/>"
                    "</ipxact:portReferences>"
                    "<ipxact:vendorExtensions>"
                        "<kactus2:offPage/>"
                        "<kactus2:route>"
                            "<kactus2:position x=\"1\" y=\"1\"/>"
                            "<kactus2:position x=\"4\" y=\"1\"/>"
                            "<kactus2:position x=\"4\" y=\"2\"/>"
                        "</kactus2:route>"
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
    QCOMPARE(adHocConnection->isOffPage(), true);

    QCOMPARE(adHocConnection->getRoute().size(), 3);
    QCOMPARE(adHocConnection->getRoute().at(0).x(), qreal(1));
    QCOMPARE(adHocConnection->getRoute().at(0).y(), qreal(1));
    QCOMPARE(adHocConnection->getRoute().at(1).x(), qreal(4));
    QCOMPARE(adHocConnection->getRoute().at(1).y(), qreal(1));
    QCOMPARE(adHocConnection->getRoute().at(2).x(), qreal(4));
    QCOMPARE(adHocConnection->getRoute().at(2).y(), qreal(2));
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

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadColumns()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadColumns()
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
                "<kactus2:columnLayout>"
                    "<kactus2:column name=\"testColumn\" contentType=\"2\" allowedItems=\"0\" "
                        "minWidth=\"259\" width=\"259\"/>"
                    "<kactus2:column name=\"otherColumn\" contentType=\"1\" allowedItems=\"0\" "
                        "minWidth=\"259\" width=\"259\"/>"
                "</kactus2:columnLayout>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<ColumnDesc> > columnList = testDesign->getColumns();
    QCOMPARE(columnList.size(), 2);

    int firstColumnWidth = columnList.first()->getWidth();
    QCOMPARE(columnList.first()->name(), QString("testColumn"));
    QCOMPARE(columnList.first()->getContentType(), ColumnTypes::COMPONENTS);
    QVERIFY(columnList.first()->getAllowedItems() == 0);
    QCOMPARE(firstColumnWidth, 259);

    int secondColumnWidth = columnList.last()->getWidth();
    QCOMPARE(columnList.last()->name(), QString("otherColumn"));
    QCOMPARE(columnList.last()->getContentType(), ColumnTypes::BUSES);
    QVERIFY(columnList.last()->getAllowedItems() == 0);
    QCOMPARE(secondColumnWidth, 259);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadSWInstances()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadSWInstances()
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
                "<kactus2:swInstances>"
                    "<kactus2:swInstance>"
                        "<kactus2:instanceName>testInstance</kactus2:instanceName>"
                        "<kactus2:displayName>testDisplay</kactus2:displayName>"
                        "<kactus2:description>testDescription</kactus2:description>"
                        "<kactus2:componentRef vendor=\"TUT\" library=\"TestLibrary\" name=\"refComponent\""
                            " version=\"1.1\"/>"
                        "<kactus2:fileSetRef>filesetRef</kactus2:fileSetRef>"
                        "<kactus2:mapping hwRef=\"hwRef\"/>"
                        "<kactus2:position x=\"1\" y=\"2\"/>"
                        "<kactus2:imported importRef=\"importer\"/>"
                        "<kactus2:draft/>"
                        "<kactus2:propertyValues>"
                            "<kactus2:propertyValue name=\"testProperty\" value=\"value\"/>"
                        "</kactus2:propertyValues>"
                        "<kactus2:apiInterfacePositions>"
                            "<kactus2:apiInterfacePosition apiRef=\"newApi\" x=\"0\" y=\"1\"/>"
                        "</kactus2:apiInterfacePositions>"
                        "<kactus2:comInterfacePositions>"
                            "<kactus2:comInterfacePosition comRef=\"newCom\" x=\"1\" y=\"1\"/>"
                        "</kactus2:comInterfacePositions>"
                    "</kactus2:swInstance>"
                "</kactus2:swInstances>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<SWInstance> > swInstances = testDesign->getSWInstances();
    QCOMPARE(swInstances.size(), 1);
    QCOMPARE(swInstances.first()->getInstanceName(), QString("testInstance"));
    QCOMPARE(swInstances.first()->getDisplayName(), QString("testDisplay"));
    QCOMPARE(swInstances.first()->getDescription(), QString("testDescription"));

    QCOMPARE(swInstances.first()->getComponentRef()->getVendor(), QString("TUT"));
    QCOMPARE(swInstances.first()->getComponentRef()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(swInstances.first()->getComponentRef()->getName(), QString("refComponent"));
    QCOMPARE(swInstances.first()->getComponentRef()->getVersion(), QString("1.1"));

    QCOMPARE(swInstances.first()->getFileSetRef(), QString("filesetRef"));
    QCOMPARE(swInstances.first()->getMapping(), QString("hwRef"));
    QCOMPARE(swInstances.first()->getPosition().x(), qreal(1));
    QCOMPARE(swInstances.first()->getPosition().y(), qreal(2));
    
    QCOMPARE(swInstances.first()->isImported(), true);
    QCOMPARE(swInstances.first()->getImportRef(), QString("importer"));
    
    QCOMPARE(swInstances.first()->isDraft(), true);
    QCOMPARE(swInstances.first()->getPropertyValues().count(), 1);
    QCOMPARE(swInstances.first()->getPropertyValues().firstKey(), QString("testProperty"));
    QCOMPARE(swInstances.first()->getPropertyValues().first(), QString("value"));

    QCOMPARE(swInstances.first()->getApiInterfacePositions().count(), 1);
    QCOMPARE(swInstances.first()->getApiInterfacePositions().firstKey(), QString("newApi"));
    QCOMPARE(swInstances.first()->getApiInterfacePositions().first().x(), qreal(0));
    QCOMPARE(swInstances.first()->getApiInterfacePositions().first().y(), qreal(1));

    QCOMPARE(swInstances.first()->getComInterfacePositions().count(), 1);
    QCOMPARE(swInstances.first()->getComInterfacePositions().firstKey(), QString("newCom"));
    QCOMPARE(swInstances.first()->getComInterfacePositions().first().x(), qreal(1));
    QCOMPARE(swInstances.first()->getComInterfacePositions().first().y(), qreal(1));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadPortAdHocVisibilitiesAndPositions()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadPortAdHocVisibilitiesAndPositions()
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
                "<kactus2:adHocVisibilities>"
                    "<kactus2:adHocVisible portName=\"testPort\" x=\"4\" y=\"25\"/>"
                "</kactus2:adHocVisibilities>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QSharedPointer<VendorExtension> adhocExtension = testDesign->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();

    QMap<QString, QPointF> adHocPorts;
    foreach (QSharedPointer<VendorExtension> extension, adhocGroup->getByType("kactus2:adHocVisible"))
    {
        QSharedPointer<Kactus2Placeholder> portAdHocVisibility = extension.dynamicCast<Kactus2Placeholder>();

        QString portName = portAdHocVisibility->getAttributeValue("portName");
        int positionX = portAdHocVisibility->getAttributeValue("x").toInt();
        int positionY = portAdHocVisibility->getAttributeValue("y").toInt();

        adHocPorts.insert(portName, QPointF(positionX, positionY));
    }

    QCOMPARE(adHocPorts.firstKey(), QString("testPort"));
    QCOMPARE(adHocPorts.first().x(), qreal(4));
    QCOMPARE(adHocPorts.first().y(), qreal(25));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadApiDependencies()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadApiConnections()
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
                "<kactus2:apiConnections>"
                    "<kactus2:apiConnection>"
                        "<ipxact:name>apiConnect</ipxact:name>"
                        "<ipxact:displayName>connection</ipxact:displayName>"
                        "<ipxact:description>described</ipxact:description>"
                        "<kactus2:activeApiInterface componentRef=\"applicationOne\" apiRef=\"busOne\"/>"
                        "<kactus2:activeApiInterface componentRef=\"applicationTwo\" apiRef=\"busTwo\"/>"
                        "<kactus2:route offPage=\"false\">"
                            "<kactus2:position x=\"1\" y=\"1\"/>"
                        "</kactus2:route>"
                        "<kactus2:imported/>"
                    "</kactus2:apiConnection>"
                "</kactus2:apiConnections>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<ApiInterconnection> > apiConnections = testDesign->getApiConnections();
    QCOMPARE(apiConnections.size(), 1);
    QCOMPARE(apiConnections.first()->name(), QString("apiConnect"));
    QCOMPARE(apiConnections.first()->displayName(), QString("connection"));
    QCOMPARE(apiConnections.first()->description(), QString("described"));

    QCOMPARE(apiConnections.first()->getStartInterface()->getComponentReference(), QString("applicationOne"));
    QCOMPARE(apiConnections.first()->getStartInterface()->getBusReference(), QString("busOne"));
    QCOMPARE(apiConnections.first()->getActiveInterfaces()->first()->getComponentReference(), QString("applicationTwo"));
    QCOMPARE(apiConnections.first()->getActiveInterfaces()->first()->getBusReference(), QString("busTwo"));

    QCOMPARE(apiConnections.first()->isOffPage(), false);

    QCOMPARE(apiConnections.first()->isImported(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadHierApiConnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadHierApiConnections()
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
                "<kactus2:apiConnections>"
                    "<kactus2:apiConnection>"
                        "<ipxact:name>hierApi</ipxact:name>"
                        "<ipxact:displayName>display</ipxact:displayName>"
                        "<ipxact:description>description</ipxact:description>"
                        "<kactus2:activeApiInterface componentRef=\"applicationOne\" apiRef=\"busOne\"/>"           
                        "<kactus2:hierApiInterface apiRef=\"topInterface\"/>"      
                    "</kactus2:apiConnection>"
                "</kactus2:apiConnections>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<ApiInterconnection> > hierApiConnections = testDesign->getApiConnections();
    QCOMPARE(hierApiConnections.size(), 1);
    QCOMPARE(hierApiConnections.first()->name(), QString("hierApi"));
    QCOMPARE(hierApiConnections.first()->displayName(), QString("display"));
    QCOMPARE(hierApiConnections.first()->description(), QString("description"));

    QCOMPARE(hierApiConnections.first()->getEndInterface()->getBusReference(), QString("topInterface"));
    QCOMPARE(hierApiConnections.first()->getStartInterface()->getComponentReference(), QString("applicationOne"));
    QCOMPARE(hierApiConnections.first()->getStartInterface()->getBusReference(), QString("busOne"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadComConnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadComConnections()
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
                "<kactus2:comConnections>"
                    "<kactus2:comConnection>"
                        "<ipxact:name>comConnect</ipxact:name>"
                        "<ipxact:displayName>display</ipxact:displayName>"
                        "<ipxact:description>description</ipxact:description>"
                        "<kactus2:activeComInterface componentRef=\"applicationOne\" comRef=\"busOne\"/>"
                        "<kactus2:activeComInterface componentRef=\"applicationTwo\" comRef=\"busTwo\"/>"
                        "<kactus2:route offPage=\"false\">"
                            "<kactus2:position x=\"1\" y=\"1\"/>"
                        "</kactus2:route>"
                    "</kactus2:comConnection>"
                "</kactus2:comConnections>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<ComInterconnection> > comConnections = testDesign->getComConnections();
    QCOMPARE(comConnections.size(), 1);
    QCOMPARE(comConnections.first()->name(), QString("comConnect"));
    QCOMPARE(comConnections.first()->displayName(), QString("display"));
    QCOMPARE(comConnections.first()->description(), QString("description"));

    QCOMPARE(comConnections.first()->getStartInterface()->getComponentReference(), QString("applicationOne"));
    QCOMPARE(comConnections.first()->getStartInterface()->getBusReference(), QString("busOne"));
    QCOMPARE(comConnections.first()->getActiveInterfaces()->first()->getComponentReference(), QString("applicationTwo"));
    QCOMPARE(comConnections.first()->getActiveInterfaces()->first()->getBusReference(), QString("busTwo"));
}

//-----------------------------------------------------------------------------
// Function: tst_DesignReader::testReadHierComConnections()
//-----------------------------------------------------------------------------
void tst_DesignReader::testReadHierComConnections()
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
                "<kactus2:comConnections>"
                    "<kactus2:comConnection>"
                        "<ipxact:name>hierComConnection</ipxact:name>"
                        "<ipxact:displayName>display</ipxact:displayName>"
                        "<ipxact:description>description</ipxact:description>"
                        "<kactus2:activeComInterface componentRef=\"applicationOne\" comRef=\"busOne\"/>"
                        "<kactus2:hierComInterface comRef=\"topInterface\"/>"
                    "</kactus2:comConnection>"
                "</kactus2:comConnections>"
            "</ipxact:vendorExtensions>"
        "</ipxact:design>");

    QDomDocument document;
    document.setContent(documentContent);

    DesignReader designReader;
    QSharedPointer<Design> testDesign = designReader.createDesignFrom(document);

    QCOMPARE(testDesign->getVendorExtensions()->size(), 2);
    QCOMPARE(testDesign->getVersion(), QString("3.0.0"));

    QList<QSharedPointer<ComInterconnection> > hierComConnections = testDesign->getComConnections();
    QCOMPARE(hierComConnections.size(), 1);
    QCOMPARE(hierComConnections.first()->name(), QString("hierComConnection"));
    QCOMPARE(hierComConnections.first()->displayName(), QString("display"));
    QCOMPARE(hierComConnections.first()->description(), QString("description"));

    QCOMPARE(hierComConnections.first()->getEndInterface()->getBusReference(), QString("topInterface"));
    QCOMPARE(hierComConnections.first()->getStartInterface()->getComponentReference(), QString("applicationOne"));
    QCOMPARE(hierComConnections.first()->getStartInterface()->getBusReference(), QString("busOne"));
}

QTEST_APPLESS_MAIN(tst_DesignReader)

#include "tst_DesignReader.moc"
