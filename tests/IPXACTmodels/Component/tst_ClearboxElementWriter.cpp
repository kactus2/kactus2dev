//-----------------------------------------------------------------------------
// File: tst_ClearboxElementWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Unit test for ClearboxElementWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ClearboxElementWriter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_ClearboxElementWriter : public QObject
{
    Q_OBJECT

public:
    tst_ClearboxElementWriter();

private slots:
    void tst_writeClearboxElement();
    void tst_writeWhiteboxElement();
};

//-----------------------------------------------------------------------------
// Function: tst_ClearboxElementWriter::tst_ClearboxElementWriter()
//-----------------------------------------------------------------------------
tst_ClearboxElementWriter::tst_ClearboxElementWriter()
{

}

void tst_ClearboxElementWriter::tst_writeClearboxElement()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> param(new Parameter());
    param->setName("testParam");
    param->setValueId("id");
    param->setValue("32");

    // Create vendor extension for test
    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<ClearboxElement> testElement(new ClearboxElement("a_name"));
    testElement->setType(ClearboxElement::Type::SIGNAL);
    testElement->setDriveable(true);
    testElement->getParameters()->append(param);
    testElement->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:clearboxElement>"
            "<ipxact:name>a_name</ipxact:name>"
            "<ipxact:clearboxType>signal</ipxact:clearboxType>"
            "<ipxact:driveable>true</ipxact:driveable>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"id\">"
                    "<ipxact:name>testParam</ipxact:name>"
                    "<ipxact:value>32</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:clearboxElement>"
    );

    ClearboxElementWriter::writeClearboxElement(xmlStreamWriter, testElement, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_ClearboxElementWriter::tst_writeWhiteboxElement()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> param(new Parameter());
    param->setName("testParam");
    param->setValueId("id");
    param->setValue("32");

    // Create vendor extension for test
    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<ClearboxElement> testElement(new ClearboxElement("a_name"));
    testElement->setType(ClearboxElement::Type::SIGNAL);
    testElement->setIsPresent("false");
    testElement->setDriveable(true);
    testElement->getParameters()->append(param);
    testElement->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:whiteboxElement>"
            "<ipxact:name>a_name</ipxact:name>"
            "<ipxact:isPresent>false</ipxact:isPresent>"
            "<ipxact:whiteboxType>signal</ipxact:whiteboxType>"
            "<ipxact:driveable>true</ipxact:driveable>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"id\">"
                    "<ipxact:name>testParam</ipxact:name>"
                    "<ipxact:value>32</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:whiteboxElement>"
    );

    ClearboxElementWriter::writeClearboxElement(xmlStreamWriter, testElement, Document::Revision::Std14);
    QFile file("output.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.write(output.toUtf8());
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ClearboxElementWriter)

#include "tst_ClearboxElementWriter.moc"
