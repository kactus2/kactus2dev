//-----------------------------------------------------------------------------
// File: tst_PowerDomainWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// Unit test for class PowerDomainWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/PowerDomainWriter.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QtTest>

class tst_PowerDomainWriter : public QObject
{
    Q_OBJECT

public:
	tst_PowerDomainWriter();

private slots:

	void testWritePowerDomainNameGroup();

	void testWriteProperties();

    void testWriteParameters();

	void testWriteVendorExtensions();

};

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainWriter::tst_PowerDomainWriter()
//-----------------------------------------------------------------------------
tst_PowerDomainWriter::tst_PowerDomainWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainWriter::testWritePowerDomainNameGroup()
//-----------------------------------------------------------------------------
void tst_PowerDomainWriter::testWritePowerDomainNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    auto testPowerDomain = QSharedPointer<PowerDomain>(new PowerDomain());
    testPowerDomain->setName("testPowerDomain");
    testPowerDomain->setDisplayName("testDisplay");
    testPowerDomain->setShortDescription("testBrief");
    testPowerDomain->setDescription("testDescription");

	QString expectedOutput = 
		"<ipxact:powerDomain>"
		    "<ipxact:name>testPowerDomain</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:shortDescription>testBrief</ipxact:shortDescription>"
		    "<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:powerDomain>";

	PowerDomainWriter::write(xmlStreamWriter, testPowerDomain);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainWriter::testWriteProperties()
//-----------------------------------------------------------------------------
void tst_PowerDomainWriter::testWriteProperties()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    auto testPowerDomain = QSharedPointer<PowerDomain>(new PowerDomain());
    testPowerDomain->setName("testPowerDomain");
    testPowerDomain->setAlwaysOn("1");
    testPowerDomain->setSubDomainOf("mainDomain");

    QString expectedOutput =
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
            "<ipxact:alwaysOn>1</ipxact:alwaysOn>"
            "<ipxact:subDomainOf>mainDomain</ipxact:subDomainOf>"
        "</ipxact:powerDomain>";

    PowerDomainWriter::write(xmlStreamWriter, testPowerDomain);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainWriter::testWriteParameters()
//-----------------------------------------------------------------------------
void tst_PowerDomainWriter::testWriteParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    auto testPowerDomain = QSharedPointer<PowerDomain>(new PowerDomain());
    testPowerDomain->setName("testPowerDomain");

    auto testParameter = QSharedPointer<Parameter>(new Parameter());
    testParameter->setName("voltage");
    testParameter->setValueId("id");
    testParameter->setValue("0.9");

    testPowerDomain->getParameters()->append(testParameter);

    QString expectedOutput =
        "<ipxact:powerDomain>"
            "<ipxact:name>testPowerDomain</ipxact:name>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"id\">"
                    "<ipxact:name>voltage</ipxact:name>"
                    "<ipxact:value>0.9</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:powerDomain>";

    PowerDomainWriter::write(xmlStreamWriter, testPowerDomain);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainWriter::testWriteVendorExtensions()
//-----------------------------------------------------------------------------
void tst_PowerDomainWriter::testWriteVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);


    auto testPowerDomain = QSharedPointer<PowerDomain>(new PowerDomain());
    testPowerDomain->setName("testPowerDomain");
	testPowerDomain->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
		new Kactus2Value("testExtension", "testValue")));

    QString expectedOutput =
        "<ipxact:powerDomain>"
			"<ipxact:name>testPowerDomain</ipxact:name>"
			"<ipxact:vendorExtensions>"
				"<testExtension>testValue</testExtension>"
			"</ipxact:vendorExtensions>"
        "</ipxact:powerDomain>";

	PowerDomainWriter::write(xmlStreamWriter, testPowerDomain);
    QCOMPARE(output, expectedOutput);
}


QTEST_APPLESS_MAIN(tst_PowerDomainWriter)

#include "tst_PowerDomainWriter.moc"
