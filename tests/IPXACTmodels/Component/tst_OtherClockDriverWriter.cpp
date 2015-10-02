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

#include <IPXACTmodels/Component/OtherClockDriverWriter.h>
#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/GenericVendorExtension.h>

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
		"<ipxact:clockPeriod units=\"ns\">0</ipxact:clockPeriod>"
		"<ipxact:clockPulseOffset units=\"ns\">0</ipxact:clockPulseOffset>"
		"<ipxact:clockPulseValue>0</ipxact:clockPulseValue>"
		"<ipxact:clockPulseDuration units=\"ns\">0</ipxact:clockPulseDuration>"
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

	General::ClockStruct* cs = new General::ClockStruct();
	cs->timeUnit_ = General::PS;
	cs->attributes_.insert("joku","jotain");
	cs->value_ = 6;

	testOtherClockDriver_->setClockPeriod(cs);

	QString expectedOutput(
		"<ipxact:otherClockDriver>"
		"<ipxact:clockPeriod units=\"ps\" joku=\"jotain\">6</ipxact:clockPeriod>"
		"<ipxact:clockPulseOffset units=\"ns\">0</ipxact:clockPulseOffset>"
		"<ipxact:clockPulseValue>0</ipxact:clockPulseValue>"
		"<ipxact:clockPulseDuration units=\"ns\">0</ipxact:clockPulseDuration>"
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

	General::ClockStruct* cpo = new General::ClockStruct();
	cpo->value_ = 1;
	cpo->timeUnit_ = General::NS;
	testOtherClockDriver_->setClockPulseOffset( cpo );

	General::ClockStruct* cpd = new General::ClockStruct();
	cpd->value_ = 3;
	cpd->timeUnit_ = General::PS;
	cpd->attributes_.insert("joku","jotain");
	testOtherClockDriver_->setClockPulseDuration( cpd );

	General::ClockPulseValue* cpv = new General::ClockPulseValue();
	cpv->value_ = 2;
	testOtherClockDriver_->setClockPulseValue( cpv );

	QString expectedOutput(
		"<ipxact:otherClockDriver>"
		"<ipxact:clockPeriod units=\"ns\">0</ipxact:clockPeriod>"
		"<ipxact:clockPulseOffset units=\"ns\">1</ipxact:clockPulseOffset>"
		"<ipxact:clockPulseValue>2</ipxact:clockPulseValue>"
		"<ipxact:clockPulseDuration units=\"ps\" joku=\"jotain\">3</ipxact:clockPulseDuration>"
		"</ipxact:otherClockDriver>"
		);

	OtherClockDriverWriter otherClockDriverWriter;
	otherClockDriverWriter.writeOtherClockDriver(xmlStreamWriter, testOtherClockDriver_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_OtherClockDriverWriter)

#include "tst_OtherClockDriverWriter.moc"
