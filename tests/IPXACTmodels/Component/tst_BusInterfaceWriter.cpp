//-----------------------------------------------------------------------------
// File: tst_businterfaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Unit test for class businterfaceWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/BusInterfaceWriter.h>

#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_businterfaceWriter : public QObject
{
	Q_OBJECT

	public:
		tst_businterfaceWriter();

private slots:

    void init();
    void cleanup();

    void testwriteBusInterfaceNameGroup();
    void testWriteIsPresent();
    void testWriteAttributes();
    void testWriteConnectionRequired();
    void testWriteBusType();
    void testWriteBitsInLau();
    void testWriteBitSteering();
	void testWriteBitSteeringExpression2022();
	void testWriteEndianness();
    void testWriteParameters();
    void testWriteVendorExtensions();
    void testWriteAbstractionReference();
    void testWritePortMaps();
    void testWritePortMaps2022();
    void testWriteMaster();
    void testWriteSlave();
    void testWriteSystem();
    void testWriteMirroredSystem();
    void testWriteMonitor();
    void testWriteMirroredSlave();
    void testWriteMirroredMaster();

    void testWriteInitiator2022();
    void testWriteTarget2022();
	void testWriteMirroredTarget2022();

private:
    QSharedPointer<BusInterface> testbusinterface_;
};

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::tst_businterfaceWriter()
//-----------------------------------------------------------------------------
tst_businterfaceWriter::tst_businterfaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::init()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::init()
{
	testbusinterface_ = QSharedPointer<BusInterface>(new BusInterface());
	testbusinterface_->setName("testbusinterface");
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::cleanup()
{
	testbusinterface_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testwriteBusInterfaceNameGroup()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testwriteBusInterfaceNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);
	testbusinterface_->setDisplayName("testDisplay");
	testbusinterface_->setDescription("testDescription");

	QString expectedOutput( 
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"</ipxact:busInterface>");

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteIsPresent()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testbusinterface_->setIsPresent("4-3");

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:isPresent>4-3</ipxact:isPresent>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"</ipxact:busInterface>");

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteAttributes()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteAttributes()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QMap<QString,QString> attributes;
	attributes.insert(QString("firstAttribute"), QString("testValue"));
	testbusinterface_->setAttributes(attributes);

    QString expectedOutput(
        "<ipxact:busInterface firstAttribute=\"testValue\">"
        "<ipxact:name>testbusinterface</ipxact:name>"
        "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
        "</ipxact:busInterface>"
        );

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteBusType()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteBusType()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setBusType(VLNV(VLNV::BUSDEFINITION,"testVendor", "testLibrary", "testName", "testVersion"));

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"testVendor\" library=\"testLibrary\" name=\"testName\" version=\"testVersion\"/>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteConnectionRequired()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteConnectionRequired()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setConnectionRequired(true);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:connectionRequired>true</ipxact:connectionRequired>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteBitsInLau()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteBitsInLau()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setBitsInLau("16");

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:bitsInLau>16</ipxact:bitsInLau>"
		"</ipxact:busInterface>"
		);	

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteBitSteering()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteBitSteering()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);
    
    QMap<QString, QString> bitSteeringAttributes;
    bitSteeringAttributes.insert(QString("testAttribute"), QString("testValue"));
    testbusinterface_->setBitSteeringAttributes(bitSteeringAttributes);
    testbusinterface_->setBitSteering("on");

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:bitSteering testAttribute=\"testValue\">on</ipxact:bitSteering>"
		"</ipxact:busInterface>"
		);


	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteBitSteeringExpression2022()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteBitSteeringExpression2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QMap<QString, QString> bitSteeringAttributes;
    bitSteeringAttributes.insert(QString("testAttribute"), QString("testValue"));
    testbusinterface_->setBitSteeringAttributes(bitSteeringAttributes);
    testbusinterface_->setBitSteering("1");

    QString expectedOutput(
        "<ipxact:busInterface>"
        "<ipxact:name>testbusinterface</ipxact:name>"
        "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
        "<ipxact:bitSteering testAttribute=\"testValue\">1</ipxact:bitSteering>"
        "</ipxact:busInterface>"
    );


    BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std22);

    QCOMPARE(output, expectedOutput);
}
//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteEndianness()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteEndianness()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setEndianness(BusInterface::BIG);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:endianness>big</ipxact:endianness>"
		"</ipxact:busInterface>"
		);	

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteParameters()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteParameters()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setName("firstParameter");
    firstParameter->setValueId("id1");
    firstParameter->setValue("5");
    firstParameter->setType("shortint");
    firstParameter->setAttribute("prompt", "Parm 1");
    firstParameter->setAttribute("resolve", "user");
    firstParameter->setDescription("First bus interface parameter.");

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setName("secondParameter");
    secondParameter->setValueId("id2");
    secondParameter->setValue("1");
    secondParameter->setDescription("Second bus interface parameter.");

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = testbusinterface_->getParameters();
    parameters->append(firstParameter);
    parameters->append(secondParameter);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id1\" prompt=\"Parm 1\" resolve=\"user\" type=\"shortint\">"
		            "<ipxact:name>firstParameter</ipxact:name>"
		            "<ipxact:description>First bus interface parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		        "<ipxact:parameter parameterId=\"id2\">"
		            "<ipxact:name>secondParameter</ipxact:name>"
		            "<ipxact:description>Second bus interface parameter.</ipxact:description>"
		            "<ipxact:value>1</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteVendorExtensions()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteVendorExtensions()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("extensionValue", "true");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testbusinterface_->getVendorExtensions()->append(testExtension);
    testbusinterface_->setDefaultPos(QPointF(1,5));

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:vendorExtensions>"
                "<testExtension extensionValue=\"true\">testValue</testExtension>"
           	    "<kactus2:position x=\"1\" y=\"5\"/>"
		    "</ipxact:vendorExtensions>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteAbstractionReference()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteAbstractionReference()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<AbstractionType> abstractionType(new AbstractionType());
    abstractionType->getViewReferences()->append(QStringLiteral("rtl_view"));
    abstractionType->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "testName", "1.0")));

    testbusinterface_->getAbstractionTypes()->append(abstractionType);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:abstractionTypes>"
		        "<ipxact:abstractionType>"
		            "<ipxact:viewRef>rtl_view</ipxact:viewRef>"
		            "<ipxact:abstractionRef vendor=\"testVendor\" library=\"testLibrary\" name=\"testName\" "
                        "version=\"1.0\"/>"
		        "</ipxact:abstractionType>"
		    "</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);


	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWritePortMaps()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWritePortMaps()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<AbstractionType> abstractionType(new AbstractionType());
    abstractionType->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "testName", "1.0")));

	QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort());
    logicalPort->name_ = "CLK";
    logicalPort->range_ = QSharedPointer<Range>(new Range("testLeft", "testRight"));

	QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort() );
	physicalPort->name_ = "clk_physical";
	physicalPort->partSelect_ = QSharedPointer<PartSelect>(new PartSelect("testLeft", "testRight"));

	QSharedPointer<QStringList> indices(new QStringList());
	indices->append("index1");
	indices->append("index2");
	physicalPort->partSelect_->setIndices(indices);

	QSharedPointer<PortMap> portMap(new PortMap());
	portMap->setLogicalPort(logicalPort);
	portMap->setPhysicalPort(physicalPort);    
	//portMap->setLogicalTieOff("tieoffValue");
    abstractionType->getPortMaps()->append(portMap);

    QSharedPointer<PortMap> tiedPortMap(new PortMap());
    tiedPortMap->setLogicalPort(logicalPort);
    tiedPortMap->setLogicalTieOff("tieoffValue");
    abstractionType->getPortMaps()->append(tiedPortMap);

    testbusinterface_->getAbstractionTypes()->append(abstractionType);

    QString expectedOutput(
		"<ipxact:busInterface>"
    		"<ipxact:name>testbusinterface</ipxact:name>"
	    	"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
    		"<ipxact:abstractionTypes>"
    	    	"<ipxact:abstractionType>"
                    "<ipxact:abstractionRef vendor=\"testVendor\" library=\"testLibrary\" name=\"testName\" "
                        "version=\"1.0\"/>"
    	    	    "<ipxact:portMaps>"
                		"<ipxact:portMap>"
                    		"<ipxact:logicalPort>"
                        		"<ipxact:name>CLK</ipxact:name>"
                        		"<ipxact:range>"
                            		"<ipxact:left>testLeft</ipxact:left>"
                            		"<ipxact:right>testRight</ipxact:right>"
                        		"</ipxact:range>"
                    		"</ipxact:logicalPort>"
                    		"<ipxact:physicalPort>"
                        		"<ipxact:name>clk_physical</ipxact:name>"
                        		"<ipxact:partSelect>"
                            		"<ipxact:range>"
                                		"<ipxact:left>testLeft</ipxact:left>"
                                		"<ipxact:right>testRight</ipxact:right>"
                            		"</ipxact:range>"
                            		"<ipxact:indices>"
                                		"<ipxact:index>index1</ipxact:index>"
                                		"<ipxact:index>index2</ipxact:index>"
                            		"</ipxact:indices>"
                        		"</ipxact:partSelect>"
                    		"</ipxact:physicalPort>"
                		"</ipxact:portMap>"
                        "<ipxact:portMap>"
                            "<ipxact:logicalPort>"
                                "<ipxact:name>CLK</ipxact:name>"
                                "<ipxact:range>"
                                    "<ipxact:left>testLeft</ipxact:left>"
                                    "<ipxact:right>testRight</ipxact:right>"
                                "</ipxact:range>"
                            "</ipxact:logicalPort>"
                            "<ipxact:logicalTieOff>tieoffValue</ipxact:logicalTieOff>"
                        "</ipxact:portMap>"
            		"</ipxact:portMaps>"
        		"</ipxact:abstractionType>"
    		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);
	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWritePortMaps2022()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWritePortMaps2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<AbstractionType> abstractionType(new AbstractionType());
    abstractionType->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(
        new ConfigurableVLNVReference(VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "testName", "1.0")));

	QSharedPointer<PortMap::LogicalPort> logicalPort(new PortMap::LogicalPort());
    logicalPort->name_ = "CLK";
    logicalPort->range_ = QSharedPointer<Range>(new Range("testLeft", "testRight"));

	QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort() );
	physicalPort->name_ = "clk_physical";
	physicalPort->partSelect_ = QSharedPointer<PartSelect>(new PartSelect("testLeft", "testRight"));

	QSharedPointer<QStringList> indices(new QStringList());
	indices->append("index1");
	indices->append("index2");
	physicalPort->partSelect_->setIndices(indices);

	QSharedPointer<PortMap> portMap(new PortMap());
	portMap->setLogicalPort(logicalPort);
	portMap->setPhysicalPort(physicalPort);    
	portMap->getVendorExtensions()->append(QSharedPointer<VendorExtension>(new Kactus2Value("testExtension", "testValue")));
    abstractionType->getPortMaps()->append(portMap);

    QSharedPointer<PortMap> tiedPortMap(new PortMap());
    tiedPortMap->setLogicalPort(logicalPort);
    tiedPortMap->setLogicalTieOff("tieoffValue");
    abstractionType->getPortMaps()->append(tiedPortMap);

    testbusinterface_->getAbstractionTypes()->append(abstractionType);

    QString expectedOutput(
		"<ipxact:busInterface>"
    		"<ipxact:name>testbusinterface</ipxact:name>"
	    	"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
    		"<ipxact:abstractionTypes>"
    	    	"<ipxact:abstractionType>"
                    "<ipxact:abstractionRef vendor=\"testVendor\" library=\"testLibrary\" name=\"testName\" "
                        "version=\"1.0\"/>"
    	    	    "<ipxact:portMaps>"
                		"<ipxact:portMap>"
                    		"<ipxact:logicalPort>"
                        		"<ipxact:name>CLK</ipxact:name>"
                        		"<ipxact:range>"
                            		"<ipxact:left>testLeft</ipxact:left>"
                            		"<ipxact:right>testRight</ipxact:right>"
                        		"</ipxact:range>"
                    		"</ipxact:logicalPort>"
                    		"<ipxact:physicalPort>"
                        		"<ipxact:name>clk_physical</ipxact:name>"
                        		"<ipxact:partSelect>"
                            		"<ipxact:range>"
                                		"<ipxact:left>testLeft</ipxact:left>"
                                		"<ipxact:right>testRight</ipxact:right>"
                            		"</ipxact:range>"
                            		"<ipxact:indices>"
                                		"<ipxact:index>index1</ipxact:index>"
                                		"<ipxact:index>index2</ipxact:index>"
                            		"</ipxact:indices>"
                        		"</ipxact:partSelect>"
                    		"</ipxact:physicalPort>"
							"<ipxact:vendorExtensions>"
								"<testExtension>testValue</testExtension>"
							"</ipxact:vendorExtensions>"
                		"</ipxact:portMap>"
                        "<ipxact:portMap>"
                            "<ipxact:logicalPort>"
                                "<ipxact:name>CLK</ipxact:name>"
                                "<ipxact:range>"
                                    "<ipxact:left>testLeft</ipxact:left>"
                                    "<ipxact:right>testRight</ipxact:right>"
                                "</ipxact:range>"
                            "</ipxact:logicalPort>"
                            "<ipxact:logicalTieOff>tieoffValue</ipxact:logicalTieOff>"
                        "</ipxact:portMap>"
            		"</ipxact:portMaps>"
        		"</ipxact:abstractionType>"
    		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);
	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMaster()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<InitiatorInterface> masterInterface(new InitiatorInterface());
    masterInterface->setAddressSpaceRef("apb");

    testbusinterface_->setInterfaceMode(General::MASTER);
    testbusinterface_->setMaster(masterInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:master>"
		        "<ipxact:addressSpaceRef addressSpaceRef=\"apb\"/>"
		    "</ipxact:master>"
		"</ipxact:busInterface>"
	);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteSlave()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<TargetInterface> slaveInterface(new TargetInterface());
    slaveInterface->setMemoryMapRef("ambaAHB");

    QSharedPointer<TransparentBridge> bridge(new TransparentBridge());
    bridge->setMasterRef("masterBus");
    bridge->setIsPresent("1");

    slaveInterface->getBridges()->append(bridge);

    QSharedPointer<TargetInterface::FileSetRefGroup> fileSetReference(new TargetInterface::FileSetRefGroup());
    fileSetReference->group_ = "group1";

    QStringList refs;
    refs.append("fileset1");
    refs.append("fileset2");
    fileSetReference->fileSetRefs_ = refs;

    slaveInterface->getFileSetRefGroup()->append(fileSetReference);

    testbusinterface_->setInterfaceMode(General::SLAVE);
    testbusinterface_->setSlave(slaveInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:slave>"
		        "<ipxact:memoryMapRef memoryMapRef=\"ambaAHB\"/>"
		        "<ipxact:transparentBridge masterRef=\"masterBus\">"
		            "<ipxact:isPresent>1</ipxact:isPresent>"
		        "</ipxact:transparentBridge>"
		        "<ipxact:fileSetRefGroup>"
		            "<ipxact:group>group1</ipxact:group>"
		            "<ipxact:fileSetRef>fileset1</ipxact:fileSetRef>"
		            "<ipxact:fileSetRef>fileset2</ipxact:fileSetRef>"
		        "</ipxact:fileSetRefGroup>"
		    "</ipxact:slave>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteSystem()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setInterfaceMode(General::SYSTEM);
    testbusinterface_->setSystem("systemGroupName");

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:system>"
		        "<ipxact:group>systemGroupName</ipxact:group>"
		    "</ipxact:system>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredSystem()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setInterfaceMode(General::MIRRORED_SYSTEM);
    testbusinterface_->setSystem("mirroredSystemName");

	QString expectedOutput(
		"<ipxact:busInterface>"
    		"<ipxact:name>testbusinterface</ipxact:name>"
	    	"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
	    	"<ipxact:mirroredSystem>"
	    	    "<ipxact:group>mirroredSystemName</ipxact:group>"
    		"</ipxact:mirroredSystem>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMonitor()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMonitor()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<BusInterface::MonitorInterface> monitorInterface(new BusInterface::MonitorInterface());
    monitorInterface->group_ = "monitorGroup";
    monitorInterface->interfaceMode_ = General::SYSTEM;

    testbusinterface_->setInterfaceMode(General::MONITOR);
    testbusinterface_->setMonitor(monitorInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:monitor interfaceMode=\"system\">"
		        "<ipxact:group>monitorGroup</ipxact:group>"
		    "</ipxact:monitor>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredSlave()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MirroredTargetInterface> mirroredSlave(new MirroredTargetInterface());
    mirroredSlave->setRange("0x00010000");

    QSharedPointer<MirroredTargetInterface::RemapAddress> firstRemapAddress(
        new MirroredTargetInterface::RemapAddress("'h0000"));
    firstRemapAddress->state_ = "decayed";
    firstRemapAddress->remapAttributes_.insert(QString("attribute1"), QString("testValue"));

    QSharedPointer<MirroredTargetInterface::RemapAddress> seconRemapAddress(
        new MirroredTargetInterface::RemapAddress("'hFFFF"));
    seconRemapAddress->state_ = "uptodate";

    mirroredSlave->getRemapAddresses()->append(firstRemapAddress);
    mirroredSlave->getRemapAddresses()->append(seconRemapAddress);

    testbusinterface_->setMirroredSlave( mirroredSlave );

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:mirroredSlave>"
		        "<ipxact:baseAddresses>"
		            "<ipxact:remapAddress state=\"decayed\" attribute1=\"testValue\">'h0000</ipxact:remapAddress>"
		            "<ipxact:remapAddress state=\"uptodate\">'hFFFF</ipxact:remapAddress>"
		            "<ipxact:range>0x00010000</ipxact:range>"
		        "</ipxact:baseAddresses>"
		    "</ipxact:mirroredSlave>"
		"</ipxact:busInterface>"
		);


	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredMaster()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setInterfaceMode(General::MIRRORED_MASTER);

    QString expectedOutput(
        "<ipxact:busInterface>"
            "<ipxact:name>testbusinterface</ipxact:name>"
            "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
            "<ipxact:mirroredMaster/>"
        "</ipxact:busInterface>"
        );

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteInitiator2022()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteInitiator2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<InitiatorInterface> initiatorInterface(new InitiatorInterface());
	initiatorInterface->setAddressSpaceRef("apb");

    testbusinterface_->setInitiator(initiatorInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:initiator>"
		        "<ipxact:addressSpaceRef addressSpaceRef=\"apb\"/>"
		    "</ipxact:initiator>"
		"</ipxact:busInterface>"
	);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWriteTarget2022()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteTarget2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<TargetInterface> targetInterface(new TargetInterface());
	targetInterface->setMemoryMapRef("ambaAHB");
	targetInterface->setModeRefs(QStringList({ "mode1", "mode2" }));

    QSharedPointer<TransparentBridge> bridge(new TransparentBridge());
    bridge->setInitiatorRef("initiatorBus");
	bridge->getVendorExtensions()->append(
		QSharedPointer<VendorExtension>(new Kactus2Value("testExtension", "testValue")));
	targetInterface->getBridges()->append(bridge);

    QSharedPointer<TargetInterface::FileSetRefGroup> fileSetReference(new TargetInterface::FileSetRefGroup());
    fileSetReference->group_ = "group1";

    QStringList refs;
    refs.append("fileset1");
    refs.append("fileset2");
    fileSetReference->fileSetRefs_ = refs;

	targetInterface->getFileSetRefGroup()->append(fileSetReference);

    testbusinterface_->setTarget(targetInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:target>"
		        "<ipxact:memoryMapRef memoryMapRef=\"ambaAHB\">"
					"<ipxact:modeRef>mode1</ipxact:modeRef>"
					"<ipxact:modeRef>mode2</ipxact:modeRef>"
				"</ipxact:memoryMapRef>"
		        "<ipxact:transparentBridge initiatorRef=\"initiatorBus\">"
					"<ipxact:vendorExtensions>"
						"<testExtension>testValue</testExtension>"
					"</ipxact:vendorExtensions>"
		        "</ipxact:transparentBridge>"
		        "<ipxact:fileSetRefGroup>"
		            "<ipxact:group>group1</ipxact:group>"
		            "<ipxact:fileSetRef>fileset1</ipxact:fileSetRef>"
		            "<ipxact:fileSetRef>fileset2</ipxact:fileSetRef>"
		        "</ipxact:fileSetRefGroup>"
		    "</ipxact:target>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}


//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredTarget2022()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredTarget2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MirroredTargetInterface> mirroredTarget(new MirroredTargetInterface());
	mirroredTarget->setRange("0x00010000");

    QSharedPointer<MirroredTargetInterface::RemapAddress> firstRemapAddress(
        new MirroredTargetInterface::RemapAddress("'h0000"));

    QSharedPointer<MirroredTargetInterface::RemapAddress> seconRemapAddress(
        new MirroredTargetInterface::RemapAddress("'hFFFF"));

	mirroredTarget->getRemapAddresses()->append(firstRemapAddress);
	mirroredTarget->getRemapAddresses()->append(seconRemapAddress);

    testbusinterface_->setMirroredTarget(mirroredTarget);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:mirroredTarget>"
		        "<ipxact:baseAddresses>"
					"<ipxact:remapAddresses>"
						"<ipxact:remapAddress>'h0000</ipxact:remapAddress>"
					"</ipxact:remapAddresses>"
					"<ipxact:remapAddresses>"
						"<ipxact:remapAddress>'hFFFF</ipxact:remapAddress>"
					"</ipxact:remapAddresses>"
		            "<ipxact:range>0x00010000</ipxact:range>"
		        "</ipxact:baseAddresses>"
		    "</ipxact:mirroredTarget>"
		"</ipxact:busInterface>"
		);


	BusInterfaceWriter::writeBusInterface(xmlStreamWriter, testbusinterface_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_businterfaceWriter)

#include "tst_BusInterfaceWriter.moc"
