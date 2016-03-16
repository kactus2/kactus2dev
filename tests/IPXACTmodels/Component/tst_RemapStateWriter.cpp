//-----------------------------------------------------------------------------
// File: tst_RemapStateWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Unit test for class RemapStateWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/RemapStateWriter.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include <QtTest>
#include <QSharedPointer>

class tst_RemapStateWriter : public QObject
{
    Q_OBJECT

public:
    tst_RemapStateWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleRemapState();
    void writeRemapPorts();

private:

    QSharedPointer<RemapState> testRemapState_;
};

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::tst_RemapStateWriter()
//-----------------------------------------------------------------------------
tst_RemapStateWriter::tst_RemapStateWriter():
testRemapState_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::init()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::init()
{
    testRemapState_ = QSharedPointer<RemapState>(new RemapState("testRemapState"));
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::cleanup()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::writeSimpleRemapState()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::writeSimpleRemapState()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:remapState>"
            "<ipxact:name>testRemapState</ipxact:name>"
        "</ipxact:remapState>"
        );

    RemapStateWriter remapStateWriter;
    remapStateWriter.writeRemapState(xmlStreamWriter, testRemapState_);
    QCOMPARE(output, expectedOutput);

    expectedOutput.clear();
    output.clear();

    testRemapState_->setDisplayName("testDisplay");
    testRemapState_->setDescription("testDescription");
    expectedOutput = 
        "<ipxact:remapState>"
            "<ipxact:name>testRemapState</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:remapState>"
        ;

    remapStateWriter.writeRemapState(xmlStreamWriter, testRemapState_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateWriter::writeRemapPorts()
//-----------------------------------------------------------------------------
void tst_RemapStateWriter::writeRemapPorts()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RemapPort> testRemapPort (new RemapPort("referencedPort"));
    testRemapPort->setValue("newValue");

    QSharedPointer<RemapPort> otherRemapPort (new RemapPort("Fuu"));
    otherRemapPort->setPortIndex("Mugen");
    otherRemapPort->setValue("Jin");

    testRemapState_->getRemapPorts()->append(testRemapPort);
    testRemapState_->getRemapPorts()->append(otherRemapPort);

    QString expectedOutput(
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

    RemapStateWriter remapStateWriter;
    remapStateWriter.writeRemapState(xmlStreamWriter, testRemapState_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_RemapStateWriter)

#include "tst_RemapStateWriter.moc"
