//-----------------------------------------------------------------------------
// File: tst_RemapStateReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Unit test for class RemapStateReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/RemapStateReader.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include <QtTest>

class tst_RemapStateReader : public QObject
{
    Q_OBJECT

public:
    tst_RemapStateReader();

private slots:

    void readSimpleRemapState();
    void readRemapPorts();
};

//-----------------------------------------------------------------------------
// Function: tst_RemapStateReader::tst_RemapStateReader()
//-----------------------------------------------------------------------------
tst_RemapStateReader::tst_RemapStateReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateReader::readSimpleRemapState()
//-----------------------------------------------------------------------------
void tst_RemapStateReader::readSimpleRemapState()
{
    QString documentContent(
        "<ipxact:remapState>"
            "<ipxact:name>testRemapState</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:remapState>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode remapStateNode = document.firstChildElement("ipxact:remapState");

    RemapStateReader remapStateReader;
    QSharedPointer<RemapState> testRemapState = remapStateReader.createRemapStateFrom(remapStateNode);

    QCOMPARE(testRemapState->name(), QString("testRemapState"));
    QCOMPARE(testRemapState->displayName(), QString("testDisplay"));
    QCOMPARE(testRemapState->description(), QString("testDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateReader::readRemapPorts()
//-----------------------------------------------------------------------------
void tst_RemapStateReader::readRemapPorts()
{
    QString documentContent(
        "<ipxact:remapState>"
            "<ipxact:name>testRemapState</ipxact:name>"
            "<ipxact:remapPorts>"
                "<ipxact:remapPort portRef=\"referencedPort\">"
                    "<ipxact:value>newValue</ipxact:value>"
                "</ipxact:remapPort>"
                "<ipxact:remapPort portRef=\"Fuu\">"
                    "<ipxact:portIndex>Mugen</ipxact:portIndex>"
                    "<ipxact:value>Jin</ipxact:value>"
                "</ipxact:remapPort>"
            "</ipxact:remapPorts>"
        "</ipxact:remapState>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode remapStateNode = document.firstChildElement("ipxact:remapState");

    RemapStateReader remapStateReader;
    QSharedPointer<RemapState> testRemapState = remapStateReader.createRemapStateFrom(remapStateNode);

    QCOMPARE(testRemapState->name(), QString("testRemapState"));

    QCOMPARE(testRemapState->getRemapPorts()->size(), 2);
    QCOMPARE(testRemapState->getRemapPorts()->first()->getPortNameRef(), QString("referencedPort"));
    QCOMPARE(testRemapState->getRemapPorts()->first()->getPortIndex(), QString(""));
    QCOMPARE(testRemapState->getRemapPorts()->first()->getValue(), QString("newValue"));

    QCOMPARE(testRemapState->getRemapPorts()->last()->getPortNameRef(), QString("Fuu"));
    QCOMPARE(testRemapState->getRemapPorts()->last()->getPortIndex(), QString("Mugen"));
    QCOMPARE(testRemapState->getRemapPorts()->last()->getValue(), QString("Jin"));
}

QTEST_APPLESS_MAIN(tst_RemapStateReader)

#include "tst_RemapStateReader.moc"
