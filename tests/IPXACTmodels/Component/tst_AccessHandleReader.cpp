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

    void readForce();
    void readIndices();
    void readViewReferences();
    void readSlices();
    void readPathSegments();
    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::tst_AccessHandleReader()
//-----------------------------------------------------------------------------
tst_AccessHandleReader::tst_AccessHandleReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readForce()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readForce()
{
    QString doc(
        "<ipxact:accessHandle>"
            "<ipxact:force>true</ipxact:force>"
        "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std22);

    QCOMPARE(accessHandle->getForce(), QString("true"));

    doc =
        "<ipxact:accessHandle>"
            "<ipxact:force>false</ipxact:force>"
        "</ipxact:accessHandle>";

    document.setContent(doc);

    accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std22);

    QCOMPARE(accessHandle->getForce(), QString("false"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readIndices()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readIndices()
{
    QString doc =
        "<ipxact:accessHandle>"
            "<ipxact:indices>"
                "<ipxact:index>0</ipxact:index>"
                "<ipxact:index>1</ipxact:index>"
                "<ipxact:index>2</ipxact:index>"
            "</ipxact:indices>"
        "</ipxact:accessHandle>";

    QDomDocument document;
    document.setContent(doc);
                
    auto accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std22);
    
    QCOMPARE(accessHandle->getIndices().size(), 3);
    QCOMPARE(accessHandle->getIndices().at(0), QString("0"));
    QCOMPARE(accessHandle->getIndices().at(1), QString("1"));
    QCOMPARE(accessHandle->getIndices().at(2), QString("2"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readViewReferences()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readViewReferences()
{   
    QString doc =
        "<ipxact:accessHandle>"
            "<ipxact:viewRefs>"
                "<ipxact:viewRef>view1</ipxact:viewRef>"
                "<ipxact:viewRef>view2</ipxact:viewRef>"
            "</ipxact:viewRefs>"
        "</ipxact:accessHandle>";
            
    QDomDocument document;
    document.setContent(doc);

    auto accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std22);

    QCOMPARE(accessHandle->getViewReferences().size(), 2);
    QCOMPARE(accessHandle->getViewReferences().at(0), QString("view1"));
    QCOMPARE(accessHandle->getViewReferences().at(1), QString("view2"));
}

void tst_AccessHandleReader::readSlices()
{
    QString doc =
        "<ipxact:accessHandle>"
            "<ipxact:slices>"
                "<ipxact:slice>"
                    "<ipxact:range>"
                        "<ipxact:left>7</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:range>"
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
                "</ipxact:slice>"
            "</ipxact:slices>"
        "</ipxact:accessHandle>";

    QDomDocument document;
    document.setContent(doc);

    auto accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std14);

    QCOMPARE(accessHandle->getSlices()->size(), 1);
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->size(), 2);
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->at(0)->name_, QString("segment1"));
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->at(1)->name_, QString("segment2"));
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->at(1)->indices_.size(), 2);
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->at(1)->indices_.at(0), QString("0"));
    QCOMPARE(accessHandle->getSlices()->at(0)->pathSegments_->at(1)->indices_.at(1), QString("1"));
    QCOMPARE(accessHandle->getSlices()->at(0)->getLeft(), QString("7"));
    QCOMPARE(accessHandle->getSlices()->at(0)->getRight(), QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readPathSegments()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readPathSegments()
{
    QString doc =
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
        "</ipxact:accessHandle>";

    QDomDocument document;
    document.setContent(doc);

    auto accessHandle = AccessHandleReader::createAccessHandleFrom(
        document.firstChildElement("ipxact:accessHandle"), Document::Revision::Std14);

    QCOMPARE(accessHandle->getPathSegments()->size(), 2);
    QCOMPARE(accessHandle->getPathSegments()->at(0)->name_, QString("segment1"));
    QCOMPARE(accessHandle->getPathSegments()->at(0)->indices_.size(), 1);
    QCOMPARE(accessHandle->getPathSegments()->at(0)->indices_.at(0), QString("0"));
    QCOMPARE(accessHandle->getPathSegments()->at(1)->name_, QString("segment2"));
    QCOMPARE(accessHandle->getPathSegments()->at(1)->indices_.size(), 2);
    QCOMPARE(accessHandle->getPathSegments()->at(1)->indices_.at(0), QString("1"));
    QCOMPARE(accessHandle->getPathSegments()->at(1)->indices_.at(1), QString("2"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessHandleReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AccessHandleReader::readVendorExtensions()
{
    QString documentContent(
    "<ipxact:accessHandle>"
        "<ipxact:vendorExtensions>"
            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
        "</ipxact:vendorExtensions>"
    "</ipxact:accessHandle>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode accessHandleNode = document.firstChildElement("ipxact:accessHandle");

    QSharedPointer<AccessHandle> testAccessHandle = AccessHandleReader::createAccessHandleFrom(accessHandleNode, Document::Revision::Std22);

    QCOMPARE(testAccessHandle->getVendorExtensions()->size(), 1);
    QCOMPARE(testAccessHandle->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_AccessHandleReader)
#include "tst_AccessHandleReader.moc"
