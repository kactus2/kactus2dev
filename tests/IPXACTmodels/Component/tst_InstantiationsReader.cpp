//-----------------------------------------------------------------------------
// File: tst_InstantiationsReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Unit test for class InstantiationsReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/Component/InstantiationsReader.h>

#include <QtTest>

class tst_InstantiationsReader : public QObject
{
    Q_OBJECT

public:
    tst_InstantiationsReader();

private slots:
    
    void readDesignInstantiation();
    void readDesignInstantiationExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::tst_InstantiationsReader()
//-----------------------------------------------------------------------------
tst_InstantiationsReader::tst_InstantiationsReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignInstantiation()
{
    QString documentContent(
        "<ipxact:designInstantiation>"
            "<ipxact:name>testDesignInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\" name=\"designReference\" version=\"1.0\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"refID\">20</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designRef>"
        "</ipxact:designInstantiation>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignInstantiation> testDesignInstantiation =
        instantiationsReader.createDesignInstantiationFrom(instantiationNode);
    
    QCOMPARE(testDesignInstantiation->name(), QString("testDesignInstantiation"));
    QCOMPARE(testDesignInstantiation->displayName(), QString("display"));
    QCOMPARE(testDesignInstantiation->description(), QString("described"));

    QSharedPointer<ConfigurableVLNVReference> designReference = testDesignInstantiation->getDesignReference();
    QCOMPARE(designReference->getVendor(), QString("TUT"));
    QCOMPARE(designReference->getLibrary(), QString("TestLibrary"));
    QCOMPARE(designReference->getName(), QString("designReference"));
    QCOMPARE(designReference->getVersion(), QString("1.0"));

    QCOMPARE(designReference->getConfigurableElementValues()->size(), 1);
    QCOMPARE(designReference->getConfigurableElementValues()->first()->getReferenceId(), QString("refID"));
    QCOMPARE(designReference->getConfigurableElementValues()->first()->getConfigurableValue(), QString("20"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignInstantiationExtensions()
{
    QString documentContent(
        "<ipxact:designInstantiation>"
            "<ipxact:name>testDesignInstantiation</ipxact:name>"
            "<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\" name=\"designReference\" version=\"1.0\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"refID\">20</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designRef>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designInstantiation>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignInstantiation> testDesignInstantiation =
        instantiationsReader.createDesignInstantiationFrom(instantiationNode);

    QCOMPARE(testDesignInstantiation->name(), QString("testDesignInstantiation"));

    QCOMPARE(testDesignInstantiation->getVendorExtensions()->size(), 1);
    QCOMPARE(testDesignInstantiation->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_InstantiationsReader)

#include "tst_InstantiationsReader.moc"
