//-----------------------------------------------------------------------------
// File: tst_BusDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Unit test for class BusDefinition.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QDomNode>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionReader.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
//! Unit test for class BusDefinition.
//-----------------------------------------------------------------------------
class tst_BusDefinitionReader : public QObject
{
    Q_OBJECT

public:
    tst_BusDefinitionReader();

private slots:
    void testReadVLNAndMandatoryFields();

    void testReadTopComments();
    void testProcessingInstructionsAreParsed();

    void testReadExtends();
    void testReadBroadcastAndDescription();
    void testReadMaximumMasterAndMaximumSlave();
    void testReadSystemGroupNames();

    void testReadParameters();
    void testReadAssertions();
    void testReadVendorExtension();

};

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::tst_BusDefinitionReader()
//-----------------------------------------------------------------------------
tst_BusDefinitionReader::tst_BusDefinitionReader()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadVLNAndMandatoryFields()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadVLNAndMandatoryFields()
{
    QDomDocument document;
    document.setContent(QString(        
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>false</ipxact:directConnection>"
            "<ipxact:isAddressable>false</ipxact:isAddressable>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);
   
    VLNV busVLNV = testBus->getVlnv();
    QCOMPARE(busVLNV.getVendor(), QString("TUT"));
    QCOMPARE(busVLNV.getLibrary(), QString("TestLibrary"));
    QCOMPARE(busVLNV.getName(), QString("MinimalBus"));
    QCOMPARE(busVLNV.getVersion(), QString("1.0"));
      
    QCOMPARE(testBus->getDirectConnection(), false);
    QCOMPARE(testBus->getIsAddressable(), false);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadTopComments()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadTopComments()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!--Commented line 1-->"
        "<!--Commented line 2-->"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<!--Comment not to include-->"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
        "</ipxact:busDefinition>"
        "<!--Comment not to include-->"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getTopComments().size(), 2);
    QCOMPARE(testBus->getTopComments().first(), QString("Commented line 1"));   
    QCOMPARE(testBus->getTopComments().last(), QString("Commented line 2"));   
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testProcessingInstructionsAreParsed()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testProcessingInstructionsAreParsed()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!-- Header comments -->"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
        "</ipxact:busDefinition>"
        "<!--Comment not to include-->"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getXmlProcessingInstructions().count(), 1);

    QPair<QString, QString> styleInstruction = testBus->getXmlProcessingInstructions().first();
    QCOMPARE(styleInstruction.first, QString("xml-stylesheet"));
    QCOMPARE(styleInstruction.second, QString("href=\"style.css\""));

}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadExtends()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadExtends()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>extending</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:extends vendor=\"TUT\" library=\"TestLibrary\" name=\"extended\" version=\"1.0\"/>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    VLNV extendedBus = testBus->getExtends();
    QCOMPARE(extendedBus.getVendor(), QString("TUT"));
    QCOMPARE(extendedBus.getLibrary(), QString("TestLibrary"));
    QCOMPARE(extendedBus.getName(), QString("extended"));
    QCOMPARE(extendedBus.getVersion(), QString("1.0"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadBroadcastAndDescription()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadBroadcastAndDescription()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:broadcast>true</ipxact:broadcast>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:description>Bus description</ipxact:description>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getBroadcast().toBool(), true);
    QCOMPARE(testBus->getDescription(), QString("Bus description"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadMaximumMasterAndMaximumSlave()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadMaximumMasterAndMaximumSlave()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:maxMasters>1</ipxact:maxMasters>"
            "<ipxact:maxSlaves>8-1</ipxact:maxSlaves>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getMaxMasters(), QString("1"));
    QCOMPARE(testBus->getMaxSlaves(), QString("8-1"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadSystemGroupNames()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadSystemGroupNames()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:systemGroupNames>"
                "<ipxact:systemGroupName>system1</ipxact:systemGroupName>"
                "<ipxact:systemGroupName>system2</ipxact:systemGroupName>"
                "<ipxact:systemGroupName>system3</ipxact:systemGroupName>"
            "</ipxact:systemGroupNames>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getSystemGroupNames().count(), 3);
    QCOMPARE(testBus->getSystemGroupNames().join(','), QString("system1,system2,system3"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadParameters()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:parameters>"
                "<ipxact:parameter>"
                    "<ipxact:name>parameter1</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"     
                "<ipxact:parameter>"
                    "<ipxact:name>parameter2</ipxact:name>"
                    "<ipxact:value>text</ipxact:value>"
                "</ipxact:parameter>"   
            "</ipxact:parameters>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getParameters()->count(), 2);
    
    QSharedPointer<Parameter> testParameter = testBus->getParameters()->first();
    QCOMPARE(testParameter->name(), QString("parameter1"));
    QCOMPARE(testParameter->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadAssertions()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadAssertions()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssert</ipxact:name>"
                    "<ipxact:displayName>testable assert</ipxact:displayName>"
                    "<ipxact:description>description for assertion</ipxact:description>"
                    "<ipxact:assert>1</ipxact:assert>"
                "</ipxact:assertion>"     
            "</ipxact:assertions>"
        "</ipxact:busDefinition>"));

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getAssertions()->count(), 1);

    QSharedPointer<Assertion> testAssertion = testBus->getAssertions()->first();
    QCOMPARE(testAssertion->name(), QString("testAssert"));
    QCOMPARE(testAssertion->displayName(), QString("testable assert"));
    QCOMPARE(testAssertion->description(), QString("description for assertion"));
    QCOMPARE(testAssertion->getAssert(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionReader::testReadVendorExtension()
//-----------------------------------------------------------------------------
void tst_BusDefinitionReader::testReadVendorExtension()
{
    QDomDocument document;
    document.setContent(QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:directConnection>true</ipxact:directConnection>"
            "<ipxact:isAddressable>true</ipxact:isAddressable>"
            "<ipxact:vendorExtensions>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                "<kactus2:version>3.0.0<kactus2:version>"
            "</ipxact:vendorExtensions>"
        "</ipxact:busDefinition>"));

    QDomElement parameterElement = document.firstChildElement();

    BusDefinitionReader busReader;
    QSharedPointer<BusDefinition> testBus = busReader.createBusDefinitionFrom(document);

    QCOMPARE(testBus->getVendorExtensions()->count(), 2);
    QCOMPARE(testBus->getVersion(), QString("3.0.0"));
}


QTEST_APPLESS_MAIN(tst_BusDefinitionReader)

#include "tst_BusDefinitionReader.moc"
