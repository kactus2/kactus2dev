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

#include <IPXACTmodels/GenericVendorExtension.h>

#include <IPXACTmodels/Component/DesignInstantiation.h>
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

QTEST_APPLESS_MAIN(tst_InstantiationsWriter)

#include "tst_InstantiationsWriter.moc"
