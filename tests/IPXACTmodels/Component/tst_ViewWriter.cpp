//-----------------------------------------------------------------------------
// File: tst_ViewWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2015
//
// Description:
// Unit test for class ViewWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ViewWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_ViewWriter : public QObject
{
    Q_OBJECT

public:
    tst_ViewWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testWriteViewNameGroup();
    void testWriteIsPresent();
    void testWriteEnvIdentifiers();
    void testWriteComponentInstantiationRef();
    void testWriteDesignInstantiationRef();
    void testWriteDesignConfigurationInstantiationRef();
    void testWriteVendorExtensions();

private:

    QSharedPointer<View> testView_;
};

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::tst_ViewWriter()
//-----------------------------------------------------------------------------
tst_ViewWriter::tst_ViewWriter():
testView_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_ViewWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ViewWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::init()
//-----------------------------------------------------------------------------
void tst_ViewWriter::init()
{
    testView_ = QSharedPointer<View>(new View("testView", "", ""));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ViewWriter::cleanup()
{
    testView_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteSimpleView()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteViewNameGroup()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    expectedOutput.clear();
    output.clear();

    testView_->setDisplayName("testDisplay");
    expectedOutput = 
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
        "</ipxact:view>"
        ;

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testView_->setDescription("testDescription");
    testView_->setShortDescription("shortDescription");
    expectedOutput = 
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:view>"
        ;

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    expectedOutput = 
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>shortDescription</ipxact:shortDescription>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:view>"
        ;

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testView_->setIsPresent("4-3");

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteEnvIdentifiers()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<View::EnvironmentIdentifier> identifier1( new View::EnvironmentIdentifier );
	identifier1->language = "language";
	identifier1->tool = "tool";
	identifier1->vendorSpecific = "vendor";
	testView_->addEnvIdentifier(identifier1);

	QSharedPointer<View::EnvironmentIdentifier> identifier2( new View::EnvironmentIdentifier );
	identifier2->language = "otherLanguage";
	identifier2->tool = "otherTool";
	identifier2->vendorSpecific = "otherVendor";
	testView_->addEnvIdentifier(identifier2);

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:envIdentifier>language:tool:vendor</ipxact:envIdentifier>"
            "<ipxact:envIdentifier>otherLanguage:otherTool:otherVendor</ipxact:envIdentifier>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteComponentInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteComponentInstantiationRef()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testView_->setComponentInstantiationRef("componentInstantiation");

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:componentInstantiationRef>componentInstantiation</ipxact:componentInstantiationRef>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteDesignInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteDesignInstantiationRef()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testView_->setDesignInstantiationRef("designInstantiation");

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:designInstantiationRef>designInstantiation</ipxact:designInstantiationRef>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteDesignConfigurationInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteDesignConfigurationInstantiationRef()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testView_->setDesignConfigurationInstantiationRef("instantiation");

    QString expectedOutput(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:designConfigurationInstantiationRef>instantiation</ipxact:designConfigurationInstantiationRef>"
        "</ipxact:view>"
        );

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);

    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ViewWriter)

//-----------------------------------------------------------------------------
// Function: tst_ViewWriter::testWriteVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ViewWriter::testWriteVendorExtensions()
{
    QDomDocument document;
    QDomElement extensionNode = document.createElement("ulina");
    extensionNode.setAttribute("kolina", "eaa");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testView_->getVendorExtensions()->append(testExtension);
    
    QString expectedOutput14(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
        "</ipxact:view>");

    QString expectedOutput22(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<ulina kolina=\"eaa\">testValue</ulina>"
            "</ipxact:vendorExtensions>"
        "</ipxact:view>");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput14);

    output.clear();

    ViewWriter::writeView(xmlStreamWriter, testView_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput22);
}

#include "tst_ViewWriter.moc"
