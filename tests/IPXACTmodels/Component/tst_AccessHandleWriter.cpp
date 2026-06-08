//-----------------------------------------------------------------------------
// File: tst_AccessHandleWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 02.06.2026
//
// Description:
// Unit test for class AccessHandleWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AccessHandleWriter.h>
#include <IPXACTmodels/Component/AccessHandle.h>
#include <IPXACTmodels/Component/Slice.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QXmlStreamWriter>
#include <QDomDocument>

class tst_AccessHandleWriter : public QObject
{
    Q_OBJECT

public:
    tst_AccessHandleWriter();

private slots:

    void init();
    void cleanup();

    void writeForce();
    void writeIndices();
    void writeViewReferences();
    void writeSlices();
    void writePathSegments();
    void writeVendorExtensions();

private:
    QSharedPointer<AccessHandle> accessHandle_;
};

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::tst_AccessHandleWriter()
//-----------------------------------------------------------------------------
tst_AccessHandleWriter::tst_AccessHandleWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::init()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::init()
{
    accessHandle_ = QSharedPointer<AccessHandle>(new AccessHandle());
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::cleanup()
{
    accessHandle_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writeForce()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writeForce()
{
    accessHandle_->setForce(true);

    QString expectedOutput(
        "<ipxact:accessHandle force=\"true\"/>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);

    // Test with force = false
    accessHandle_->setForce(false);

    QString expectedOutput2(
        "<ipxact:accessHandle force=\"false\"/>"
    );

    output.clear();
    QXmlStreamWriter writer2(&output);

    AccessHandleWriter::writeAccessHandle(writer2, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput2);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writeIndices()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writeIndices()
{
    QStringList indices;
    indices.append("0");
    accessHandle_->setIndices(indices);

    QString expectedOutput(
        "<ipxact:accessHandle>"
            "<ipxact:indices>"
                "<ipxact:index>0</ipxact:index>"
            "</ipxact:indices>"
        "</ipxact:accessHandle>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);

    // Test with multiple indices
    indices.clear();
    indices.append("0");
    indices.append("1");
    indices.append("2");
    accessHandle_->setIndices(indices);

    QString expectedOutput2(
        "<ipxact:accessHandle>"
            "<ipxact:indices>"
                "<ipxact:index>0</ipxact:index>"
                "<ipxact:index>1</ipxact:index>"
                "<ipxact:index>2</ipxact:index>"
            "</ipxact:indices>"
        "</ipxact:accessHandle>"
    );

    output.clear();
    QXmlStreamWriter writer2(&output);

    AccessHandleWriter::writeAccessHandle(writer2, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput2);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writeViewReferences()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writeViewReferences()
{
    QStringList viewRefs;
    viewRefs.append("view1");
    viewRefs.append("view2");
    viewRefs.append("view3");
    accessHandle_->setViewReferences(viewRefs);

    QString expectedOutput(
        "<ipxact:accessHandle>"
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:viewRef>view3</ipxact:viewRef>"
        "</ipxact:accessHandle>"
    );

    QString output;
    QXmlStreamWriter writer(&output);;

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writeSlices()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writeSlices()
{
    QSharedPointer<Slice> slice1(new Slice());
    slice1->setLeft("7");
    slice1->setRight("0");
    accessHandle_->getSlices()->append(slice1);

    QString expectedOutput(
        "<ipxact:accessHandle>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                "</ipxact:slice>"
            "</ipxact:slices>"
        "</ipxact:accessHandle>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);

    // Test with multiple slices
    accessHandle_->getSlices()->clear();
    QSharedPointer<Slice> slice2(new Slice());
    slice2->setLeft("15");
    slice2->setRight("8");
    QSharedPointer<Slice> slice3(new Slice());
    slice3->setLeft("23");
    slice3->setRight("16");
    accessHandle_->getSlices()->append(slice1);
    accessHandle_->getSlices()->append(slice2);
    accessHandle_->getSlices()->append(slice3);

    QString expectedOutput2(
        "<ipxact:accessHandle>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                "</ipxact:slice>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>15</ipxact:left>"
                        "<ipxact:right>8</ipxact:right>"
                    "</ipxact:range>"
                "</ipxact:slice>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>23</ipxact:left>"
                        "<ipxact:right>16</ipxact:right>"
                    "</ipxact:range>"
                "</ipxact:slice>"
            "</ipxact:slices>"
        "</ipxact:accessHandle>"
    );

    output.clear();
    QXmlStreamWriter writer2(&output);

    AccessHandleWriter::writeAccessHandle(writer2, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput2);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writePathSegments()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writePathSegments()
{
    QSharedPointer<PathSegment> segment1(new PathSegment());
    segment1->name_ = "segment1";
    accessHandle_->getPathSegments()->append(segment1);

    // Std14 and 22 have different pathSegment elements, name directly in pathSegment for 22 and no indices

    QString expectedOutput(
        "<ipxact:accessHandle>"
            "<ipxact:pathSegments>"
                "<ipxact:pathSegment>segment1</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
        "</ipxact:accessHandle>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);

    // Test with path segments that have indices
    accessHandle_->getPathSegments()->clear();
    QSharedPointer<PathSegment> segment2(new PathSegment());
    segment2->name_ = "segment1";
    segment2->indices_.append("0");

    QSharedPointer<PathSegment> segment3(new PathSegment());
    segment3->name_ = "segment2";
    segment3->indices_.append("1");
    segment3->indices_.append("2");

    accessHandle_->getPathSegments()->append(segment2);
    accessHandle_->getPathSegments()->append(segment3);

    QString expectedOutput2(
        "<ipxact:accessHandle>"
            "<ipxact:pathSegments>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment2</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>1</ipxact:index>"
                        "<ipxact:index>2</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
        "</ipxact:accessHandle>"
    );

    output.clear();
    QXmlStreamWriter writer2(&output);

    AccessHandleWriter::writeAccessHandle(writer2, accessHandle_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput2);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AccessHandleWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter writer(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    accessHandle_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:accessHandle>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    AccessHandleWriter::writeAccessHandle(writer, accessHandle_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_AccessHandleWriter)
#include "tst_AccessHandleWriter.moc"
