//-----------------------------------------------------------------------------
// File: tst_ApiDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Unit test for class ApiDefinitionWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/kactusExtensions/ApiDefinitionWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_ApiDefinitionWriter : public QObject
{
    Q_OBJECT

public:
	tst_ApiDefinitionWriter();

private slots:
	void init();
	void cleanup();

	void baseCase();

private:

	QSharedPointer<ApiDefinition> testApiDefinition_;
};

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::tst_ApiDefinitionWriter()
//-----------------------------------------------------------------------------
tst_ApiDefinitionWriter::tst_ApiDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::init()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::init()
{
	testApiDefinition_ = QSharedPointer<ApiDefinition>(new ApiDefinition(VLNV(), Document::Revision::Std14));
}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::cleanup()
{
	testApiDefinition_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::baseCase()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::baseCase()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testApiDefinition_->setVlnv(VLNV(VLNV::APIDEFINITION,"me","kurjasto","def","0.11"));
	testApiDefinition_->setComDefinitionRef(VLNV(VLNV::COMDEFINITION,"he","s-lib","cdef","1.337"));
	testApiDefinition_->getDataTypes()->append("new item");

	QSharedPointer<ApiFunction> func( new ApiFunction );
	func->setName("hjjhjh");
	func->setDescription("yryry");
	func->setReturnValueType("void");
	func->setReturnValueDescription("lolol");

    testApiDefinition_->getFunctions()->append(func);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testApiDefinition_->getVendorExtensions()->append(testExtension);
    testApiDefinition_->setVersion("3.0.0");

	QString expectedOutput(
		"<?xml version=\"1.0\"?>"
		"<kactus2:apiDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
		" xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\""
		" xmlns:kactus2=\"http://kactus2.cs.tut.fi\""
		" xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014"
		" http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
		"<ipxact:vendor>me</ipxact:vendor>"
		"<ipxact:library>kurjasto</ipxact:library>"
		"<ipxact:name>def</ipxact:name>"
		"<ipxact:version>0.11</ipxact:version>"
		"<kactus2:comDefinitionRef vendor=\"he\" library=\"s-lib\" name=\"cdef\" version=\"1.337\"/>"
		"<kactus2:dataTypes><kactus2:dataType name=\"new item\"/></kactus2:dataTypes>"
		"<kactus2:functions>"
		"<kactus2:function name=\"hjjhjh\" description=\"yryry\">"
		"<kactus2:returnValue type=\"void\" description=\"lolol\"/>"
		"</kactus2:function>"
        "</kactus2:functions>"
        "<ipxact:vendorExtensions>"
        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
        "<kactus2:version>3.0.0</kactus2:version>"
        "</ipxact:vendorExtensions>"
		"</kactus2:apiDefinition>\n"
		);

	ApiDefinitionWriter::writeApiDefinition(xmlStreamWriter, testApiDefinition_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ApiDefinitionWriter)

#include "tst_ApiDefinitionWriter.moc"
