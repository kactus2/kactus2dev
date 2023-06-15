//-----------------------------------------------------------------------------
// File: tst_CPUWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for class CPUWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/CPUWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_CPUWriter : public QObject
{
    Q_OBJECT

public:
	tst_CPUWriter();

private slots:
	void init();
	void cleanup();

    void testWriteCPUNameGroup();
    void testWriteCPUNameGroup2022();

	void testWriteIsPresent();
	void testWriteParameter();
	void testWriteAddressSpaces();

    void testWriteBlockSize2022();

    void testWriteBlockRegions2022();

	void testWriteAUB2022();

    void testWriteExecutableImages2022();

	void testWriteMemoryMapReference2022();

	void testWriteVendorExtension();

private:

	QSharedPointer<Cpu> testCPU_;
};

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::tst_CPUWriter()
//-----------------------------------------------------------------------------
tst_CPUWriter::tst_CPUWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::init()
//-----------------------------------------------------------------------------
void tst_CPUWriter::init()
{
	testCPU_ = QSharedPointer<Cpu>(new Cpu());
	testCPU_->setName("testCpu");
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_CPUWriter::cleanup()
{
	testCPU_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteCPUNameGroup()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteCPUNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setDisplayName("testDisplay");
	testCPU_->setDescription("testDescription");

	QString expectedOutput = 
		"<ipxact:cpu>"
		    "<ipxact:name>testCpu</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:cpu>";

    CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteCPUNameGroup2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteCPUNameGroup2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testCPU_->setDisplayName("testDisplay");
    testCPU_->setShortDescription("brief");
    testCPU_->setDescription("testDescription");

    QString expectedOutput =
        "<ipxact:cpu>"
			"<ipxact:name>testCpu</ipxact:name>"
			"<ipxact:displayName>testDisplay</ipxact:displayName>"
			"<ipxact:shortDescription>brief</ipxact:shortDescription>"
			"<ipxact:description>testDescription</ipxact:description>"
        "</ipxact:cpu>";

    CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteIsPresent()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setIsPresent("4-3");

	QString expectedOutput(
		"<ipxact:cpu>"
		    "<ipxact:name>testCpu</ipxact:name>"
		    "<ipxact:isPresent>4-3</ipxact:isPresent>"
		"</ipxact:cpu>"
		);

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteParameter()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteParameter()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setIsPresent("4-3");

	QSharedPointer<Parameter> para1(new Parameter());
	para1->setName("testParameter");
	para1->setValueId("id1");
	para1->setValue("5");
	para1->setType("shortint");
	para1->setAttribute("prompt", "Parm 1");
	para1->setAttribute("resolve", "user");
	para1->setDescription("First generator parameter.");

    testCPU_->getParameters()->append(para1);

	QString expectedOutput(
		"<ipxact:cpu>"
		    "<ipxact:name>testCpu</ipxact:name>"
		    "<ipxact:isPresent>4-3</ipxact:isPresent>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id1\" prompt=\"Parm 1\" resolve=\"user\" type=\"shortint\">"
		            "<ipxact:name>testParameter</ipxact:name>"
		            "<ipxact:description>First generator parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:cpu>"
		);

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteAddressSpaces()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteAddressSpaces()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setName("cpuWithAddressSpace");

    QSharedPointer<Cpu::AddressSpaceRef> addressReference (new Cpu::AddressSpaceRef("lackOfImagination"));
    addressReference->setIsPresent("spacePresence");

    testCPU_->getAddressSpaceReferences()->append(addressReference);

	QString expectedOutput(
		"<ipxact:cpu>"
    		"<ipxact:name>cpuWithAddressSpace</ipxact:name>"
	    	    "<ipxact:addressSpaceRef addressSpaceRef=\"lackOfImagination\">"
                    "<ipxact:isPresent>spacePresence</ipxact:isPresent>"
                "</ipxact:addressSpaceRef>"
		"</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteBlockSize2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteBlockSize2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testCPU_->setRange("16");
    testCPU_->setWidth("32");

	QString expectedOutput(
		"<ipxact:cpu>"
    		"<ipxact:name>testCpu</ipxact:name>"
	    	    "<ipxact:range>16</ipxact:range>"
                "<ipxact:width>32</ipxact:width>"
		"</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteBlockRegions2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteBlockRegions2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Region> firstRegion(new Region("firstRegion"));
    firstRegion->setDisplayName("r1");
    firstRegion->setShortDescription("brief");
    firstRegion->setDescription("region description text");
	firstRegion->setOffset("0");
	firstRegion->setRange("8");
	firstRegion->getVendorExtensions()->append(
		QSharedPointer<VendorExtension>(new Kactus2Value("testExtension", "testValue")));

	testCPU_->getRegions()->append(firstRegion);

    QSharedPointer<Region> secondRegion(new Region("secondRegion"));
    secondRegion->setDisplayName("r2");
	secondRegion->setOffset("8");
	secondRegion->setRange("16");

	testCPU_->getRegions()->append(secondRegion);

	QString expectedOutput(
        "<ipxact:cpu>"
            "<ipxact:name>testCpu</ipxact:name>"
            "<ipxact:regions>"
                "<ipxact:region>"
                    "<ipxact:name>firstRegion</ipxact:name>"
                    "<ipxact:displayName>r1</ipxact:displayName>"
                    "<ipxact:shortDescription>brief</ipxact:shortDescription>"
                    "<ipxact:description>region description text</ipxact:description>"
                    "<ipxact:addressOffset>0</ipxact:addressOffset>"
                    "<ipxact:range>8</ipxact:range>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension>testValue</testExtension>"
		            "</ipxact:vendorExtensions>"
                "</ipxact:region>"
                "<ipxact:region>"
                    "<ipxact:name>secondRegion</ipxact:name>"
                    "<ipxact:displayName>r2</ipxact:displayName>"
                    "<ipxact:addressOffset>8</ipxact:addressOffset>"
                    "<ipxact:range>16</ipxact:range>"
                "</ipxact:region>"
            "</ipxact:regions>"
        "</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteAUB2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteAUB2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testCPU_->setAddressUnitBits("32");

	QString expectedOutput(
		"<ipxact:cpu>"
    		"<ipxact:name>testCpu</ipxact:name>"
	    	"<ipxact:addressUnitBits>32</ipxact:addressUnitBits>"
		"</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteExecutableImages2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteExecutableImages2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<ExecutableImage> firstImage(new ExecutableImage);
    firstImage->setName("image1");
    firstImage->setDisplayName("d1");
    firstImage->setShortDescription("brief");
    firstImage->setDescription("description text");
    firstImage->setImageId("id1");
    firstImage->setImageType("binary");

	testCPU_->getExecutableImages()->append(firstImage);

	QSharedPointer<Parameter> parameter(new Parameter());
	parameter->setName("param1");
	parameter->setValueId("id1");
	parameter->setValueResolve("user");
	parameter->setValue("1");

    firstImage->getParameters()->append(parameter);


	QSharedPointer<FileBuilder> fileBuilder(new FileBuilder);
    fileBuilder->setFileType("cSource");
    fileBuilder->setCommand("gcc");
    fileBuilder->setFlags("-O2");
    fileBuilder->setReplaceDefaultFlags("1");

    QSharedPointer<LanguageTools> languageTools(new LanguageTools);
    languageTools->getFileBuilders()->append(fileBuilder);
	languageTools->setLinker("ld");
	languageTools->setLinkerFlags("-flag");

    auto linkerCommandFile = languageTools->getLinkerCommandFile();
    linkerCommandFile->name_ = "./link.cmd";
    linkerCommandFile->commandLineSwitch_ = "-f";
    linkerCommandFile->enable_ = "1";
    linkerCommandFile->generatorRefs_.append("gen1");
    linkerCommandFile->generatorRefs_.append("gen2");
	linkerCommandFile->getVendorExtensions()->append(
		QSharedPointer<VendorExtension>(new Kactus2Value("testExtension", "testValue")));

    firstImage->setLanguageTools(languageTools);

	QString expectedOutput(
		"<ipxact:cpu>"
            "<ipxact:name>testCpu</ipxact:name>"
            "<ipxact:executableImage imageId=\"id1\" imageType=\"binary\">"
                "<ipxact:name>image1</ipxact:name>"
                "<ipxact:displayName>d1</ipxact:displayName>"
                "<ipxact:shortDescription>brief</ipxact:shortDescription>"
                "<ipxact:description>description text</ipxact:description>"
                "<ipxact:parameters>"
                    "<ipxact:parameter parameterId=\"id1\" resolve=\"user\">"
                        "<ipxact:name>param1</ipxact:name>"
                        "<ipxact:value>1</ipxact:value>"
                    "</ipxact:parameter>"
                "</ipxact:parameters>"
                "<ipxact:languageTools>"
                   "<ipxact:fileBuilder>"
                        "<ipxact:fileType>cSource</ipxact:fileType>"
                        "<ipxact:command>gcc</ipxact:command>"
                        "<ipxact:flags>-O2</ipxact:flags>"
                        "<ipxact:replaceDefaultFlags>1</ipxact:replaceDefaultFlags>"
                    "</ipxact:fileBuilder>"
                    "<ipxact:linker>ld</ipxact:linker>"
                    "<ipxact:linkerFlags>-flag</ipxact:linkerFlags>"
                    "<ipxact:linkerCommandFile>"
                        "<ipxact:name>./link.cmd</ipxact:name>"
                        "<ipxact:commandLineSwitch>-f</ipxact:commandLineSwitch>"
                        "<ipxact:enable>1</ipxact:enable>"
                        "<ipxact:generatorRef>gen1</ipxact:generatorRef>"
                        "<ipxact:generatorRef>gen2</ipxact:generatorRef>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension>testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:linkerCommandFile>"
                "</ipxact:languageTools>"
            "</ipxact:executableImage>"
        "</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteMemoryMapReference2022()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteMemoryMapReference2022()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testCPU_->setMemoryMapReference("map1");

	QString expectedOutput(
		"<ipxact:cpu>"
    		"<ipxact:name>testCpu</ipxact:name>"
	    	"<ipxact:memoryMapRef>map1</ipxact:memoryMapRef>"
		"</ipxact:cpu>");

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std22);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteVendorExtension()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteVendorExtension()
{
	QDomDocument document;
	QDomElement extensionNode = document.createElement("testExtension");
	extensionNode.setAttribute("testAttribute", "true");
	extensionNode.appendChild(document.createTextNode("testValue"));

	QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

	testCPU_->getVendorExtensions()->append(testExtension);

	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QString expectedOutput(
		"<ipxact:cpu>"
		    "<ipxact:name>testCpu</ipxact:name>"
		    "<ipxact:vendorExtensions>"
		        "<testExtension testAttribute=\"true\">testValue</testExtension>"
		    "</ipxact:vendorExtensions>"
		"</ipxact:cpu>"
		);

	CPUWriter::writeCPU(xmlStreamWriter, testCPU_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_CPUWriter)

#include "tst_CPUWriter.moc"
