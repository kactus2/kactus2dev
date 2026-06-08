//-----------------------------------------------------------------------------
// File: tst_AccessHandleReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Unit test for class AccessHandleReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AccessHandleReader.h>

#include <IPXACTmodels/Component/AccessHandle.h>
#include <IPXACTmodels/Component/Slice.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_AccessHandleReader : public QObject
{
    Q_OBJECT

public:
    tst_AccessHandleReader();

private slots:

    void readSimple14();
    void readSimple22();
    void readNonIndexedLeaf14();
    void readIndexed14();
    void readLeaf14();
    void readSliced22();
    void readPort22();
};

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::tst_AccessHandleReader()
//-----------------------------------------------------------------------------
tst_AccessHandleReader::tst_AccessHandleReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readSimple14()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readSimple14()
{
    QString doc(
        "<ipxact:accessHandle force=\"true\">" // simpleAccessHandle should not contain force
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                "</ipxact:pathSegment>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment2</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>" // should not contain slices
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>"
                            "<ipxact:pathSegmentName>segment3</ipxact:pathSegmentName>"
                        "</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>" // should not contain indices
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>" // only std22 has vendor extensions
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto simpleAccessHandle14 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Bank, Document::Revision::Std14);

    QCOMPARE(simpleAccessHandle14->getForce(), QString(""));
    QCOMPARE(simpleAccessHandle14->getViewReferences().size(), 2);
    QCOMPARE(simpleAccessHandle14->getViewReferences().first(), QString("view1"));
    QCOMPARE(simpleAccessHandle14->getViewReferences().at(1), QString("view2"));
    QCOMPARE(simpleAccessHandle14->getPathSegments()->size(), 2);
    QCOMPARE(simpleAccessHandle14->getPathSegments()->at(0)->name_, QString("segment1"));
    QCOMPARE(simpleAccessHandle14->getPathSegments()->at(1)->name_, QString("segment2"));
    QCOMPARE(simpleAccessHandle14->getPathSegments()->at(1)->indices_.at(0), QString("0"));
    QCOMPARE(simpleAccessHandle14->getPathSegments()->at(1)->indices_.at(1), QString("1"));
    QCOMPARE(simpleAccessHandle14->getSlices()->size(), 0);
    QCOMPARE(simpleAccessHandle14->getIndices().size(), 0);
    QCOMPARE(simpleAccessHandle14->getVendorExtensions()->size(), 0);


}

void tst_AccessHandleReader::readSimple22()
{
    QString doc(
        "<ipxact:accessHandle force=\"true\">" // simpleAccessHandle should not contain force
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>"
                "<ipxact:pathSegment>segment1</ipxact:pathSegment>"
                "<ipxact:pathSegment>segment2</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>" // should not contain slices
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>segment3</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>" // should not contain indices
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>" // only std22 has vendor extensions
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto simpleAccessHandle22 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Bank, Document::Revision::Std22);

    QCOMPARE(simpleAccessHandle22->getForce(), QString(""));
    QCOMPARE(simpleAccessHandle22->getViewReferences().size(), 2);
    QCOMPARE(simpleAccessHandle22->getViewReferences().first(), QString("view1"));
    QCOMPARE(simpleAccessHandle22->getViewReferences().at(1), QString("view2"));
    QCOMPARE(simpleAccessHandle22->getPathSegments()->size(), 2);
    QCOMPARE(simpleAccessHandle22->getPathSegments()->at(0)->name_, QString("segment1"));
    QCOMPARE(simpleAccessHandle22->getPathSegments()->at(1)->name_, QString("segment2"));
    QCOMPARE(simpleAccessHandle22->getSlices()->size(), 0);
    QCOMPARE(simpleAccessHandle22->getIndices().size(), 0);
    QCOMPARE(simpleAccessHandle22->getVendorExtensions()->size(), 1);
    QCOMPARE(simpleAccessHandle22->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readViewReferences()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readNonIndexedLeaf14()
{   
    QString doc(
        "<ipxact:accessHandle force=\"true\">"
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>" // should not contain path segments
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                "</ipxact:pathSegment>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment2</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>"
                            "<ipxact:pathSegmentName>segment3</ipxact:pathSegmentName>"
                        "</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>" // should not contain indices
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>" // should not contain vendor extensions
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto nonIndexedLeaf14 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::AddressBlock, Document::Revision::Std14);

    QCOMPARE(nonIndexedLeaf14->getForce(), QString("true"));
    QCOMPARE(nonIndexedLeaf14->getViewReferences().size(), 2);
    QCOMPARE(nonIndexedLeaf14->getViewReferences().first(), QString("view1"));
    QCOMPARE(nonIndexedLeaf14->getViewReferences().at(1), QString("view2"));
    QCOMPARE(nonIndexedLeaf14->getPathSegments()->size(), 0);
    QCOMPARE(nonIndexedLeaf14->getSlices()->size(), 1);
    QCOMPARE(nonIndexedLeaf14->getSlices()->first()->getLeft(), QString("7"));
    QCOMPARE(nonIndexedLeaf14->getSlices()->first()->getRight(), QString("0"));
    QCOMPARE(nonIndexedLeaf14->getSlices()->first()->pathSegments_->size(), 1);
    QCOMPARE(nonIndexedLeaf14->getSlices()->first()->pathSegments_->first()->name_, QString("segment3"));
    QCOMPARE(nonIndexedLeaf14->getIndices().size(), 0);
    QCOMPARE(nonIndexedLeaf14->getVendorExtensions()->size(), 0);
}

void tst_AccessHandleReader::readIndexed14()
{
    QString doc(
        "<ipxact:accessHandle force=\"true\">" // Should not contain force
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                "</ipxact:pathSegment>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment2</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>" // should not contain slices
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>"
                            "<ipxact:pathSegmentName>segment3</ipxact:pathSegmentName>"
                        "</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>"
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>" // should not contain vendor extensions
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto indexed14 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Register, Document::Revision::Std14);

    QCOMPARE(indexed14->getForce(), QString(""));
    QCOMPARE(indexed14->getViewReferences().size(), 2);
    QCOMPARE(indexed14->getViewReferences().first(), QString("view1"));
    QCOMPARE(indexed14->getViewReferences().at(1), QString("view2"));
    QCOMPARE(indexed14->getPathSegments()->size(), 2);
    QCOMPARE(indexed14->getPathSegments()->at(0)->name_, QString("segment1"));
    QCOMPARE(indexed14->getPathSegments()->at(1)->name_, QString("segment2"));
    QCOMPARE(indexed14->getPathSegments()->at(1)->indices_.at(0), QString("0"));
    QCOMPARE(indexed14->getPathSegments()->at(1)->indices_.at(1), QString("1"));
    QCOMPARE(indexed14->getIndices().size(), 1);
    QCOMPARE(indexed14->getIndices().first(), QString("3"));
    QCOMPARE(indexed14->getVendorExtensions()->size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readLeaf14()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readLeaf14()
{
    QString doc(
        "<ipxact:accessHandle force=\"false\">"
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>" // should not contain path segments
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                "</ipxact:pathSegment>"
                "<ipxact:pathSegment>"
                    "<ipxact:pathSegmentName>segment2</ipxact:pathSegmentName>"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>"
                            "<ipxact:pathSegmentName>segment3</ipxact:pathSegmentName>"
                        "</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>"
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>" // should not contain vendor extensions
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto leaf14 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Port, Document::Revision::Std14);

    QCOMPARE(leaf14->getForce(), QString("false"));
    QCOMPARE(leaf14->getViewReferences().size(), 2);
    QCOMPARE(leaf14->getViewReferences().first(), QString("view1"));
    QCOMPARE(leaf14->getViewReferences().at(1), QString("view2"));
    QCOMPARE(leaf14->getPathSegments()->size(), 0);
    QCOMPARE(leaf14->getSlices()->size(), 1);
    QCOMPARE(leaf14->getSlices()->first()->getLeft(), QString("7"));
    QCOMPARE(leaf14->getSlices()->first()->getRight(), QString("0"));
    QCOMPARE(leaf14->getSlices()->first()->pathSegments_->size(), 1);
    QCOMPARE(leaf14->getSlices()->first()->pathSegments_->first()->name_, QString("segment3"));
    QCOMPARE(leaf14->getIndices().size(), 1);
    QCOMPARE(leaf14->getIndices().first(), QString("3"));
    QCOMPARE(leaf14->getVendorExtensions()->size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readSliced22()
{
    QString doc(
        "<ipxact:accessHandle force=\"false\">"
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>" // should not contain path segments
                "<ipxact:pathSegment>segment1</ipxact:pathSegment>"
                "<ipxact:pathSegment>segment2</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>segment3</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>" // should not contain indices
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto sliced22 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Field, Document::Revision::Std22);

    QCOMPARE(sliced22->getForce(), QString("false"));
    QCOMPARE(sliced22->getViewReferences().size(), 2);
    QCOMPARE(sliced22->getViewReferences().first(), QString("view1"));
    QCOMPARE(sliced22->getViewReferences().at(1), QString("view2"));
    QCOMPARE(sliced22->getPathSegments()->size(), 0);
    QCOMPARE(sliced22->getSlices()->size(), 1);
    QCOMPARE(sliced22->getSlices()->first()->getLeft(), QString("7"));
    QCOMPARE(sliced22->getSlices()->first()->getRight(), QString("0"));
    QCOMPARE(sliced22->getSlices()->first()->pathSegments_->size(), 1);
    QCOMPARE(sliced22->getSlices()->first()->pathSegments_->first()->name_, QString("segment3"));
    QCOMPARE(sliced22->getIndices().size(), 0);
    QCOMPARE(sliced22->getVendorExtensions()->size(), 1);
    QCOMPARE(sliced22->getVendorExtensions()->first()->type(), QString("testExtension"));
}

void tst_AccessHandleReader::readPort22()
{
    QString doc(
        "<ipxact:accessHandle force=\"true\">"
            "<ipxact:viewRef>view1</ipxact:viewRef>"
            "<ipxact:viewRef>view2</ipxact:viewRef>"
            "<ipxact:pathSegments>" // should not contain path segments
                "<ipxact:pathSegment>segment1</ipxact:pathSegment>"
                "<ipxact:pathSegment>segment2</ipxact:pathSegment>"
            "</ipxact:pathSegments>"
            "<ipxact:slices>" 
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>segment3</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:slice>"
            "</ipxact:slices>"
            "<ipxact:indices>"
                "<ipxact:index>3</ipxact:index>"
            "</ipxact:indices>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto port22 = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), AccessHandle::ElementType::Port, Document::Revision::Std22);

    QCOMPARE(port22->getForce(), QString("true"));
    QCOMPARE(port22->getViewReferences().size(), 2);
    QCOMPARE(port22->getViewReferences().first(), QString("view1"));
    QCOMPARE(port22->getViewReferences().at(1), QString("view2"));
    QCOMPARE(port22->getPathSegments()->size(), 0);
    QCOMPARE(port22->getSlices()->size(), 1);
    QCOMPARE(port22->getSlices()->first()->getLeft(), QString("7"));
    QCOMPARE(port22->getSlices()->first()->getRight(), QString("0"));
    QCOMPARE(port22->getSlices()->first()->pathSegments_->size(), 1);
    QCOMPARE(port22->getSlices()->first()->pathSegments_->first()->name_, QString("segment3"));
    QCOMPARE(port22->getIndices().size(), 1);
    QCOMPARE(port22->getIndices().first(), QString("3"));
    QCOMPARE(port22->getVendorExtensions()->size(), 1);
    QCOMPARE(port22->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_AccessHandleReader)
#include "tst_AccessHandleReader.moc"
