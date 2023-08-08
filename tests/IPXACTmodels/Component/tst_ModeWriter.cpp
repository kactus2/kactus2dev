//-----------------------------------------------------------------------------
// File: tst_ModeWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2023
//
// Description:
// Unit test for class ModeWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/ModeWriter.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_ModeWriter : public QObject
{
    Q_OBJECT

public:
    tst_ModeWriter();

private slots:

    void testWriteNameGroup();
};

//-----------------------------------------------------------------------------
// Function: tst_ModeWriter::tst_ModeWriter()
//-----------------------------------------------------------------------------
tst_ModeWriter::tst_ModeWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModeWriter::testWriteNameGroup()
//-----------------------------------------------------------------------------
void tst_ModeWriter::testWriteNameGroup()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);


    QSharedPointer<Mode> testMode(new Mode("testMode"));
    testMode->setDisplayName("testDisplay");
    testMode->setShortDescription("testShortDescription");
    testMode->setDescription("testDescription");
    
    ModeWriter::writeMode(xmlStreamWriter, testMode);

    QString expectedOutput(
        "<ipxact:mode>"
            "<ipxact:name>testMode</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>testShortDescription</ipxact:shortDescription>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:mode>"
    );

    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ModeWriter)

#include "tst_ModeWriter.moc"
