//-----------------------------------------------------------------------------
// File: tst_MemoryArrayReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.07.2023
//
// Description:
// Unit test for class MemoryArrayReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryArrayReader.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <QDomDocument>

#include <QtTest>

class tst_MemoryArrayReader : public QObject
{
    Q_OBJECT

public:
    tst_MemoryArrayReader();

private slots:

    void readSingleDimension();
    void readMultipleDimension();
    void readStride();
    void readBitStride();
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayReader::tst_MemoryArrayReader()
//-----------------------------------------------------------------------------
tst_MemoryArrayReader::tst_MemoryArrayReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayReader::readSingleDimension()
//-----------------------------------------------------------------------------
void tst_MemoryArrayReader::readSingleDimension()
{
    QString documentContent(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
        "</ipxact:array>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    auto array = MemoryArrayReader::createMemoryArrayFrom(document.firstChildElement("ipxact:array"), false);

    QCOMPARE(array->getDimensions()->size(), 1);
    QCOMPARE(array->getDimensions()->first()->dimension_, "8");
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayReader::readMultipleDimension()
//-----------------------------------------------------------------------------
void tst_MemoryArrayReader::readMultipleDimension()
{
    QString documentContent(
        "<ipxact:array>"
            "<ipxact:dim indexVar=\"first\">8</ipxact:dim>"
            "<ipxact:dim indexVar=\"second\">16</ipxact:dim>"
        "</ipxact:array>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    auto array = MemoryArrayReader::createMemoryArrayFrom(document.firstChildElement("ipxact:array"), false);

    auto dimensions = array->getDimensions();

    QCOMPARE(dimensions->size(), 2);
    QCOMPARE(dimensions->first()->dimension_, "8");
    QCOMPARE(dimensions->first()->indexVar_, "first");
    QCOMPARE(dimensions->last()->dimension_, "16");
    QCOMPARE(dimensions->last()->indexVar_, "second");
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayReader::readStride()
//-----------------------------------------------------------------------------
void tst_MemoryArrayReader::readStride()
{
    QString documentContent(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
            "<ipxact:stride>16</ipxact:stride>"
        "</ipxact:array>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    auto array = MemoryArrayReader::createMemoryArrayFrom(document.firstChildElement("ipxact:array"), false);

    QCOMPARE(array->getDimensions()->size(), 1);
    QCOMPARE(array->getDimensions()->first()->dimension_, "8");
    QCOMPARE(array->getStride(), "16");
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayReader::readBitStride()
//-----------------------------------------------------------------------------
void tst_MemoryArrayReader::readBitStride()
{
    QString documentContent(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
            "<ipxact:bitStride>16</ipxact:bitStride>"
        "</ipxact:array>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    auto array = MemoryArrayReader::createMemoryArrayFrom(document.firstChildElement("ipxact:array"), true);

    QCOMPARE(array->getDimensions()->size(), 1);
    QCOMPARE(array->getDimensions()->first()->dimension_, "8");
    QCOMPARE(array->getStride(), "16");
}


QTEST_APPLESS_MAIN(tst_MemoryArrayReader)

#include "tst_MemoryArrayReader.moc"
