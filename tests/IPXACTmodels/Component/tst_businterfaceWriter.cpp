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

#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
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

			void testWritebusinterfaceNameGroup();
			void testWriteIsPresent();
			void testWriteAttributes();
			void testWriteConnectionRequired();
			void testWriteBusType();
			void testWriteBitsInLau();
			void testWriteBitSteering();
			void testWriteEndianness();
			void testWriteParameters();
			void testWriteVendorExtensions();
			void testWriteAbstractionReference();
			void testWritePortMaps();
			void testWriteMaster();
			void testWriteSlave();
			void testWriteSystem();
			void testWriteMirroredSystem();
			void testWriteMonitor();
			void testWriteMirroredSlave();
			void testWriteMirroredMaster();

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
// Function: tst_businterfaceWriter::testWritebusinterfaceNameGroup()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWritebusinterfaceNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	BusInterfaceWriter businterfaceWriter;

	testbusinterface_->setDisplayName("testDisplay");
	testbusinterface_->setDescription("testDescription");

	QString expectedOutput( 
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"</ipxact:busInterface>");

	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);
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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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
    testbusinterface_->setBitSteering(BusInterface::BITSTEERING_ON);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:bitSteering testAttribute=\"testValue\">on</ipxact:bitSteering>"
		"</ipxact:busInterface>"
		);


	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

    BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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
    abstractionType->setViewRef("rtl_view");
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


	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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
	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMaster()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MasterInterface> masterInterface(new MasterInterface());
    masterInterface->setAddressSpaceRef("apb");

    testbusinterface_->setInterfaceMode(General::MASTER);
    testbusinterface_->setMaster(masterInterface);

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:master>"
		        "<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		    "</ipxact:master>"
		"</ipxact:busInterface>"
	);

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteSlave()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<SlaveInterface> slaveInterface(new SlaveInterface());
    slaveInterface->setMemoryMapRef("ambaAHB");

    QSharedPointer<SlaveInterface::Bridge> bridge(new SlaveInterface::Bridge());
    bridge->masterRef_ = "masterBus";
    bridge->isPresent_ = "1";

    slaveInterface->getBridges()->append(bridge);

    QSharedPointer<SlaveInterface::FileSetRefGroup> fileSetReference(new SlaveInterface::FileSetRefGroup());
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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredSystem()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testbusinterface_->setInterfaceMode(General::MIRROREDSYSTEM);
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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

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

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredSlave()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MirroredSlaveInterface> mirroredSlave(new MirroredSlaveInterface());
    mirroredSlave->setRange("0x00010000");

    QSharedPointer<MirroredSlaveInterface::RemapAddress> firstRemapAddress(
        new MirroredSlaveInterface::RemapAddress("'h0000"));
    firstRemapAddress->state_ = "decayed";
    firstRemapAddress->remapAttributes_.insert(QString("attribute1"), QString("testValue"));

    QSharedPointer<MirroredSlaveInterface::RemapAddress> seconRemapAddress(
        new MirroredSlaveInterface::RemapAddress("'hFFFF"));
    seconRemapAddress->state_ = "uptodate";

    mirroredSlave->getRemapAddresses()->append(firstRemapAddress);
    mirroredSlave->getRemapAddresses()->append(seconRemapAddress);

    testbusinterface_->setInterfaceMode(General::MIRROREDSLAVE);
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


	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testWriteMirroredMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWriteMirroredMaster()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MasterInterface> interface( new MasterInterface() );
    interface->setAddressSpaceRef("apb");

    testbusinterface_->setInterfaceMode(General::MIRROREDMASTER);
    testbusinterface_->setMaster( interface );

	QString expectedOutput(
		"<ipxact:busInterface>"
		    "<ipxact:name>testbusinterface</ipxact:name>"
		    "<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		    "<ipxact:mirroredMaster>"
		        "<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		    "</ipxact:mirroredMaster>"
		"</ipxact:busInterface>"
		);

	BusInterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_businterfaceWriter)

#include "tst_businterfaceWriter.moc"
