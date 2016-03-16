//-----------------------------------------------------------------------------
// File: tst_OtherClockDriverWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.10.2015
//
// Description:
// Unit test for class OtherClockDriverWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ClockUnit.h>

#include <IPXACTmodels/Component/OtherClockDriverWriter.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_OtherClockDriverWriter : public QObject
{
    Q_OBJECT

public:
	tst_OtherClockDriverWriter();

private slots:
	void init();
	void cleanup();

	void testWriteOtherClockDriver();
	void testWriteClockPeriod();
	void testWriteClockPulses();

private:

	QSharedPointer<OtherClockDriver> testOtherClockDriver_;
};

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::tst_OtherClockDriverWriter()
//-----------------------------------------------------------------------------
tst_OtherClockDriverWriter::tst_OtherClockDriverWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::init()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverWriter::init()
{
	testOtherClockDriver_ = QSharedPointer<OtherClockDriver>(new OtherClockDriver());
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverWriter::cleanup()
{
	testOtherClockDriver_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::testWriteOtherClockDriver()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverWriter::testWriteOtherClockDriver()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testOtherClockDriver_->setClockName("Marko");
	testOtherClockDriver_->setClockSource("Ari");

	QString expectedOutput(
		"<ipxact:otherClockDriver clockName=\"Marko\" clockSource=\"Ari\">"
        "<ipxact:clockPeriod></ipxact:clockPeriod>"
        "<ipxact:clockPulseOffset></ipxact:clockPulseOffset>"
        "<ipxact:clockPulseValue></ipxact:clockPulseValue>"
        "<ipxact:clockPulseDuration></ipxact:clockPulseDuration>"
		"</ipxact:otherClockDriver>"
		);

	OtherClockDriverWriter otherClockDriverWriter;
	otherClockDriverWriter.writeOtherClockDriver(xmlStreamWriter, testOtherClockDriver_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::testWriteClockPeriod()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverWriter::testWriteClockPeriod()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ClockUnit> clockPeriod (new ClockUnit("6"));
    clockPeriod->setTimeUnit(ClockUnit::PS);

    testOtherClockDriver_->setClockPeriod(clockPeriod);

	QString expectedOutput(
		"<ipxact:otherClockDriver clockName=\"\">"
    		"<ipxact:clockPeriod units=\"ps\">6</ipxact:clockPeriod>"
	    	"<ipxact:clockPulseOffset></ipxact:clockPulseOffset>"
		    "<ipxact:clockPulseValue></ipxact:clockPulseValue>"
    		"<ipxact:clockPulseDuration></ipxact:clockPulseDuration>"
		"</ipxact:otherClockDriver>"
		);

	OtherClockDriverWriter otherClockDriverWriter;
	otherClockDriverWriter.writeOtherClockDriver(xmlStreamWriter, testOtherClockDriver_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverWriter::testWriteClockPulses()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverWriter::testWriteClockPulses()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ClockUnit> pulseOffset (new ClockUnit("1"));
    pulseOffset->setTimeUnit(ClockUnit::NS);

    QSharedPointer<ClockUnit> pulseDuration (new ClockUnit("3"));
    pulseDuration->setTimeUnit(ClockUnit::PS);

    testOtherClockDriver_->setClockPulseOffset(pulseOffset);
    testOtherClockDriver_->setClockPulseDuration(pulseDuration);
    testOtherClockDriver_->setClockPulseValue("2");

	QString expectedOutput(
		"<ipxact:otherClockDriver clockName=\"\">"
    		"<ipxact:clockPeriod></ipxact:clockPeriod>"
	    	"<ipxact:clockPulseOffset units=\"ns\">1</ipxact:clockPulseOffset>"
		    "<ipxact:clockPulseValue>2</ipxact:clockPulseValue>"
    		"<ipxact:clockPulseDuration units=\"ps\">3</ipxact:clockPulseDuration>"
		"</ipxact:otherClockDriver>"
		);

	OtherClockDriverWriter otherClockDriverWriter;
	otherClockDriverWriter.writeOtherClockDriver(xmlStreamWriter, testOtherClockDriver_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_OtherClockDriverWriter)

#include "tst_OtherClockDriverWriter.moc"
