//-----------------------------------------------------------------------------
// File: tst_ChoiceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Unit test for class ChoiceReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ChoiceReader.h>

#include <QtTest>

class tst_ChoiceReader : public QObject
{
    Q_OBJECT

public:
    tst_ChoiceReader();

private slots:

	void testReadSimpleChoice();
	void testReadEnumeration();
	void testReadEnumeration2();
};

//-----------------------------------------------------------------------------
// Function: tst_ChoiceReader::tst_ChoiceReader()
//-----------------------------------------------------------------------------
tst_ChoiceReader::tst_ChoiceReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceReader::testReadSimpleChoice()
//-----------------------------------------------------------------------------
void tst_ChoiceReader::testReadSimpleChoice()
{
    QString documentContent(
        "<ipxact:choice>"
            "<ipxact:name>Choice</ipxact:name>"
        "</ipxact:choice>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode ChoiceNode = document.firstChildElement("ipxact:choice");

    ChoiceReader choiceReader;
    QSharedPointer<Choice> testChoice = choiceReader.createChoiceFrom(ChoiceNode);

    QCOMPARE(testChoice->name(), QString("Choice"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceReader::testReadEnumeration()
//-----------------------------------------------------------------------------
void tst_ChoiceReader::testReadEnumeration()
{
	QString documentContent(
		"<ipxact:choice>"
		"<ipxact:name>Choice</ipxact:name>"
		"<ipxact:enumeration text=\"64 bits\" help=\"ei auteta\">64</ipxact:enumeration>"
		"</ipxact:choice>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ChoiceNode = document.firstChildElement("ipxact:choice");

	ChoiceReader choiceReader;
	QSharedPointer<Choice> testChoice = choiceReader.createChoiceFrom(ChoiceNode);

	QCOMPARE(testChoice->name(), QString("Choice"));
	QCOMPARE(testChoice->enumerations()->size(), 1);
	QCOMPARE(testChoice->enumerations()->first()->getValue(), QString("64"));
	QCOMPARE(testChoice->enumerations()->first()->getText(), QString("64 bits"));
	QCOMPARE(testChoice->enumerations()->first()->getHelp(), QString("ei auteta"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceReader::testReadEnumeration2()
//-----------------------------------------------------------------------------
void tst_ChoiceReader::testReadEnumeration2()
{
	QString documentContent(
		"<ipxact:choice>"
		"<ipxact:name>Choice</ipxact:name>"
		"<ipxact:enumeration text=\"64 bits\" help=\"ei auteta\">64</ipxact:enumeration>"
		"<ipxact:enumeration text=\"32 bits\" help=\"ei auteta viel‰k‰‰n\">32</ipxact:enumeration>"
		"</ipxact:choice>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ChoiceNode = document.firstChildElement("ipxact:choice");

	ChoiceReader choiceReader;
	QSharedPointer<Choice> testChoice = choiceReader.createChoiceFrom(ChoiceNode);

	QCOMPARE(testChoice->name(), QString("Choice"));
	QCOMPARE(testChoice->enumerations()->size(), 2);
	QCOMPARE(testChoice->enumerations()->first()->getValue(), QString("64"));
	QCOMPARE(testChoice->enumerations()->first()->getText(), QString("64 bits"));
	QCOMPARE(testChoice->enumerations()->first()->getHelp(), QString("ei auteta"));
	QCOMPARE(testChoice->enumerations()->last()->getValue(), QString("32"));
	QCOMPARE(testChoice->enumerations()->last()->getText(), QString("32 bits"));
	QCOMPARE(testChoice->enumerations()->last()->getHelp(), QString("ei auteta viel‰k‰‰n"));
}

QTEST_APPLESS_MAIN(tst_ChoiceReader)

#include "tst_ChoiceReader.moc"
