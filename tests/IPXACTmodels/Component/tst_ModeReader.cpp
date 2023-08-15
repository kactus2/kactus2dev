//-----------------------------------------------------------------------------
// File: tst_ModeReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Unit test for class ModeReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/ModeReader.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_ModeReader : public QObject
{
    Q_OBJECT

public:
    tst_ModeReader();

private slots:

    void testReadNameGroup();
    void testReadPortSlices();
};

//-----------------------------------------------------------------------------
// Function: tst_ModeReader::tst_ModeReader()
//-----------------------------------------------------------------------------
tst_ModeReader::tst_ModeReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModeReader::testReadNameGroup()
//-----------------------------------------------------------------------------
void tst_ModeReader::testReadNameGroup()
{
    QString documentContent(
        "<ipxact:mode>"
            "<ipxact:name>testMode</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>testShortDescription</ipxact:shortDescription>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:mode>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode modeNode = document.firstChildElement("ipxact:mode");

    QSharedPointer<Mode> testMode = ModeReader::createModeFrom(modeNode);

    QCOMPARE(testMode->name(), QString("testMode"));
    QCOMPARE(testMode->displayName(), QString("testDisplay"));
    QCOMPARE(testMode->shortDescription(), QString("testShortDescription"));
    QCOMPARE(testMode->description(), QString("testDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModeReader::testReadPortSlices()
//-----------------------------------------------------------------------------
void tst_ModeReader::testReadPortSlices()
{
    QString documentContent(
        "<ipxact:mode>"
            "<ipxact:name>testMode</ipxact:name>"
            "<ipxact:portSlice>"
                "<ipxact:name>testSlice</ipxact:name>"
                "<ipxact:displayName>sliceDisplay</ipxact:displayName>"
                "<ipxact:shortDescription>sliceShortDescription</ipxact:shortDescription>"
                "<ipxact:description>sliceDescription</ipxact:description>"
                "<ipxact:portRef portRef=\"testRef\"/>"
                "<ipxact:partSelect>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                "</ipxact:partSelect>"
            "</ipxact:portSlice>"
            "<ipxact:portSlice>"
                "<ipxact:name>lastSlice</ipxact:name>"
            "</ipxact:portSlice>"
        "</ipxact:mode>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode modeNode = document.firstChildElement("ipxact:mode");

    QSharedPointer<Mode> testMode = ModeReader::createModeFrom(modeNode);

    QCOMPARE(testMode->getPortSlices()->count(), 2);
    auto portSlice = testMode->getPortSlices()->first();

    QCOMPARE(portSlice->name(), QString("testSlice"));
    QCOMPARE(portSlice->displayName(), QString("sliceDisplay"));
    QCOMPARE(portSlice->shortDescription(), QString("sliceShortDescription"));
    QCOMPARE(portSlice->description(), QString("sliceDescription"));
    QCOMPARE(portSlice->getPortRef(), QString("testRef"));

    auto partSelect = portSlice->getPartSelect();
    QVERIFY(partSelect != nullptr);

    QCOMPARE(partSelect->getLeftRange(), QString("7"));
    QCOMPARE(partSelect->getRightRange(), QString("0"));
}

QTEST_APPLESS_MAIN(tst_ModeReader)

#include "tst_ModeReader.moc"
