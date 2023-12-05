//-----------------------------------------------------------------------------
// File: tst_MemoryArrayWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.07.2023
//
// Description:
// Unit test for class MemoryArrayWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryArrayWriter.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <QXmlStreamWriter>

#include <QtTest>

class tst_MemoryArrayWriter : public QObject
{
    Q_OBJECT

public:
    tst_MemoryArrayWriter();

private slots:

    void init();
    void cleanup();

    void writeSingleDimension();
    void writeMultipleDimensions();
    void writeStride();
    void writeBitStride();

private:
    QSharedPointer<MemoryArray> memoryArray_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::tst_MemoryArrayWriter()
//-----------------------------------------------------------------------------
tst_MemoryArrayWriter::tst_MemoryArrayWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::init()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::init()
{
    memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray());
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::cleanup()
{
    memoryArray_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::writeSingleDimension()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::writeSingleDimension()
{
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("8");
    
    memoryArray_->getDimensions()->append(dim);

    QString expectedOutput(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
        "</ipxact:array>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    MemoryArrayWriter::writeMemoryArray(writer, memoryArray_, Document::Revision::Std14, false);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::writeMultipleDimensions()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::writeMultipleDimensions()
{
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("8");
    dim->indexVar_ = QString("first");

    QSharedPointer<MemoryArray::Dimension> dim2(new MemoryArray::Dimension);
    dim2->value_ = QString("16");
    dim2->indexVar_ = QString("second");

    memoryArray_->getDimensions()->append(dim);
    memoryArray_->getDimensions()->append(dim2);

    QString expectedOutput(
        "<ipxact:array>"
            "<ipxact:dim indexVar=\"first\">8</ipxact:dim>"
            "<ipxact:dim indexVar=\"second\">16</ipxact:dim>"
        "</ipxact:array>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    MemoryArrayWriter::writeMemoryArray(writer, memoryArray_, Document::Revision::Std14, false);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::writeStride()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::writeStride()
{
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("8");

    memoryArray_->getDimensions()->append(dim);
    memoryArray_->setStride("16");

    QString expectedOutput(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
            "<ipxact:stride>16</ipxact:stride>"
        "</ipxact:array>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    MemoryArrayWriter::writeMemoryArray(writer, memoryArray_, Document::Revision::Std14, false);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayWriter::writeBitStride()
//-----------------------------------------------------------------------------
void tst_MemoryArrayWriter::writeBitStride()
{
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("8");

    memoryArray_->getDimensions()->append(dim);
    memoryArray_->setStride("16");

    QString expectedOutput(
        "<ipxact:array>"
            "<ipxact:dim>8</ipxact:dim>"
            "<ipxact:bitStride>16</ipxact:bitStride>"
        "</ipxact:array>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    MemoryArrayWriter::writeMemoryArray(writer, memoryArray_, Document::Revision::Std14, true);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_MemoryArrayWriter)

#include "tst_MemoryArrayWriter.moc"
