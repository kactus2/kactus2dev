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

    void testReadFieldSlices();

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

//-----------------------------------------------------------------------------
// Function: tst_ModeReader::testReadFieldSlices()
//-----------------------------------------------------------------------------
void tst_ModeReader::testReadFieldSlices()
{
        QString documentContent(
        "<ipxact:mode>"
            "<ipxact:name>testMode</ipxact:name>"
            "<ipxact:fieldSlice>"
                "<ipxact:name>testSlice</ipxact:name>"
                "<ipxact:displayName>sliceDisplay</ipxact:displayName>"
                "<ipxact:shortDescription>sliceShortDescription</ipxact:shortDescription>"
                "<ipxact:description>sliceDescription</ipxact:description>"
                "<ipxact:memoryMapRef memoryMapRef=\"testMMRef\"/>"
                "<ipxact:addressBlockRef addressBlockRef=\"testABRef\"/>"
                "<ipxact:registerRef registerRef=\"testRegisterRef\"/>"
                "<ipxact:fieldRef fieldRef=\"testFieldRef\"/>"
                "<ipxact:range>"
                    "<ipxact:left>7</ipxact:left>"
                    "<ipxact:right>0</ipxact:right>"
                "</ipxact:range>"
            "</ipxact:fieldSlice>"
             "<ipxact:fieldSlice>"
                 "<ipxact:name>lastSlice</ipxact:name>"
             "</ipxact:fieldSlice>"
        "</ipxact:mode>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode modeNode = document.firstChildElement("ipxact:mode");

    QSharedPointer<Mode> testMode = ModeReader::createModeFrom(modeNode);

    QCOMPARE(testMode->getFieldSlices()->count(), 2);
    auto fieldSlice = testMode->getFieldSlices()->first();

    QCOMPARE(fieldSlice->name(), QString("testSlice"));
    QCOMPARE(fieldSlice->displayName(), QString("sliceDisplay"));
    QCOMPARE(fieldSlice->shortDescription(), QString("sliceShortDescription"));
    QCOMPARE(fieldSlice->description(), QString("sliceDescription"));
    QCOMPARE(fieldSlice->getReference(FieldReference::MEMORY_MAP)->reference_, QString("testMMRef"));
    QCOMPARE(fieldSlice->getReference(FieldReference::ADDRESS_BLOCK)->reference_, QString("testABRef"));
    QCOMPARE(fieldSlice->getReference(FieldReference::REGISTER)->reference_, QString("testRegisterRef"));
    QCOMPARE(fieldSlice->getReference(FieldReference::FIELD)->reference_, QString("testFieldRef"));
    QCOMPARE(fieldSlice->getLeft(), QString("7"));
    QCOMPARE(fieldSlice->getRight(), QString("0"));
}


QTEST_APPLESS_MAIN(tst_ModeReader)

#include "tst_ModeReader.moc"
