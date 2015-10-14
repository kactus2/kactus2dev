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

#include <IPXACTmodels/Component/businterfaceWriter.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/GenericVendorExtension.h>

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
			void testWritePortMaps2();
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

	businterfaceWriter businterfaceWriter;

	testbusinterface_->setDisplayName("testDisplay");
	testbusinterface_->setDescription("testDescription");
	QString expectedOutput = 
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:displayName>testDisplay</ipxact:displayName>"
		"<ipxact:description>testDescription</ipxact:description>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"</ipxact:busInterface>"
		;

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
		"</ipxact:busInterface>"
		);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface joq=\"jotain\">"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"</ipxact:busInterface>"
		);

	QMap<QString,QString> attr;
	attr.insert(QString("joq"), QString("jotain"));
	testbusinterface_->setAttributes(attr);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"1\" library=\"2\" name=\"3\" version=\"4\"/>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setBusType(VLNV(VLNV::BUSDEFINITION,"1","2","3","4"));

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:connectionRequired>true</ipxact:connectionRequired>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setConnectionRequired(true);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:bitsInLau>16</ipxact:bitsInLau>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setBitsInLau("16");

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:bitSteering joq=\"jotain\">on</ipxact:bitSteering>"
		"</ipxact:busInterface>"
		);

	QMap<QString,QString> attr;
	attr.insert(QString("joq"), QString("jotain"));
	testbusinterface_->setBitSteeringAttributes(attr);
	testbusinterface_->setBitSteering(General::ON);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:endianness>big</ipxact:endianness>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setEndianness(General::BIG);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\" resolve=\"user\" type=\"shortint\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"<ipxact:parameter parameterId=\"ev0\" prompt=\"Parm 2\" resolve=\"user\" type=\"longint\">"
		"<ipxact:name>Mikko</ipxact:name>"
		"<ipxact:description>Second generator parameter.</ipxact:description>"
		"<ipxact:value>1337</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<Parameter> para1(new Parameter());
	para1->setName("Esko");
	para1->setValueId("joq");
	para1->setValue("5");
	para1->setType("shortint");
	para1->setAttribute("prompt", "Parm 1");
	para1->setAttribute("resolve", "user");
	para1->setDescription("First generator parameter.");

	QSharedPointer<Parameter> para2(new Parameter());
	para2->setName("Mikko");
	para2->setValueId("ev0");
	para2->setValue("1337");
	para2->setType("longint");
	para2->setAttribute("prompt", "Parm 2");
	para2->setAttribute("resolve", "user");
	para2->setDescription("Second generator parameter.");

	QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = testbusinterface_->getParameters();
	parameters->append(para1);
	parameters->append(para2);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:vendorExtensions>"
            "<ulina kolina=\"eaa\">testValue</ulina>"
           	"<kactus2:position x=\"1\" y=\"5\"/>"
		"</ipxact:vendorExtensions>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	QDomElement extensionNode = document.createElement("ulina");
	extensionNode.setAttribute("kolina", "eaa");
	extensionNode.appendChild(document.createTextNode("testValue"));

	QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

	testbusinterface_->getVendorExtensions()->append(testExtension);
	testbusinterface_->setDefaultPos(QPointF(1,5));

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:abstractionTypes>"
		"<ipxact:abstractionType>"
		"<ipxact:viewRef>joku</ipxact:viewRef>"
		"<ipxact:abstractionRef vendor=\"accellera.org\" library=\"Sample\""
		" name=\"SampleAbstractionDefinition_TLM\" version=\"1.0\"/>"
		"</ipxact:abstractionType>"
		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<BusInterface::AbstractionType> at( new BusInterface::AbstractionType );
	at->viewRef_ = "joku";
	at->abstractionRef_ = QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(VLNV::ABSTRACTIONDEFINITION,
		"accellera.org","Sample","SampleAbstractionDefinition_TLM","1.0"));

    testbusinterface_->getAbstractionTypes()->append(at);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
    		"<ipxact:name>testbusinterface</ipxact:name>"
	    	"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
    		"<ipxact:abstractionTypes>"
    	    	"<ipxact:abstractionType>"
    	    	    "<ipxact:portMaps>"
                		"<ipxact:portMap>"
                    		"<ipxact:logicalTieOff>eoae</ipxact:logicalTieOff>"
                    		"<ipxact:logicalPort>"
                        		"<ipxact:name>CLK</ipxact:name>"
                        		"<ipxact:range>"
                            		"<ipxact:left>vasen</ipxact:left>"
                            		"<ipxact:right>oikea</ipxact:right>"
                        		"</ipxact:range>"
                    		"</ipxact:logicalPort>"
                    		"<ipxact:physicalPort>"
                        		"<ipxact:name>clk</ipxact:name>"
                        		"<ipxact:partSelect>"
                            		"<ipxact:range>"
                                		"<ipxact:left>vasen</ipxact:left>"
                                		"<ipxact:right>oikea</ipxact:right>"
                            		"</ipxact:range>"
                            		"<ipxact:indices>"
                                		"<ipxact:index>eka</ipxact:index>"
                                		"<ipxact:index>toka</ipxact:index>"
                            		"</ipxact:indices>"
                        		"</ipxact:partSelect>"
                    		"</ipxact:physicalPort>"
                		"</ipxact:portMap>"
            		"</ipxact:portMaps>"
        		"</ipxact:abstractionType>"
    		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<BusInterface::AbstractionType> abstractionType( new BusInterface::AbstractionType );

	QSharedPointer<PortMap::LogicalPort> logicalPort( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> physicalPort( new PortMap::PhysicalPort );

	logicalPort->name_ = "CLK";
	physicalPort->name_ = "clk";

	logicalPort->range_ = QSharedPointer<Range>( new Range( "vasen", "oikea" ) );

	QSharedPointer<PartSelect> ps( new PartSelect("vasen","oikea") );
	physicalPort->partSelect_ = ps;

	QSharedPointer<QStringList> indices( new QStringList() );
	indices->append("eka");
	indices->append("toka");
	physicalPort->partSelect_->setIndices(indices);

	QSharedPointer<PortMap> portMap( new PortMap() );
	portMap->setLogicalPort( logicalPort );
	portMap->setPhysicalPort( physicalPort );
    abstractionType->portMaps_->append(portMap);
	portMap->setLogicalTieOff("eoae");

    testbusinterface_->getAbstractionTypes()->append(abstractionType);

	businterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceWriter::testWritePortMaps2()
//-----------------------------------------------------------------------------
void tst_businterfaceWriter::testWritePortMaps2()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:abstractionTypes>"
		"<ipxact:abstractionType>"
		"<ipxact:portMaps>"
		"<ipxact:portMap>"
		"<ipxact:logicalPort>"
		"<ipxact:name>CLK</ipxact:name>"
		"</ipxact:logicalPort>"
		"<ipxact:physicalPort>"
		"<ipxact:name>clk</ipxact:name>"
		"</ipxact:physicalPort>"
		"</ipxact:portMap>"
		"</ipxact:portMaps>"
		"</ipxact:abstractionType>"
		"<ipxact:abstractionType>"
		"<ipxact:portMaps>"
		"<ipxact:portMap>"
		"<ipxact:logicalPort>"
		"<ipxact:name>joq</ipxact:name>"
		"</ipxact:logicalPort>"
		"<ipxact:physicalPort>"
		"<ipxact:name>jotain</ipxact:name>"
		"</ipxact:physicalPort>"
		"</ipxact:portMap>"
		"</ipxact:portMaps>"
		"</ipxact:abstractionType>"
		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<BusInterface::AbstractionType> at( new BusInterface::AbstractionType );
	QSharedPointer<BusInterface::AbstractionType> at2( new BusInterface::AbstractionType );

	QSharedPointer<PortMap::LogicalPort> lport1( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> pport1( new PortMap::PhysicalPort );
	lport1->name_ = "CLK";
	pport1->name_ = "clk";

	QSharedPointer<PortMap> pm( new PortMap() );
	pm->setLogicalPort(lport1);
	pm->setPhysicalPort(pport1);
    at->portMaps_->append(pm);

	QSharedPointer<PortMap::LogicalPort> lport2( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> pport2( new PortMap::PhysicalPort );
	lport2->name_ = "joq";
	pport2->name_ = "jotain";

	QSharedPointer<PortMap> pm2( new PortMap() );
	pm2->setLogicalPort(lport2);
	pm2->setPhysicalPort(pport2);
    at2->portMaps_->append(pm2);

    testbusinterface_->getAbstractionTypes()->append(at);
    testbusinterface_->getAbstractionTypes()->append(at2);

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:master>"
		"<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		"</ipxact:master>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<MasterInterface> interface( new MasterInterface() );
	interface->setAddressSpaceRef("apb");

	testbusinterface_->setInterfaceMode(General::MASTER);
	testbusinterface_->setMaster( interface );

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:slave>"
		"<ipxact:memoryMapRef memoryMapRef=\"ambaAHB\"/>"
		"<ipxact:transparentBridge masterRef=\"joq\">"
		"<ipxact:isPresent>poeoe</ipxact:isPresent>"
		"</ipxact:transparentBridge>"
		"<ipxact:fileSetRefGroup>"
		"<ipxact:group>joq</ipxact:group>"
		"<ipxact:fileSetRef>eka</ipxact:fileSetRef>"
		"<ipxact:fileSetRef>toka</ipxact:fileSetRef>"
		"</ipxact:fileSetRefGroup>"
		"</ipxact:slave>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<SlaveInterface> interface( new SlaveInterface() );
	interface->setMemoryMapRef("ambaAHB");

	QSharedPointer<SlaveInterface::Bridge> bridge( new SlaveInterface::Bridge() );
	bridge->masterRef_ = "joq";
	bridge->isPresent_ = "poeoe";
	QList<QSharedPointer<SlaveInterface::Bridge> > bridges;
	bridges.append(bridge);
	interface->setBridges(bridges);

	QSharedPointer<SlaveInterface::FileSetRefGroup> fsrg( new SlaveInterface::FileSetRefGroup() );
	fsrg->group_ = "joq";
	QStringList refs;
	refs.append("eka");
	refs.append("toka");
	fsrg->fileSetRefs_ = refs;
	QList<QSharedPointer<SlaveInterface::FileSetRefGroup> > sets;
	sets.append(fsrg);
	interface->setFileSetRefGroup(sets);

	testbusinterface_->setInterfaceMode(General::SLAVE);
	testbusinterface_->setSlave( interface );

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:system>"
		"<ipxact:group>esa</ipxact:group>"
		"</ipxact:system>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setInterfaceMode(General::SYSTEM);
	testbusinterface_->setSystem("esa");

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
    		"<ipxact:name>testbusinterface</ipxact:name>"
	    	"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
	    	"<ipxact:mirroredSystem>"
	    	    "<ipxact:group>esa</ipxact:group>"
    		"</ipxact:mirroredSystem>"
		"</ipxact:busInterface>"
		);

	testbusinterface_->setInterfaceMode(General::MIRROREDSYSTEM);
	testbusinterface_->setSystem("esa");

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:monitor interfaceMode=\"system\">"
		"<ipxact:group>esa</ipxact:group>"
		"</ipxact:monitor>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<BusInterface::MonitorInterface> interface( new BusInterface::MonitorInterface() );
	interface->group_ = "esa";
	interface->interfaceMode_ = General::SYSTEM;

	testbusinterface_->setInterfaceMode(General::MONITOR);
	testbusinterface_->setMonitor( interface );

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:mirroredSlave>"
		"<ipxact:baseAddresses>"
		"<ipxact:remapAddress state=\"decayed\" joku=\"jotain\">"
		"0x000000F0</ipxact:remapAddress>"
		"<ipxact:remapAddress state=\"uptodate\" yks=\"kaks\">"
		"0xBEEF</ipxact:remapAddress>"
		"<ipxact:range>0x00010000</ipxact:range>"
		"</ipxact:baseAddresses>"
		"</ipxact:mirroredSlave>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<MirroredSlaveInterface> interface( new MirroredSlaveInterface() );
	interface->setRange("0x00010000");

	QSharedPointer<MirroredSlaveInterface::RemapAddress> ra1( new MirroredSlaveInterface::RemapAddress );
	QSharedPointer<MirroredSlaveInterface::RemapAddress> ra2( new MirroredSlaveInterface::RemapAddress );
		
	ra1->remapAddress_ = "0x000000F0";
	ra1->state_ = "decayed";
	ra1->remapAttributes_.insert( QString("joku"), QString("jotain") );

	ra2->remapAddress_ = "0xBEEF";
	ra2->state_ = "uptodate";
	ra2->remapAttributes_.insert( QString("yks"), QString("kaks") );

	QList<QSharedPointer<MirroredSlaveInterface::RemapAddress> > raas;
	raas.append( ra1 );
	raas.append( ra2 );

	interface->setRemapAddresses( raas );

	testbusinterface_->setInterfaceMode(General::MIRROREDSLAVE);
	testbusinterface_->setMirroredSlave( interface );

	businterfaceWriter businterfaceWriter;
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

	QString expectedOutput(
		"<ipxact:busInterface>"
		"<ipxact:name>testbusinterface</ipxact:name>"
		"<ipxact:busType vendor=\"\" library=\"\" name=\"\" version=\"\"/>"
		"<ipxact:mirroredMaster>"
		"<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		"</ipxact:mirroredMaster>"
		"</ipxact:busInterface>"
		);

	QSharedPointer<MasterInterface> interface( new MasterInterface() );
	interface->setAddressSpaceRef("apb");

	testbusinterface_->setInterfaceMode(General::MIRROREDMASTER);
	testbusinterface_->setMaster( interface );

	businterfaceWriter businterfaceWriter;
	businterfaceWriter.writebusinterface(xmlStreamWriter, testbusinterface_);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_businterfaceWriter)

#include "tst_businterfaceWriter.moc"
