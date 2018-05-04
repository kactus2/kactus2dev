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

#include <IPXACTmodels/common/VendorExtension.h>
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

    void readComponentInstantiation();
    void readVirtualComponentInstantiation();
    void readComponentInstantiationLanguage();
    void readComponentInstantiationNameReferences();
    void readComponentInstantiationModuleParameters();
    void readComponentInstantiationDefaultFileBuilders();
    void readComponentInstantiationFileSetReferences();
    void readComponentInstantiationParameters();
    void readComponentInstantiationExtensions();
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

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiation()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->displayName(), QString("displayed"));
    QCOMPARE(testInstantiation->description(), QString("described"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readVirtualComponentInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readVirtualComponentInstantiation()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:isVirtual>true</ipxact:isVirtual>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->isVirtual(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationLanguage()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationLanguage()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:language>verilog</ipxact:language>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->getLanguage(), QString("verilog"));
    QCOMPARE(testInstantiation->isLanguageStrict(), false);

    documentContent = 
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:language strict=\"true\">vhdl</ipxact:language>"
        "</ipxact:componentInstantiation>"
        ;

    document.setContent(documentContent);
    instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    testInstantiation = instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->getLanguage(), QString("vhdl"));
    QCOMPARE(testInstantiation->isLanguageStrict(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationNameReferences()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationNameReferences()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:libraryName>work</ipxact:libraryName>"
            "<ipxact:packageName>componentVLNV_cmp_pkg</ipxact:packageName>"
            "<ipxact:moduleName>componentVLNV</ipxact:moduleName>"
            "<ipxact:architectureName>rtl</ipxact:architectureName>"
            "<ipxact:configurationName>componentVLNV_rtl_cfg</ipxact:configurationName>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getLibraryName(), QString("work"));
    QCOMPARE(testInstantiation->getPackageName(), QString("componentVLNV_cmp_pkg"));
    QCOMPARE(testInstantiation->getModuleName(), QString("componentVLNV"));
    QCOMPARE(testInstantiation->getArchitectureName(), QString("rtl"));
    QCOMPARE(testInstantiation->getConfigurationName(), QString("componentVLNV_rtl_cfg"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationModuleParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationModuleParameters()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:moduleName>componentVLNV</ipxact:moduleName>"
            "<ipxact:moduleParameters>"
                "<ipxact:moduleParameter choiceRef=\"testingChoice\" dataType=\"longInt\" maximum=\"403\""
                        " minimum=\"3\" parameterId=\"testID\" usageType=\"int\">"
                    "<ipxact:name>nameTest</ipxact:name>"
                    "<ipxact:value>400</ipxact:value>"
                    "<ipxact:isPresent>4-3</ipxact:isPresent>"
                "</ipxact:moduleParameter>"
            "</ipxact:moduleParameters>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));
    QCOMPARE(testInstantiation->getModuleName(), QString("componentVLNV"));

    QCOMPARE(testInstantiation->getModuleParameters()->size(), 1);
    QSharedPointer<ModuleParameter> moduleParameter = testInstantiation->getModuleParameters()->first();
    QCOMPARE(moduleParameter->name(), QString("nameTest"));
    QCOMPARE(moduleParameter->getValue(), QString("400"));
    QCOMPARE(moduleParameter->getIsPresent(), QString("4-3"));

    QCOMPARE(moduleParameter->getChoiceRef(), QString("testingChoice"));
    QCOMPARE(moduleParameter->getDataType(), QString("longInt"));
    QCOMPARE(moduleParameter->getMaximumValue(), QString("403"));
    QCOMPARE(moduleParameter->getMinimumValue(), QString("3"));
    QCOMPARE(moduleParameter->getValueId(), QString("testID"));
    QCOMPARE(moduleParameter->getUsageType(), QString("int"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationDefaultFileBuilders()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationDefaultFileBuilders()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType>asmSource</ipxact:fileType>"
                "<ipxact:command>riseFromYourGrave</ipxact:command>"
                "<ipxact:flags>passedFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>1-1+0</ipxact:replaceDefaultFlags>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getDefaultFileBuilders()->size(), 1);

    QSharedPointer<FileBuilder> defaultFileBuilder = testInstantiation->getDefaultFileBuilders()->first();
    QCOMPARE(defaultFileBuilder->getFileType(), QString("asmSource"));
    QCOMPARE(defaultFileBuilder->getUserFileType(), QString(""));
    QCOMPARE(defaultFileBuilder->getCommand(), QString("riseFromYourGrave"));
    QCOMPARE(defaultFileBuilder->getFlags(), QString("passedFlags"));
    QCOMPARE(defaultFileBuilder->getReplaceDefaultFlags(), QString("1-1+0"));

    documentContent =
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
                "<ipxact:command>riseFromYourGrave</ipxact:command>"
                "<ipxact:flags>passedFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>1-1+0</ipxact:replaceDefaultFlags>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:designConfigurationInstantiation>"
        ;

    document.setContent(documentContent);

    instantiationNode = document.firstChildElement("ipxact:componentInstantiation");
    testInstantiation = instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->getDefaultFileBuilders()->size(), 1);

    defaultFileBuilder = testInstantiation->getDefaultFileBuilders()->first();
    QCOMPARE(defaultFileBuilder->getFileType(), QString("user"));
    QCOMPARE(defaultFileBuilder->getUserFileType(), QString("userFile"));
    QCOMPARE(defaultFileBuilder->getCommand(), QString("riseFromYourGrave"));
    QCOMPARE(defaultFileBuilder->getFlags(), QString("passedFlags"));
    QCOMPARE(defaultFileBuilder->getReplaceDefaultFlags(), QString("1-1+0"));

}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationFileSetReferences()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationFileSetReferences()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:fileSetRef>"
                "<ipxact:localName>otherFile</ipxact:localName>"
            "</ipxact:fileSetRef>"
            "<ipxact:fileSetRef>"
                "<ipxact:localName>referencedFile</ipxact:localName>"
            "</ipxact:fileSetRef>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getFileSetReferences()->count(), 2);
    QCOMPARE(testInstantiation->getFileSetReferences()->first(), QString("otherFile"));
    QCOMPARE(testInstantiation->getFileSetReferences()->last(), QString("referencedFile"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationParameters()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>nameTest</ipxact:name>"
                    "<ipxact:value>400</ipxact:value>"
                "</ipxact:parameter>"
                "<ipxact:parameter parameterId=\"otherID\">"
                    "<ipxact:name>otherTest</ipxact:name>"
                    "<ipxact:value>200</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getParameters()->size(), 2);
    QCOMPARE(testInstantiation->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testInstantiation->getParameters()->first()->name(), QString("nameTest"));
    QCOMPARE(testInstantiation->getParameters()->first()->getValue(), QString("400"));
    QCOMPARE(testInstantiation->getParameters()->last()->getValueId(), QString("otherID"));
    QCOMPARE(testInstantiation->getParameters()->last()->name(), QString("otherTest"));
    QCOMPARE(testInstantiation->getParameters()->last()->getValue(), QString("200"));
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsReader::readComponentInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsReader::readComponentInstantiationExtensions()
{
    QString documentContent(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:designConfigurationInstantiation>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode instantiationNode = document.firstChildElement("ipxact:componentInstantiation");

    InstantiationsReader instantiationsReader;
    QSharedPointer<ComponentInstantiation> testInstantiation =
        instantiationsReader.createComponentInstantiationFrom(instantiationNode);

    QCOMPARE(testInstantiation->name(), QString("testInstantiation"));

    QCOMPARE(testInstantiation->getVendorExtensions()->size(), 1);
    QCOMPARE(testInstantiation->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_InstantiationsReader)

#include "tst_InstantiationsReader.moc"
