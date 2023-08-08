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

QTEST_APPLESS_MAIN(tst_ModeReader)

#include "tst_ModeReader.moc"
