//-----------------------------------------------------------------------------
// File: tst_OtherClockDriverReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Unit test for class OtherClockDriverReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/OtherClockDriverReader.h>

#include <IPXACTmodels/common/ClockUnit.h>

#include <QtTest>

class tst_OtherClockDriverReader : public QObject
{
    Q_OBJECT

public:
    tst_OtherClockDriverReader();

private slots:

    void testReadSimpleOtherClockDriver();
	void testReadClockPeriod();
	void testReadClockPulses();
};

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverReader::tst_OtherClockDriverReader()
//-----------------------------------------------------------------------------
tst_OtherClockDriverReader::tst_OtherClockDriverReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverReader::testReadSimpleOtherClockDriver()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverReader::testReadSimpleOtherClockDriver()
{
    QString documentContent(
        "<ipxact:otherClockDriver clockName=\"Marko\" clockSource=\"Ari\">"
        "</ipxact:otherClockDriver>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode OtherClockDriverNode = document.firstChildElement("ipxact:otherClockDriver");

    OtherClockDriverReader OtherClockDriverReader;
	QSharedPointer<OtherClockDriver> testOtherClockDriver = OtherClockDriverReader.createOtherClockDriverFrom(OtherClockDriverNode);

	QCOMPARE(testOtherClockDriver->getClockName(), QString("Marko"));
	QCOMPARE(testOtherClockDriver->getClockSource(), QString("Ari"));
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverReader::testReadClockPeriod()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverReader::testReadClockPeriod()
{
	QString documentContent(
		"<ipxact:otherClockDriver clockName=\"Marko\" clockSource=\"Ari\">"
		    "<ipxact:clockPeriod units=\"ps\">6</ipxact:clockPeriod>"
		"</ipxact:otherClockDriver>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode OtherClockDriverNode = document.firstChildElement("ipxact:otherClockDriver");

	OtherClockDriverReader OtherClockDriverReader;
	QSharedPointer<OtherClockDriver> testOtherClockDriver = OtherClockDriverReader.createOtherClockDriverFrom(OtherClockDriverNode);

	QCOMPARE(testOtherClockDriver->getClockPeriod()->getValue(), QString("6"));
	QCOMPARE(testOtherClockDriver->getClockPeriod()->getTimeUnit(), ClockUnit::PS);
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverReader::testReadClockPulses()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverReader::testReadClockPulses()
{
	QString documentContent(
		"<ipxact:otherClockDriver>"
    		"<ipxact:clockPulseOffset units=\"ns\">1</ipxact:clockPulseOffset>"
	    	"<ipxact:clockPulseValue>2</ipxact:clockPulseValue>"
		    "<ipxact:clockPulseDuration units=\"ps\">3</ipxact:clockPulseDuration>"
		"</ipxact:otherClockDriver>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode OtherClockDriverNode = document.firstChildElement("ipxact:otherClockDriver");

	OtherClockDriverReader OtherClockDriverReader;
	QSharedPointer<OtherClockDriver> testOtherClockDriver = OtherClockDriverReader.createOtherClockDriverFrom(OtherClockDriverNode);

	QCOMPARE(testOtherClockDriver->getClockPulseOffset()->getValue(), QString("1"));
	QCOMPARE(testOtherClockDriver->getClockPulseOffset()->getTimeUnit(), ClockUnit::NS);
	QCOMPARE(testOtherClockDriver->getClockPulseValue(), QString("2"));
    QCOMPARE(testOtherClockDriver->getClockPulseDuration()->getValue(), QString("3"));
	QCOMPARE(testOtherClockDriver->getClockPulseDuration()->getTimeUnit(), ClockUnit::PS);
}

QTEST_APPLESS_MAIN(tst_OtherClockDriverReader)

#include "tst_OtherClockDriverReader.moc"
