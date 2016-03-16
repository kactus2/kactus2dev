//-----------------------------------------------------------------------------
// File: tst_CPUReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for class CPUReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/CPUReader.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_CPUReader : public QObject
{
    Q_OBJECT

public:
    tst_CPUReader();

private slots:

    void testReadSimpleCPU();
    void testReadIsPresent();
	void testReadParameter();
	void testRead2Parameter();
	void testReadAddressSpaces();
    void testReadAddressSpaceReference();
	void testReadVendorExtension();
};

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::tst_CPUReader()
//-----------------------------------------------------------------------------
tst_CPUReader::tst_CPUReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadSimpleCPU()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadSimpleCPU()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:displayName>cpuDisplay</ipxact:displayName>"
            "<ipxact:description>cpuDescription</ipxact:description>"
        "</ipxact:cpu>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    CPUReader CPUReader;
    QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

    QCOMPARE(testCPU->name(), QString("testCPU"));
    QCOMPARE(testCPU->displayName(), QString("cpuDisplay"));
    QCOMPARE(testCPU->description(), QString("cpuDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:cpu>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    CPUReader CPUReader;
    QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

    QCOMPARE(testCPU->name(), QString("testCPU"));
    QCOMPARE(testCPU->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadParameter()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadParameter()
{
	QString documentContent(
		"<ipxact:cpu>"
    		"<ipxact:name>testCPU</ipxact:name>"
	    	"<ipxact:isPresent>4-3</ipxact:isPresent>"
		    "<ipxact:parameters>"
        		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\"type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>Esko</ipxact:name>"
        	    	"<ipxact:description>First generator parameter.</ipxact:description>"
        		    "<ipxact:value>5</ipxact:value>"
        		"</ipxact:parameter>"
    		"</ipxact:parameters>"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	CPUReader CPUReader;
	QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

	QCOMPARE(testCPU->name(), QString("testCPU"));
	QCOMPARE(testCPU->getIsPresent(), QString("4-3"));
	QCOMPARE(testCPU->getParameters()->size(), 1);
	QCOMPARE(testCPU->getParameters()->first()->name(), QString("Esko"));
	QCOMPARE(testCPU->getParameters()->first()->getValueId(), QString("joq"));
	QCOMPARE(testCPU->getParameters()->first()->getValue(), QString("5"));
	QCOMPARE(testCPU->getParameters()->first()->getType(), QString("shortint"));
	QCOMPARE(testCPU->getParameters()->first()->getAttribute("prompt"), QString("Parm 1"));
	QCOMPARE(testCPU->getParameters()->first()->getAttribute("resolve"), QString("user"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testRead2Parameter()
//-----------------------------------------------------------------------------
void tst_CPUReader::testRead2Parameter()
{
	QString documentContent(
		"<ipxact:cpu>"
    		"<ipxact:name>testCPU5</ipxact:name>"
	    	"<ipxact:isPresent>s</ipxact:isPresent>"
		    "<ipxact:parameters>"
        		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\"type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>Esko</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>5</ipxact:value>"
        		"</ipxact:parameter>"
        		"<ipxact:parameter parameterId=\"ev0\" prompt=\"Parm 1\"type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>Mikko</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>1337</ipxact:value>"
        		"</ipxact:parameter>"
    		"</ipxact:parameters>"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	CPUReader CPUReader;
	QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

	QCOMPARE(testCPU->name(), QString("testCPU5"));
	QCOMPARE(testCPU->getIsPresent(), QString("s"));
	QCOMPARE(testCPU->getParameters()->size(), 2);
	QCOMPARE(testCPU->getParameters()->first()->name(), QString("Esko"));
	QCOMPARE(testCPU->getParameters()->first()->getValueId(), QString("joq"));
	QCOMPARE(testCPU->getParameters()->first()->getValue(), QString("5"));
	QCOMPARE(testCPU->getParameters()->last()->name(), QString("Mikko"));
	QCOMPARE(testCPU->getParameters()->last()->getValueId(), QString("ev0"));
	QCOMPARE(testCPU->getParameters()->last()->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadAddressSpaces()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadAddressSpaces()
{
	QString documentContent(
		"<ipxact:cpu>"
		"<ipxact:name>joqCPU</ipxact:name>"
		"<ipxact:addressSpaceRef addressSpaceRef=\"default_interconnect\" />"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	CPUReader CPUReader;
	QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

	QCOMPARE(testCPU->name(), QString("joqCPU"));
	QCOMPARE(testCPU->getAddressSpaceRefs().size(), 1);
	QCOMPARE(testCPU->getAddressSpaceRefs().first(), QString("default_interconnect"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadAddressSpaceReference()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadAddressSpaceReference()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>joqCPU</ipxact:name>"
            "<ipxact:addressSpaceRef addressSpaceRef=\"default_interconnect\">"
                "<ipxact:isPresent>Balduran</ipxact:isPresent>"
            "</ipxact:addressSpaceRef>"
        "</ipxact:cpu>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    CPUReader CPUReader;
    QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

    QCOMPARE(testCPU->name(), QString("joqCPU"));
    QCOMPARE(testCPU->getAddressSpaceReferences()->size(), 1);
    QCOMPARE(testCPU->getAddressSpaceReferences()->first()->getAddressSpaceRef(), QString("default_interconnect"));
    QCOMPARE(testCPU->getAddressSpaceReferences()->first()->getIsPresent(), QString("Balduran"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadVendorExtension()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadVendorExtension()
{
	QString documentContent(
		"<ipxact:cpu>"
		"<ipxact:name>joqCPU</ipxact:name>"
		"<ipxact:vendorExtensions>"
		"<ulina kolina=\"eaa\"/>"
		"</ipxact:vendorExtensions>"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	CPUReader CPUReader;
	QSharedPointer<Cpu> testCPU = CPUReader.createCPUFrom(cpuNode);

	QCOMPARE(testCPU->name(), QString("joqCPU"));
	QCOMPARE(testCPU->getVendorExtensions()->first()->type(), QString("ulina"));
}

QTEST_APPLESS_MAIN(tst_CPUReader)

#include "tst_CPUReader.moc"
