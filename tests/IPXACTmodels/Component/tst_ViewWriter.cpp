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

#include <QtTest>

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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);
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

    viewWriter.writeView(xmlStreamWriter, testView_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testView_->setDescription("testDescription");
    expectedOutput = 
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:view>"
        ;

    viewWriter.writeView(xmlStreamWriter, testView_);
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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);

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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);

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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);

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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);

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

    ViewWriter viewWriter;
    viewWriter.writeView(xmlStreamWriter, testView_);

    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ViewWriter)

#include "tst_ViewWriter.moc"
