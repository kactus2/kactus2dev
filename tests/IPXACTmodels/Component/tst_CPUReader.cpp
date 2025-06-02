//-----------------------------------------------------------------------------
// File: tst_CPUReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for class CPUReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/CPUReader.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_CPUReader : public QObject
{
    Q_OBJECT

public:
    tst_CPUReader();

private slots:

    void testReadNamedCPU();
    void testReadNamedCPU2022();

    void testReadIsPresent();
	void testReadParameters();

    void testReadAddressSpaceReference();

    void testReadBlockSize2022();

    void testReadRegions2022();

    void testReadAUB2022();

    void testReadExecutableImages2022();

    void testReadMemoryMapReference2022();

    void testReadVendorExtension();
};

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::tst_CPUReader()
//-----------------------------------------------------------------------------
tst_CPUReader::tst_CPUReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadNamedCPU()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadNamedCPU()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:displayName>cpuDisplay</ipxact:displayName>"
            "<ipxact:description>cpuDescription</ipxact:description>"
        "</ipxact:cpu>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std14);

    QCOMPARE(testCPU->name(), QString("testCPU"));
    QCOMPARE(testCPU->displayName(), QString("cpuDisplay"));
    QCOMPARE(testCPU->description(), QString("cpuDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadNamedCPU2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadNamedCPU2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:displayName>cpuDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>brief description</ipxact:shortDescription>"
            "<ipxact:description>longer description</ipxact:description>"
        "</ipxact:cpu>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);

    QCOMPARE(testCPU->name(), QString("testCPU"));
    QCOMPARE(testCPU->displayName(), QString("cpuDisplay"));
    QCOMPARE(testCPU->shortDescription(), QString("brief description"));
    QCOMPARE(testCPU->description(), QString("longer description"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:cpu>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std14);

    QCOMPARE(testCPU->name(), QString("testCPU"));
    QCOMPARE(testCPU->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:cpu>"
    		"<ipxact:name>testCPU</ipxact:name>"
	    	"<ipxact:isPresent>1</ipxact:isPresent>"
		    "<ipxact:parameters>"
        		"<ipxact:parameter parameterId=\"id1\" prompt=\"Parm 1\" type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>param1</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>5</ipxact:value>"
        		"</ipxact:parameter>"
        		"<ipxact:parameter parameterId=\"id2\" prompt=\"Parm 1\" type=\"shortint\" resolve=\"user\">"
            		"<ipxact:name>param2</ipxact:name>"
            		"<ipxact:description>First generator parameter.</ipxact:description>"
            		"<ipxact:value>1337</ipxact:value>"
        		"</ipxact:parameter>"
    		"</ipxact:parameters>"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std14);

	QCOMPARE(testCPU->name(), QString("testCPU"));
	QCOMPARE(testCPU->getIsPresent(), QString("1"));
	QCOMPARE(testCPU->getParameters()->size(), 2);
	QCOMPARE(testCPU->getParameters()->first()->name(), QString("param1"));
	QCOMPARE(testCPU->getParameters()->first()->getValueId(), QString("id1"));
	QCOMPARE(testCPU->getParameters()->first()->getValue(), QString("5"));
	QCOMPARE(testCPU->getParameters()->last()->name(), QString("param2"));
	QCOMPARE(testCPU->getParameters()->last()->getValueId(), QString("id2"));
	QCOMPARE(testCPU->getParameters()->last()->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadAddressSpaceReference()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadAddressSpaceReference()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>CPU</ipxact:name>"
            "<ipxact:addressSpaceRef addressSpaceRef=\"default_interconnect\">"
                "<ipxact:isPresent>Balduran</ipxact:isPresent>"
            "</ipxact:addressSpaceRef>"
        "</ipxact:cpu>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std14);

    QCOMPARE(testCPU->getAddressSpaceReferences()->size(), 1);
    QCOMPARE(testCPU->getAddressSpaceReferences()->first()->getAddressSpaceRef(), QString("default_interconnect"));
    QCOMPARE(testCPU->getAddressSpaceReferences()->first()->getIsPresent(), QString("Balduran"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadBlockSize2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadBlockSize2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
            "<ipxact:range>16</ipxact:range>"
            "<ipxact:width>32</ipxact:width>"
        "</ipxact:cpu>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);

    QCOMPARE(testCPU->getRange(), QString("16"));
    QCOMPARE(testCPU->getWidth(), QString("32"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadRegions2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadRegions2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>testCPU</ipxact:name>"
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
        "</ipxact:cpu>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);

    QCOMPARE(testCPU->getRegions()->count(), 2);

    QSharedPointer<Region> firstRegion = testCPU->getRegions()->first();
    QCOMPARE(firstRegion->name(), QString("firstRegion"));
    QCOMPARE(firstRegion->displayName(), QString("r1"));
    QCOMPARE(firstRegion->shortDescription(), QString("brief"));
    QCOMPARE(firstRegion->description(), QString("region description text"));
    QCOMPARE(firstRegion->getAddressOffset(), QString("0"));
    QCOMPARE(firstRegion->getRange(), QString("8"));
    QCOMPARE(firstRegion->getVendorExtensions()->count(), 1);

    QSharedPointer<Region> secondRegion = testCPU->getRegions()->last();
    QCOMPARE(secondRegion->name(), QString("secondRegion"));
    QCOMPARE(secondRegion->displayName(), QString("r2"));
    QCOMPARE(secondRegion->shortDescription(), QString());
    QCOMPARE(secondRegion->description(), QString());
    QCOMPARE(secondRegion->getAddressOffset(), QString("8"));
    QCOMPARE(secondRegion->getRange(), QString("16"));
    QCOMPARE(secondRegion->getVendorExtensions()->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadAUB2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadAUB2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>CPU</ipxact:name>"
            "<ipxact:addressUnitBits>32</ipxact:addressUnitBits>"
        "</ipxact:cpu>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);
    QCOMPARE(testCPU->getAddressUnitBits(), QString("32"));

    // No AUB = default 8 bits
    QString documentContentNoAUB(
        "<ipxact:cpu>"
        "<ipxact:name>CPU</ipxact:name>"
        "</ipxact:cpu>"
    );

    document.setContent(documentContentNoAUB);
    cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPUNoAUB = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);
    QCOMPARE(testCPUNoAUB->getAddressUnitBits(), QString("8"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadExecutableImages2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadExecutableImages2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>CPU</ipxact:name>"
            "<ipxact:executableImage imageId=\"id1\" imageType=\"binary\" >"
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
        "</ipxact:cpu>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);

    QCOMPARE(testCPU->getExecutableImages()->count(), 1);
    QSharedPointer<ExecutableImage> firstImage = testCPU->getExecutableImages()->first();

    QCOMPARE(firstImage->name(), QString("image1"));
    QCOMPARE(firstImage->displayName(), QString("d1"));
    QCOMPARE(firstImage->shortDescription(), QString("brief"));
    QCOMPARE(firstImage->description(), QString("description text"));
    QCOMPARE(firstImage->getImageId(), QString("id1"));
    QCOMPARE(firstImage->getImageType(), QString("binary"));

    QCOMPARE(firstImage->getParameters()->count(), 1);

    auto languageTools = firstImage->getLanguageTools();
    QCOMPARE(languageTools->getFileBuilders()->count(), 1);
    auto fileBuilder = languageTools->getFileBuilders()->first();
    QCOMPARE(fileBuilder->getFileType().type_, QString("cSource"));
    QCOMPARE(fileBuilder->getCommand(), QString("gcc"));
    QCOMPARE(fileBuilder->getFlags(), QString("-O2"));
    QCOMPARE(fileBuilder->getReplaceDefaultFlags(), QString("1"));

    QCOMPARE(languageTools->getLinker(), QString("ld"));
    QCOMPARE(languageTools->getLinkerFlags(), QString("-flag"));

    auto linkerCommandFile = languageTools->getLinkerCommandFile();
    QCOMPARE(linkerCommandFile->name_, QString("./link.cmd"));
    QCOMPARE(linkerCommandFile->commandLineSwitch_, QString("-f"));
    QCOMPARE(linkerCommandFile->enable_, QString("1"));
    QCOMPARE(linkerCommandFile->generatorRefs_.count(), 2);
    QCOMPARE(linkerCommandFile->generatorRefs_.first(), QString("gen1"));
    QCOMPARE(linkerCommandFile->generatorRefs_.last(), QString("gen2"));
    QCOMPARE(linkerCommandFile->getVendorExtensions()->count(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadMemoryMapReference2022()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadMemoryMapReference2022()
{
    QString documentContent(
        "<ipxact:cpu>"
            "<ipxact:name>CPU</ipxact:name>"
            "<ipxact:memoryMapRef>map1</ipxact:memoryMapRef>"
        "</ipxact:cpu>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

    QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std22);

    QCOMPARE(testCPU->name(), QString("CPU"));
    QCOMPARE(testCPU->getMemoryMapReference(), QString("map1"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadVendorExtension()
//-----------------------------------------------------------------------------
void tst_CPUReader::testReadVendorExtension()
{
	QString documentContent(
		"<ipxact:cpu>"
		"<ipxact:name>CPU</ipxact:name>"
		"<ipxact:vendorExtensions>"
		    "<testExtension attr=\"ea\"/>"
		"</ipxact:vendorExtensions>"
		"</ipxact:cpu>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode cpuNode = document.firstChildElement("ipxact:cpu");

	QSharedPointer<Cpu> testCPU = CPUReader::createCPUFrom(cpuNode, Document::Revision::Std14);

	QCOMPARE(testCPU->name(), QString("CPU"));
	QCOMPARE(testCPU->getVendorExtensions()->first()->type(), QString("testExtension"));
}


QTEST_APPLESS_MAIN(tst_CPUReader)

#include "tst_CPUReader.moc"
