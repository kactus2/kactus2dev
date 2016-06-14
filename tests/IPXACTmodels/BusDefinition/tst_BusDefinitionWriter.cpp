//-----------------------------------------------------------------------------
// File: tst_BusDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Unit test for class BusDefinitionWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionWriter.h>

class tst_BusDefinition : public QObject
{
    Q_OBJECT

public:
    tst_BusDefinition();

private slots:

    void testWriteMinimalBusDefinition();

    void testTopCommentsAreWritten();
    void testProcessingInstructionsAreWritten();

    void testWriteExtendingBusDefinition();
    void testWriteBroadcastAndDescription();

    void testWriteMasterAndSlaveMaximum();
    void testWriteSystemGroupNames();

    void testWriteParameters();
    void testWriteAssertions();
    void testVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::tst_BusDefinition()
//-----------------------------------------------------------------------------
tst_BusDefinition::tst_BusDefinition()
{
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::testWriteMinimalBusDefinition()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteMinimalBusDefinition()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "MinimalBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->setDirectConnection(true);
    busDefinition->setIsAddressable(false);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
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
            "<ipxact:isAddressable>false</ipxact:isAddressable>"
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinition::testTopCommentsAreWritten()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testTopCommentsAreWritten()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "MinimalBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->setTopComments("Commented section");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<!--Commented section-->"
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinition::testProcessingInstructionsAreWritten()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testProcessingInstructionsAreWritten()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "StyledBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->addXmlProcessingInstructions("xml-stylesheet", "href=\"style.css\" attribute=\"value\"");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<?xml-stylesheet href=\"style.css\" attribute=\"value\"?>"
        "<ipxact:busDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>TUT</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>StyledBus</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:directConnection>true</ipxact:directConnection>"
        "<ipxact:isAddressable>true</ipxact:isAddressable>"
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::testWriteExtendingBusDefinition()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteExtendingBusDefinition()
{
    VLNV extendedVlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "extended", "1.0");

    VLNV extendingVlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "extending", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(extendingVlnv);
    busDefinition->setExtends(extendedVlnv);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::testWriteBroadcastAndDescription()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteBroadcastAndDescription()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->setBroadcast(true);
    busDefinition->setDescription("Bus description");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::testWriteMasterAndSlaveMaximum()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteMasterAndSlaveMaximum()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->setMaxMasters("1");
    busDefinition->setMaxSlaves("8-1");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionWriter::testWriteSystemGroupNames()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteSystemGroupNames()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);

    QStringList systemGroupNames;
    systemGroupNames << "system1" << "system2" << "system3";

    busDefinition->setSystemGroupNames(systemGroupNames);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinition::testWriteParameters()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteParameters()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setName("parameter1");
    firstParameter->setValueId("");
    firstParameter->setValue("1");

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setName("parameter2");
    secondParameter->setValueId("");
    secondParameter->setValue("text");

    QSharedPointer<QList<QSharedPointer<Parameter> > > busParameters = busDefinition->getParameters();
    busParameters->append(firstParameter);
    busParameters->append(secondParameter);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinition::testWriteAssertions()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testWriteAssertions()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);

    QSharedPointer<Assertion> firstTestAssertion(new Assertion());
    firstTestAssertion->setName("testAssertion1");
    firstTestAssertion->setDisplayName("Display name for assertion.");
    firstTestAssertion->setDescription("Description for assertion.");
    firstTestAssertion->setAssert("1");

    QSharedPointer<Assertion> secondTestAssertion(new Assertion());
    secondTestAssertion->setName("testAssertion2");
    secondTestAssertion->setAssert("0");

    busDefinition->getAssertions()->append(firstTestAssertion);
    busDefinition->getAssertions()->append(secondTestAssertion);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
                    "<ipxact:name>testAssertion1</ipxact:name>"
                    "<ipxact:displayName>Display name for assertion.</ipxact:displayName>"
                    "<ipxact:description>Description for assertion.</ipxact:description>"
                    "<ipxact:assert>1</ipxact:assert>"
                "</ipxact:assertion>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion2</ipxact:name>"
                    "<ipxact:assert>0</ipxact:assert>"
                "</ipxact:assertion>"
            "</ipxact:assertions>"
        "</ipxact:busDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinition::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_BusDefinition::testVendorExtensions()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(vlnv);
    busDefinition->setVersion("3.0.0");

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions = busDefinition->getVendorExtensions();
    vendorExtensions->append(testExtension);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    BusDefinitionWriter busWriter;

    busWriter.writeBusDefinition(xmlStreamWriter, busDefinition);

    QCOMPARE(output, QString(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:busDefinition>\n"));
}


QTEST_APPLESS_MAIN(tst_BusDefinition)

#include "tst_BusDefinitionWriter.moc"
