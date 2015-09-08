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

    void readDesignConfigurationInstantiation();
    void readDesignConfigurationInstantiationLanguage();
    void readDesignConfigurationInstantiationParameters();
    void readDesignConfigurationInstantiationExtensions();
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

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignConfigurationInstantiation()
{
    QString documentContent(
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designConfigurationInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignConfigurationInstantiation> testInstantiation =
        instantiationsReader.createDesignConfigurationInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->displayName(), QString("display"));
    QCOMPARE(testInstantiation->description(), QString("described"));

    QSharedPointer<ConfigurableVLNVReference> configurationReference =
        testInstantiation->getDesignConfigurationReference();
    QCOMPARE(configurationReference->getVendor(), QString("TUT"));
    QCOMPARE(configurationReference->getLibrary(), QString("TestLibrary"));
    QCOMPARE(configurationReference->getName(), QString("designConfiguration"));
    QCOMPARE(configurationReference->getVersion(), QString("1.1"));

    QCOMPARE(configurationReference->getConfigurableElementValues()->size(), 1);
    QSharedPointer<ConfigurableElementValue> element =
        configurationReference->getConfigurableElementValues()->first();
    QCOMPARE(element->getReferenceId(), QString("reference"));
    QCOMPARE(element->getConfigurableValue(), QString("4+4"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignConfigurationLanguage()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignConfigurationInstantiationLanguage()
{
    QString documentContent(
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:language>vhdl</ipxact:language>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designConfigurationInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignConfigurationInstantiation> testInstantiation =
        instantiationsReader.createDesignConfigurationInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->getLanguage(), QString("vhdl"));
    QCOMPARE(testInstantiation->isLangugageStrict(), false);

    documentContent =
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>designConfigurationInstantiation</ipxact:name>"
            "<ipxact:language strict=\"true\">verilog</ipxact:language>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        ;
    document.setContent(documentContent);
    instantiationNode = document.firstChildElement("ipxact:designConfigurationInstantiation");

    testInstantiation = instantiationsReader.createDesignConfigurationInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("designConfigurationInstantiation"));
    QCOMPARE(testInstantiation->getLanguage(), QString("verilog"));
    QCOMPARE(testInstantiation->isLangugageStrict(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignConfigurationParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignConfigurationInstantiationParameters()
{
    QString documentContent(
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
                "<ipxact:parameter parameterId=\"otherID\">"
                    "<ipxact:name>otherParameter</ipxact:name>"
                    "<ipxact:value>testID+1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designConfigurationInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignConfigurationInstantiation> testInstantiation =
        instantiationsReader.createDesignConfigurationInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    
    QCOMPARE(testInstantiation->getParameters()->size(), 2);
    QCOMPARE(testInstantiation->getParameters()->first()->name(), QString("testParameter"));
    QCOMPARE(testInstantiation->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testInstantiation->getParameters()->first()->getValue(), QString("1"));
    QCOMPARE(testInstantiation->getParameters()->last()->name(), QString("otherParameter"));
    QCOMPARE(testInstantiation->getParameters()->last()->getValueId(), QString("otherID"));
    QCOMPARE(testInstantiation->getParameters()->last()->getValue(), QString("testID+1"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readDesignConfigurationInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readDesignConfigurationInstantiationExtensions()
{
    QString documentContent(
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:designConfigurationInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<DesignConfigurationInstantiation> testInstantiation =
        instantiationsReader.createDesignConfigurationInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getVendorExtensions()->size(), 1);
    QCOMPARE(testInstantiation->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_InstantiationsReader)

#include "tst_InstantiationsReader.moc"
