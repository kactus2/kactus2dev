//-----------------------------------------------------------------------------
// File: tst_ChoiceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Unit test for class ChoiceWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ChoiceWriter.h>

#include <QtTest>

class tst_ChoiceWriter : public QObject
{
    Q_OBJECT

public:
	tst_ChoiceWriter();

private slots:
	void init();
	void cleanup();

	void testWriteChoiceName();
	void testWriteEnum();
	void testWriteEnum2();

private:

	QSharedPointer<Choice> testChoice_;
};

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::tst_ChoiceWriter()
//-----------------------------------------------------------------------------
tst_ChoiceWriter::tst_ChoiceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::init()
//-----------------------------------------------------------------------------
void tst_ChoiceWriter::init()
{
	testChoice_ = QSharedPointer<Choice>(new Choice());
	testChoice_->setName("testChoice");
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ChoiceWriter::cleanup()
{
	testChoice_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::testWriteChoiceName()
//-----------------------------------------------------------------------------
void tst_ChoiceWriter::testWriteChoiceName()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testChoice_->setName("Mikko");

	QString expectedOutput(
		"<ipxact:choice>"
		"<ipxact:name>Mikko</ipxact:name>"
		"</ipxact:choice>"
		);

	ChoiceWriter ChoiceWriter;
	ChoiceWriter.writeChoice(xmlStreamWriter, testChoice_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::testWriteEnum()
//-----------------------------------------------------------------------------
void tst_ChoiceWriter::testWriteEnum()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Enumeration> enumr(new Enumeration());
	enumr->setValue("64");
	enumr->setText("64 bits");
	enumr->setHelp("ei auteta");

	QSharedPointer<QList<QSharedPointer<Enumeration> > > enumrs = testChoice_->enumerations();
	enumrs->append(enumr);

	QString expectedOutput(
		"<ipxact:choice>"
		"<ipxact:name>testChoice</ipxact:name>"
		"<ipxact:enumeration text=\"64 bits\" help=\"ei auteta\">64</ipxact:enumeration>"
		"</ipxact:choice>"
		);

	ChoiceWriter choiceWriter;
	choiceWriter.writeChoice(xmlStreamWriter, testChoice_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceWriter::testWriteEnum2()
//-----------------------------------------------------------------------------
void tst_ChoiceWriter::testWriteEnum2()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Enumeration> enumr(new Enumeration());
	enumr->setValue("64");
	enumr->setText("64 bits");
	enumr->setHelp("ei auteta");

	QSharedPointer<Enumeration> enumr2(new Enumeration());
	enumr2->setValue("32");
	enumr2->setText("32 bits");
	enumr2->setHelp("ei auteta viel‰k‰‰n");

	QSharedPointer<QList<QSharedPointer<Enumeration> > > enumrs = testChoice_->enumerations();
	enumrs->append(enumr);
	enumrs->append(enumr2);

	QString expectedOutput(
		"<ipxact:choice>"
		"<ipxact:name>testChoice</ipxact:name>"
		"<ipxact:enumeration text=\"64 bits\" help=\"ei auteta\">64</ipxact:enumeration>"
		"<ipxact:enumeration text=\"32 bits\" help=\"ei auteta viel‰k‰‰n\">32</ipxact:enumeration>"
		"</ipxact:choice>"
		);

	ChoiceWriter choiceWriter;
	choiceWriter.writeChoice(xmlStreamWriter, testChoice_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ChoiceWriter)

#include "tst_ChoiceWriter.moc"
