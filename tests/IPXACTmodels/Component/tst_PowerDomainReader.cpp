//-----------------------------------------------------------------------------
// File: tst_PowerDomainReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.10.2023
//
// Description:
// Unit test for class PowerDomainReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/PowerDomainReader.h>

#include <QtTest>

class tst_PowerDomainReader : public QObject
{
    Q_OBJECT

public:
    tst_PowerDomainReader();

private slots:

    void testReadNamedPowerDomain();
    void testReadProperties();
    void testReadParameters();
    void testReadVendorExtensions();

};

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainReader::tst_PowerDomainReader()
//-----------------------------------------------------------------------------
tst_PowerDomainReader::tst_PowerDomainReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainReader::testReadNamedPowerDomain()
//-----------------------------------------------------------------------------
void tst_PowerDomainReader::testReadNamedPowerDomain()
{
    QString documentContent(
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>testBrief</ipxact:shortDescription>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:powerDomain>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode PowerDomainNode = document.firstChildElement("ipxact:powerDomain");

    QSharedPointer<PowerDomain> testPowerDomain = PowerDomainReader::createFrom(PowerDomainNode);

    QCOMPARE(testPowerDomain->name(), QString("testPowerDomain"));
    QCOMPARE(testPowerDomain->displayName(), QString("testDisplay"));
    QCOMPARE(testPowerDomain->shortDescription(), QString("testBrief"));
    QCOMPARE(testPowerDomain->description(), QString("testDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainReader::testReadProperties()
//-----------------------------------------------------------------------------
void tst_PowerDomainReader::testReadProperties()
{
    QString documentContent(
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
            "<ipxact:alwaysOn>onExpression</ipxact:alwaysOn>"
            "<ipxact:subDomainOf>mainDomain</ipxact:subDomainOf>"
        "</ipxact:powerDomain>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode PowerDomainNode = document.firstChildElement("ipxact:powerDomain");

    QSharedPointer<PowerDomain> testPowerDomain = PowerDomainReader::createFrom(PowerDomainNode);

    QCOMPARE(testPowerDomain->name(), QString("testPowerDomain"));
    QCOMPARE(testPowerDomain->getAlwaysOn(), QString("onExpression"));
    QCOMPARE(testPowerDomain->getSubDomainOf(), QString("mainDomain"));
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_PowerDomainReader::testReadParameters()
{
    QString documentContent(
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
		    "<ipxact:parameters>"
        		"<ipxact:parameter parameterId=\"id1\" prompt=\"Parm 1\" type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>param1</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>5</ipxact:value>"
        		"</ipxact:parameter>"
        		"<ipxact:parameter parameterId=\"id2\" prompt=\"Parm 1\" type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>param2</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>13</ipxact:value>"
        		"</ipxact:parameter>"
    		"</ipxact:parameters>"
        "</ipxact:powerDomain>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode PowerDomainNode = document.firstChildElement("ipxact:powerDomain");

    QSharedPointer<PowerDomain> testPowerDomain = PowerDomainReader::createFrom(PowerDomainNode);

    QCOMPARE(testPowerDomain->getParameters()->size(), 2);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_PowerDomainReader::testReadVendorExtensions()
{
    QString documentContent(
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:powerDomain>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode PowerDomainNode = document.firstChildElement("ipxact:powerDomain");

    QSharedPointer<PowerDomain> testPowerDomain = PowerDomainReader::createFrom(PowerDomainNode);

    QCOMPARE(testPowerDomain->getVendorExtensions()->size(), 1);
    QCOMPARE(testPowerDomain->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_PowerDomainReader)

#include "tst_PowerDomainReader.moc"
