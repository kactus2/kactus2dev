//-----------------------------------------------------------------------------
// File: tst_businterfaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Unit test for class businterfaceReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/BusInterfaceReader.h>

#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_businterfaceReader : public QObject
{
    Q_OBJECT

public:
    tst_businterfaceReader();

private slots:

    void testReadSimplebusinterface();
	void testReadIsPresent();
	void testReadBusType();
	void testReadSlave();
	void testReadMaster();
	void testReadSystem();
	void testReadMirroredSlave();
	void testReadMirroredMaster();
	void testReadMirroredSystem();
	void testReadMonitor();
	void testReadAttributes();
	void testReadConnectionRequired();
	void testReadBitsInLau();
	void testReadBitSteering();
	void testReadEndianness();
	void testReadParameters();
	void testReadVendorExtensions();
	void testReadAbstractionReference();
	void testReadPortMaps();
};

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::tst_businterfaceReader()
//-----------------------------------------------------------------------------
tst_businterfaceReader::tst_businterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSimplebusinterface()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSimplebusinterface()
{
    QString documentContent(
        "<ipxact:busInterface>"
            "<ipxact:name>businterface</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:busInterface>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

    BusinterfaceReader businterfaceReader;
    QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->name(), QString("businterface"));
    QCOMPARE(testbusinterface->displayName(), QString("viewDisplay"));
    QCOMPARE(testbusinterface->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:busInterface>"
            "<ipxact:name>testbusinterface</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:busInterface>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

    BusinterfaceReader businterfaceReader;
    QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->name(), QString("testbusinterface"));
    QCOMPARE(testbusinterface->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBusType()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBusType()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:busType vendor=\"testVendor\" library=\"testLibrary\" name=\"clock\" version=\"1.0\"/>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);
	VLNV refVLNV = VLNV(VLNV::BUSDEFINITION, "testVendor", "testLibrary", "clock", "1.0");

    ConfigurableVLNVReference configurableVLNV (refVLNV);

	QCOMPARE(testbusinterface->getBusType(), configurableVLNV);
	QCOMPARE(testbusinterface->getBusType().getType(), VLNV::BUSDEFINITION);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSlave()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:slave>"
		        "<ipxact:memoryMapRef memoryMapRef=\"ambaAHB\"/>"
		        "<ipxact:transparentBridge masterRef=\"masterBus\">"
		            "<ipxact:isPresent>1</ipxact:isPresent>"
		        "</ipxact:transparentBridge>"
		        "<ipxact:fileSetRefGroup>"
		            "<ipxact:group>testGroup</ipxact:group>"
		            "<ipxact:fileSetRef>fileset1</ipxact:fileSetRef>"
		            "<ipxact:fileSetRef>fileset2</ipxact:fileSetRef>"
		        "</ipxact:fileSetRefGroup>"
		    "</ipxact:slave>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSlave()->getMemoryMapRef(), QString("ambaAHB"));
	QSharedPointer<SlaveInterface::Bridge> bridge = testbusinterface->getSlave()->getBridges()->first();
	QCOMPARE(bridge->masterRef_, QString("masterBus"));
	QCOMPARE(bridge->isPresent_, QString("1"));

	QSharedPointer<SlaveInterface::FileSetRefGroup> refg = testbusinterface->getSlave()->getFileSetRefGroup()->first();
	QCOMPARE(refg->group_, QString("testGroup"));
	QCOMPARE(refg->fileSetRefs_.size(), 2);
	QCOMPARE(refg->fileSetRefs_.first(), QString("fileset1"));
	QCOMPARE(refg->fileSetRefs_.last(), QString("fileset2"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMaster()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:master>"
		        "<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		    "</ipxact:master>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMaster()->getAddressSpaceRef(), QString("apb"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSystem()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:system>"
		    "<ipxact:group>testSystemGroup</ipxact:group>"
		    "</ipxact:system>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSystem(), QString("testSystemGroup"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredSlave()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:mirroredSlave>"
		        "<ipxact:baseAddresses>"
		            "<ipxact:remapAddress state=\"decayed\">'h0000</ipxact:remapAddress>"
		            "<ipxact:remapAddress state=\"uptodate\" testAttribute=\"value\">'hFFFF</ipxact:remapAddress>"
	                "<ipxact:range>'h00010000</ipxact:range>"
	            "</ipxact:baseAddresses>"
	        "</ipxact:mirroredSlave>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);
    QSharedPointer<MirroredSlaveInterface::RemapAddress> firstRemapAddress =
        testbusinterface->getMirroredSlave()->getRemapAddresses()->first();

	QCOMPARE(firstRemapAddress->remapAddress_, QString("'h0000"));
	QCOMPARE(firstRemapAddress->state_, QString("decayed"));

    QSharedPointer<MirroredSlaveInterface::RemapAddress> secondRemapAddress =
        testbusinterface->getMirroredSlave()->getRemapAddresses()->last();

	QCOMPARE(secondRemapAddress->remapAddress_, QString("'hFFFF"));
	QCOMPARE(secondRemapAddress->state_, QString("uptodate"));
    QCOMPARE(secondRemapAddress->remapAttributes_.firstKey(), QString("testAttribute"));
    QCOMPARE(secondRemapAddress->remapAttributes_.first(), QString("value"));

	QCOMPARE(testbusinterface->getMirroredSlave()->getRange(), QString("'h00010000"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredMaster()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:mirroredMaster>"
		        "<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		    "</ipxact:mirroredMaster>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMaster()->getAddressSpaceRef(), QString("apb"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredSystem()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:mirroredSystem>"
		        "<ipxact:group>testSystemGroup</ipxact:group>"
		    "</ipxact:mirroredSystem>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSystem(), QString("testSystemGroup"));
    QCOMPARE(testbusinterface->getInterfaceMode(), General::MIRROREDSYSTEM);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMonitor()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMonitor()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:monitor interfaceMode=\"system\">"
		        "<ipxact:group>testGroup</ipxact:group>"
		    "</ipxact:monitor>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMonitor()->group_, QString("testGroup"));
	QCOMPARE(testbusinterface->getMonitor()->interfaceMode_, General::SYSTEM);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadAttributes()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadAttributes()
{
	QString documentContent(
		"<ipxact:busInterface attribute=\"value\">"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getAttributes().firstKey(), QString("attribute"));
	QCOMPARE(testbusinterface->getAttributes().first(), QString("value"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadConnectionRequired()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadConnectionRequired()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:connectionRequired>true</ipxact:connectionRequired>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->getConnectionRequired(), QString("true"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBitsInLau()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBitsInLau()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:bitsInLau>1234</ipxact:bitsInLau>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->getBitsInLau(), QString("1234"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBitSteering()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBitSteering()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:bitSteering testAttribute=\"value\">on</ipxact:bitSteering>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getBitSteering(), BusInterface::BITSTEERING_ON);
	QCOMPARE(testbusinterface->getBitSteeringAttributes().firstKey(), QString("testAttribute"));
	QCOMPARE(testbusinterface->getBitSteeringAttributes().first(), QString("value"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadEndianness()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadEndianness()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:endianness>big</ipxact:endianness>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getEndianness(), BusInterface::Endianness::BIG);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id1\">"
		            "<ipxact:name>testParameter</ipxact:name>"
		            "<ipxact:description>First generator parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		        "<ipxact:parameter parameterId=\"id2\">"
		            "<ipxact:name>secondParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getParameters()->size(), 2);
	QCOMPARE(testbusinterface->getParameters()->first()->name(), QString("testParameter"));
	QCOMPARE(testbusinterface->getParameters()->first()->getValueId(), QString("id1"));
	QCOMPARE(testbusinterface->getParameters()->first()->getValue(), QString("5"));

	QCOMPARE(testbusinterface->getParameters()->last()->name(), QString("secondParameter"));
	QCOMPARE(testbusinterface->getParameters()->last()->getValueId(), QString("id2"));
	QCOMPARE(testbusinterface->getParameters()->last()->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadVendorExtensions()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:vendorExtensions>"
        		"<testExtension testAttribute=\"attributeValue\"/>"
	        	"<kactus2:position x=\"1\" y=\"5\" />"
	    	"</ipxact:vendorExtensions>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getVendorExtensions()->last()->type(), QString("testExtension"));
    QCOMPARE(testbusinterface->getVendorExtensions()->first()->type(), QString("kactus2:position"));

	QCOMPARE(testbusinterface->getDefaultPos().isNull(), false);
    QCOMPARE(testbusinterface->getDefaultPos().x(), qreal(1));
    QCOMPARE(testbusinterface->getDefaultPos().y(), qreal(5));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadAbstractionReference()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadAbstractionReference()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:abstractionTypes>"
		        "<ipxact:abstractionType>"
		            "<ipxact:viewRef>testView</ipxact:viewRef>"
		            "<ipxact:abstractionRef vendor=\"testVendor\" library=\"testLibrary\" name=\"testName\" version=\"1\"/>"
		        "</ipxact:abstractionType>"
		    "</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QSharedPointer<AbstractionType> abstraction = testbusinterface->getAbstractionTypes()->first();
	QCOMPARE(abstraction->getViewRef(), QString("testView"));
	QCOMPARE(abstraction->getAbstractionRef()->getVendor(), QString("testVendor"));	
	QCOMPARE(abstraction->getAbstractionRef()->getLibrary(), QString("testLibrary"));
    QCOMPARE(abstraction->getAbstractionRef()->getName(), QString("testName"));
	QCOMPARE(abstraction->getAbstractionRef()->getVersion(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadPortMaps()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadPortMaps()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:abstractionTypes>"
		        "<ipxact:abstractionType>"
		            "<ipxact:portMaps>"
		                "<ipxact:portMap>"
		                    "<ipxact:logicalPort>"
		                        "<ipxact:name>CLK</ipxact:name>"
		                        "<ipxact:range>"
		                            "<ipxact:left>leftRangeTest</ipxact:left>"
		                            "<ipxact:right>rightRangeTest</ipxact:right>"
		                        "</ipxact:range>"
		                    "</ipxact:logicalPort>"
		                    "<ipxact:physicalPort>"
		                        "<ipxact:name>clk_physical</ipxact:name>"
		                        "<ipxact:partSelect>"
		                            "<ipxact:range>"
		                                "<ipxact:left>leftTest</ipxact:left>"
		                                "<ipxact:right>rightTest</ipxact:right>"
		                            "</ipxact:range>"
		                            "<ipxact:indices>"
		                                "<ipxact:index>index1</ipxact:index>"
		                                "<ipxact:index>0</ipxact:index>"
		                            "</ipxact:indices>"
		                        "</ipxact:partSelect>"
		                    "</ipxact:physicalPort>"
		                    "<ipxact:logicalTieOff>eoae</ipxact:logicalTieOff>"
		                "</ipxact:portMap>"
		            "</ipxact:portMaps>"
		        "</ipxact:abstractionType>"
		    "</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	BusinterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QSharedPointer<PortMap> portMap = testbusinterface->getAbstractionTypes()->first()->getPortMaps()->first();
	QCOMPARE(portMap->getLogicalPort()->name_, QString("CLK"));
	QCOMPARE(portMap->getLogicalPort()->range_->getLeft(), QString("leftRangeTest"));
	QCOMPARE(portMap->getLogicalPort()->range_->getRight(), QString("rightRangeTest"));

	QCOMPARE(portMap->getPhysicalPort()->name_, QString("clk_physical"));
	QCOMPARE(portMap->getPhysicalPort()->partSelect_->getLeftRange(), QString("leftTest"));
	QCOMPARE(portMap->getPhysicalPort()->partSelect_->getRightRange(), QString("rightTest"));

	QSharedPointer<QStringList > indices = portMap->getPhysicalPort()->partSelect_->getIndices();
	QCOMPARE(indices->first(), QString("index1"));
	QCOMPARE(indices->last(), QString("0"));

	QCOMPARE(portMap->getLogicalTieOff(), QString("eoae") );
}

QTEST_APPLESS_MAIN(tst_businterfaceReader)

#include "tst_businterfaceReader.moc"
