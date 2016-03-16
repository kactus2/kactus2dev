//-----------------------------------------------------------------------------
// File: tst_InstantiationsWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class InstantiationsWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/InstantiationsWriter.h>

#include <QtTest>

class tst_InstantiationsWriter : public QObject
{
    Q_OBJECT

public:
    tst_InstantiationsWriter();

private slots:

    void writeDesignInstatiation();
    void writeDesignInstantiationExtensions();

    void writeDesignConfigurationInstantiation();
    void writeDesignConfigurationInstantiationLanguage();
    void writeDesignConfigurationInstantiationParameters();
    void writeDesignConfigurationInstantiationExtensions();

    void writeComponentInstantiation();
    void writeVirtualComponentInstantiation();
    void writeComponentInstantiationLanguage();
    void writeComponentInstantiationNameReferences();
    void writeComponentInstantiationModuleParameters();
    void writeComponentInstantiationDefaultFileBuilders();
    void writeComponentInstantiationFileSetReferences();
    void writeComponentInstantiationParameters();
    void writeComponentInstantiationExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::tst_InstantiationsWriter()
//-----------------------------------------------------------------------------
tst_InstantiationsWriter::tst_InstantiationsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignInstatiations()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignInstatiation()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

     QSharedPointer<ConfigurableVLNVReference> testDesignReference (
         new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designReference", "1.0"));
 
     QSharedPointer<ConfigurableElementValue> designReferenceConfig (new ConfigurableElementValue("20", "refID"));
     testDesignReference->getConfigurableElementValues()->append(designReferenceConfig);

    QSharedPointer<DesignInstantiation> testDesignInstantiation (
        new DesignInstantiation("testDesignInstantiation"));
    testDesignInstantiation->setDesignReference(testDesignReference);

    QString expectedOutput(
        "<ipxact:designInstantiation>"
            "<ipxact:name>testDesignInstantiation</ipxact:name>"
            "<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\" name=\"designReference\" version=\"1.0\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"refID\">20</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designRef>"
        "</ipxact:designInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignInstantiation(xmlStreamWriter, testDesignInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testDesignInstantiation->setDisplayName("display");
    expectedOutput = 
        "<ipxact:designInstantiation>"
            "<ipxact:name>testDesignInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\" name=\"designReference\" version=\"1.0\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"refID\">20</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designRef>"
        "</ipxact:designInstantiation>"
        ;

    instantiationWriter.writeDesignInstantiation(xmlStreamWriter, testDesignInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testDesignInstantiation->setDescription("described");
    expectedOutput =
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
        ;

    instantiationWriter.writeDesignInstantiation(xmlStreamWriter, testDesignInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignInstantiationExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtensions(new GenericVendorExtension(extensionNode));

    QSharedPointer<ConfigurableVLNVReference> testDesignReference (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designReference", "1.0"));

    QSharedPointer<ConfigurableElementValue> designReferenceConfig (new ConfigurableElementValue("20", "refID"));
    testDesignReference->getConfigurableElementValues()->append(designReferenceConfig);

    QSharedPointer<DesignInstantiation> testDesignInstantiation (
        new DesignInstantiation("testDesignInstantiation"));
    testDesignInstantiation->setDesignReference(testDesignReference);
    testDesignInstantiation->getVendorExtensions()->append(testExtensions);

    QString expectedOutput(
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

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignInstantiation(xmlStreamWriter, testDesignInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignConfigurationInstantiation()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ConfigurableVLNVReference> testDesignConfiguration (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designConfiguration", "1.1"));

    QSharedPointer<ConfigurableElementValue> configurableElement (new ConfigurableElementValue("4+4", "reference"));
    testDesignConfiguration->getConfigurableElementValues()->append(configurableElement);

    QSharedPointer<DesignConfigurationInstantiation> testDesignConfigurationInstantiation
        (new DesignConfigurationInstantiation("testInstantiation"));
    testDesignConfigurationInstantiation->setDesignConfigurationReference(testDesignConfiguration);

    QString expectedOutput(
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testDesignConfigurationInstantiation->setDisplayName("display");
    expectedOutput = 
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        ;

    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testDesignConfigurationInstantiation->setDescription("described");
    expectedOutput =
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
        ;

    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignConfigurationInstantiationLanguage()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignConfigurationInstantiationLanguage()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ConfigurableVLNVReference> testDesignConfiguration (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designConfiguration", "1.1"));

    QSharedPointer<ConfigurableElementValue> configurableElement (new ConfigurableElementValue("4+4", "reference"));
    testDesignConfiguration->getConfigurableElementValues()->append(configurableElement);

    QSharedPointer<DesignConfigurationInstantiation> testDesignConfigurationInstantiation
        (new DesignConfigurationInstantiation("testInstantiation"));
    testDesignConfigurationInstantiation->setDesignConfigurationReference(testDesignConfiguration);
    testDesignConfigurationInstantiation->setLanguage("vhdl");

    QString expectedOutput(
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

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testDesignConfigurationInstantiation->setLanguageStrict(true);
    expectedOutput =
        "<ipxact:designConfigurationInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:language strict=\"true\">vhdl</ipxact:language>"
            "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                    " name=\"designConfiguration\" version=\"1.1\">"
                "<ipxact:configurableElementValues>"
                    "<ipxact:configurableElementValue referenceId=\"reference\">4+4</ipxact:configurableElementValue>"
                "</ipxact:configurableElementValues>"
            "</ipxact:designConfigurationRef>"
        "</ipxact:designConfigurationInstantiation>"
        ;

    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignConfigurationInstantiationParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignConfigurationInstantiationParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ConfigurableVLNVReference> testDesignConfiguration (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designConfiguration", "1.1"));

    QSharedPointer<ConfigurableElementValue> configurableElement (new ConfigurableElementValue("4+4", "reference"));
    testDesignConfiguration->getConfigurableElementValues()->append(configurableElement);

    QSharedPointer<DesignConfigurationInstantiation> testDesignConfigurationInstantiation
        (new DesignConfigurationInstantiation("testInstantiation"));
    testDesignConfigurationInstantiation->setDesignConfigurationReference(testDesignConfiguration);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("nameTest");
    testParameter->setValue("400");

    QSharedPointer<Parameter> otherParameter(new Parameter());
    otherParameter->setValueId("otherID");
    otherParameter->setName("otherTest");
    otherParameter->setValue("200");

    testDesignConfigurationInstantiation->getParameters()->append(testParameter);
    testDesignConfigurationInstantiation->getParameters()->append(otherParameter);

    QString expectedOutput(
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

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeDesignConfigurationInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeDesignConfigurationInstantiationExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ConfigurableVLNVReference> testDesignConfiguration (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designConfiguration", "1.1"));

    QSharedPointer<ConfigurableElementValue> configurableElement (new ConfigurableElementValue("4+4", "reference"));
    testDesignConfiguration->getConfigurableElementValues()->append(configurableElement);

    QSharedPointer<DesignConfigurationInstantiation> testDesignConfigurationInstantiation
        (new DesignConfigurationInstantiation("testInstantiation"));
    testDesignConfigurationInstantiation->setDesignConfigurationReference(testDesignConfiguration);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtensions(new GenericVendorExtension(extensionNode));
    testDesignConfigurationInstantiation->getVendorExtensions()->append(testExtensions);

    QString expectedOutput(
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

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeDesignConfigurationInstantiation(xmlStreamWriter,
        testDesignConfigurationInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiation()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation (
        new ComponentInstantiation("testComponentInstantiation"));

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testComponentInstantiation->setDisplayName("display");
    expectedOutput = 
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
        "</ipxact:componentInstantiation>"
        ;

    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testComponentInstantiation->setDescription("described");
    expectedOutput =
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        "</ipxact:componentInstantiation>"
        ;

    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeVirtualComponentInstantiation()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeVirtualComponentInstantiation()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation (
        new ComponentInstantiation("testComponentInstantiation"));
    testComponentInstantiation->setVirtual(true);

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:isVirtual>true</ipxact:isVirtual>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationLanguage()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationLanguage()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation (
        new ComponentInstantiation("testComponentInstantiation"));
    testComponentInstantiation->setLanguage("verilog");

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:language>verilog</ipxact:language>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testComponentInstantiation->setLanguage("vhdl");
    testComponentInstantiation->setLanguageStrictness(true);
    expectedOutput = 
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:language strict=\"true\">vhdl</ipxact:language>"
        "</ipxact:componentInstantiation>"
        ;

    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationNameReferences()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationNameReferences()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation (
        new ComponentInstantiation("testComponentInstantiation"));
    testComponentInstantiation->setLibraryName("work");
    testComponentInstantiation->setPackageName("componentVLNV_cmp_pkg");
    testComponentInstantiation->setModuleName("componentVLNV");
    testComponentInstantiation->setArchitectureName("rtl");
    testComponentInstantiation->setConfigurationName("componentVLNV_rtl_cfg");

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testComponentInstantiation</ipxact:name>"
            "<ipxact:libraryName>work</ipxact:libraryName>"
            "<ipxact:packageName>componentVLNV_cmp_pkg</ipxact:packageName>"
            "<ipxact:moduleName>componentVLNV</ipxact:moduleName>"
            "<ipxact:architectureName>rtl</ipxact:architectureName>"
            "<ipxact:configurationName>componentVLNV_rtl_cfg</ipxact:configurationName>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationModuleParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationModuleParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation
        (new ComponentInstantiation("testInstantiation"));

    QSharedPointer<ModuleParameter> testModuleParameter(new ModuleParameter());
    testModuleParameter->setValueId("testID");
    testModuleParameter->setChoiceRef("testingChoice");
    testModuleParameter->setMinimumValue("3");
    testModuleParameter->setMaximumValue("403");
    testModuleParameter->setName("nameTest");
    testModuleParameter->setValue("400");
    testModuleParameter->setIsPresent("4-3");
    testModuleParameter->setUsageType("int");
    testModuleParameter->setDataType("longInt");

    testComponentInstantiation->getModuleParameters()->append(testModuleParameter);

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:moduleParameters>"
                "<ipxact:moduleParameter choiceRef=\"testingChoice\" dataType=\"longInt\" maximum=\"403\""
                        " minimum=\"3\" parameterId=\"testID\" usageType=\"int\">"
                    "<ipxact:name>nameTest</ipxact:name>"
                    "<ipxact:value>400</ipxact:value>"
                    "<ipxact:isPresent>4-3</ipxact:isPresent>"
                "</ipxact:moduleParameter>"
            "</ipxact:moduleParameters>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}


//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationDefaultFileBuilders()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationDefaultFileBuilders()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation
        (new ComponentInstantiation("testInstantiation"));

    QSharedPointer<FileBuilder> defaultFileBuilder (new FileBuilder);
    defaultFileBuilder->setCommand("riseFromYourGrave");
    defaultFileBuilder->setFlags("passedFlags");
    defaultFileBuilder->setReplaceDefaultFlags("1-1+0");
    defaultFileBuilder->setFileType("asmSource");

    testComponentInstantiation->getDefaultFileBuilders()->append(defaultFileBuilder);

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType>asmSource</ipxact:fileType>"
                "<ipxact:command>riseFromYourGrave</ipxact:command>"
                "<ipxact:flags>passedFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>1-1+0</ipxact:replaceDefaultFlags>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    defaultFileBuilder->setFileType("userFile");
    expectedOutput =
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
                "<ipxact:command>riseFromYourGrave</ipxact:command>"
                "<ipxact:flags>passedFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>1-1+0</ipxact:replaceDefaultFlags>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:componentInstantiation>"
        ;

    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationFileSetReferences()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationFileSetReferences()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation
        (new ComponentInstantiation("testInstantiation"));
    testComponentInstantiation->getFileSetReferences()->append("referencedFile");
    testComponentInstantiation->getFileSetReferences()->append("otherFile");

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:fileSetRef>"
                "<ipxact:localName>referencedFile</ipxact:localName>"
            "</ipxact:fileSetRef>"
            "<ipxact:fileSetRef>"
                "<ipxact:localName>otherFile</ipxact:localName>"
            "</ipxact:fileSetRef>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationParameters()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation
        (new ComponentInstantiation("testInstantiation"));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("nameTest");
    testParameter->setValue("400");

    QSharedPointer<Parameter> otherParameter(new Parameter());
    otherParameter->setValueId("otherID");
    otherParameter->setName("otherTest");
    otherParameter->setValue("200");

    testComponentInstantiation->getParameters()->append(testParameter);
    testComponentInstantiation->getParameters()->append(otherParameter);

    QString expectedOutput(
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
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_InstantiationsWriter::writeComponentInstantiationExtensions()
//-----------------------------------------------------------------------------
void tst_InstantiationsWriter::writeComponentInstantiationExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ComponentInstantiation> testComponentInstantiation
        (new ComponentInstantiation("testInstantiation"));

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtensions(new GenericVendorExtension(extensionNode));
    testComponentInstantiation->getVendorExtensions()->append(testExtensions);

    QString expectedOutput(
        "<ipxact:componentInstantiation>"
            "<ipxact:name>testInstantiation</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:componentInstantiation>"
        );

    InstantiationsWriter instantiationWriter;
    instantiationWriter.writeComponentInstantiation(xmlStreamWriter, testComponentInstantiation);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_InstantiationsWriter)

#include "tst_InstantiationsWriter.moc"
