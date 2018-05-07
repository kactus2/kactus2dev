//-----------------------------------------------------------------------------
// File: tst_ViewReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Unit test for class ViewReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ViewReader.h>

#include <QtTest>

class tst_ViewReader : public QObject
{
    Q_OBJECT

public:
    tst_ViewReader();

private slots:

    void testReadSimpleView();
	void testReadIsPresent();
	void testReadEnvIdentifiers();
	void testReadMalformedEnvIdentifiers();
	void testReadMalformedEnvIdentifiers2();
    void testReadComponentInstantiationRef();
    void testReadDesignInstantiationRef();
    void testReadDesignConfigurationInstantiationRef();
};

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::tst_ViewReader()
//-----------------------------------------------------------------------------
tst_ViewReader::tst_ViewReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadSimpleView()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadSimpleView()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));
    QCOMPARE(testView->displayName(), QString("viewDisplay"));
    QCOMPARE(testView->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));
    QCOMPARE(testView->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadEnvIdentifiers()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
			"<ipxact:envIdentifier>environmentIdentifier:tool:vendorX</ipxact:envIdentifier>"
            "<ipxact:envIdentifier>otherEnvironment:otherTool:vendorY</ipxact:envIdentifier>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));

    QCOMPARE(testView->getEnvIdentifiers()->size(), 2);
    QCOMPARE(testView->getEnvIdentifiers()->first()->toString(), QString("environmentIdentifier:tool:vendorX"));
    QCOMPARE(testView->getEnvIdentifiers()->last()->toString(), QString("otherEnvironment:otherTool:vendorY"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadMalformedEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadMalformedEnvIdentifiers()
{
	QString documentContent(
		"<ipxact:view>"
		"<ipxact:name>testView</ipxact:name>"
		"<ipxact:envIdentifier>environmentIdentifier</ipxact:envIdentifier>"
		"<ipxact:envIdentifier>otherEnvironment</ipxact:envIdentifier>"
		"</ipxact:view>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode viewNode = document.firstChildElement("ipxact:view");

	ViewReader viewReader;
	QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

	QCOMPARE(testView->name(), QString("testView"));

	QCOMPARE(testView->getEnvIdentifiers()->size(), 2);
	QCOMPARE(testView->getEnvIdentifiers()->first()->toString(), QString("environmentIdentifier::"));
	QCOMPARE(testView->getEnvIdentifiers()->last()->toString(), QString("otherEnvironment::"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadMalformedEnvIdentifiers2()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadMalformedEnvIdentifiers2()
{
	QString documentContent(
		"<ipxact:view>"
		"<ipxact:name>testView</ipxact:name>"
		"<ipxact:envIdentifier>environmentIdentifier:tool</ipxact:envIdentifier>"
		"<ipxact:envIdentifier>otherEnvironment:otherTool</ipxact:envIdentifier>"
		"</ipxact:view>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode viewNode = document.firstChildElement("ipxact:view");

	ViewReader viewReader;
	QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

	QCOMPARE(testView->name(), QString("testView"));

	QCOMPARE(testView->getEnvIdentifiers()->size(), 2);
	QCOMPARE(testView->getEnvIdentifiers()->first()->toString(), QString("environmentIdentifier:tool:"));
	QCOMPARE(testView->getEnvIdentifiers()->last()->toString(), QString("otherEnvironment:otherTool:"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadComponentInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadComponentInstantiationRef()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:componentInstantiationRef>instantiation</ipxact:componentInstantiationRef>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));
    QCOMPARE(testView->getComponentInstantiationRef(), QString("instantiation"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadDesignInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadDesignInstantiationRef()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:designInstantiationRef>instantiation</ipxact:designInstantiationRef>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));
    QCOMPARE(testView->getDesignInstantiationRef(), QString("instantiation"));
}

//-----------------------------------------------------------------------------
// Function: tst_ViewReader::testReadDesignConfigurationInstantiationRef()
//-----------------------------------------------------------------------------
void tst_ViewReader::testReadDesignConfigurationInstantiationRef()
{
    QString documentContent(
        "<ipxact:view>"
            "<ipxact:name>testView</ipxact:name>"
            "<ipxact:designConfigurationInstantiationRef>instantiation</ipxact:designConfigurationInstantiationRef>"
        "</ipxact:view>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode viewNode = document.firstChildElement("ipxact:view");

    ViewReader viewReader;
    QSharedPointer<View> testView = viewReader.createViewFrom(viewNode);

    QCOMPARE(testView->name(), QString("testView"));
    QCOMPARE(testView->getDesignConfigurationInstantiationRef(), QString("instantiation"));
}

QTEST_APPLESS_MAIN(tst_ViewReader)

#include "tst_ViewReader.moc"
