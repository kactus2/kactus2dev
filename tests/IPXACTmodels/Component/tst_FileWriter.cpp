//-----------------------------------------------------------------------------
// File: tst_FileWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Unit test for class FileWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QDomDocument>
#include <QtTest>

class tst_FileWriter : public QObject
{
    Q_OBJECT

public:
    tst_FileWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleFile();
    void writeFileAttributes();
    
    void writeIsPresent();
    void writeIsStructural();
    void writeIsIncluded();
    void writeLogicalName();
    void writeExportedName();
    void writeBuildCommand();
    void writeDependencies();
    void writeDefines();
    void writeImageTypes();
    void writeDescription();

    void writeHashExtensions();
    void writeVendorExtensions();

private:

    QSharedPointer<File> testFile_;
};

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::tst_FileWriter()
//-----------------------------------------------------------------------------
tst_FileWriter::tst_FileWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_FileWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_FileWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::init()
//-----------------------------------------------------------------------------
void tst_FileWriter::init()
{
    testFile_ = QSharedPointer<File> (new File("./testFile", "vhdlSource"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_FileWriter::cleanup()
{
    testFile_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeSimpleFile()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeSimpleFile()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testFile_->getFileTypes()->append("testUserFileType");
    expectedOutput =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:fileType user=\"testUserFileType\">user</ipxact:fileType>"
        "</ipxact:file>"
        ;

    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testFile_->getFileTypes()->removeLast();
    testFile_->setDescription("describedFile");
    expectedOutput =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:description>describedFile</ipxact:description>"
        "</ipxact:file>"
        ;

    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeFileAttributes()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeFileAttributes()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setFileId("testID");

    QString expectedOutput(
        "<ipxact:file fileId=\"testID\">"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setIsPresent("4*4/4-3");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:isPresent>4*4/4-3</ipxact:isPresent>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeIsStructural()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeIsStructural()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setStructural(true);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isStructural>true</ipxact:isStructural>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeIsIncluded()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeIsIncluded()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setIncludeFile(true);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isIncludeFile>true</ipxact:isIncludeFile>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testFile_->setExternalDeclarations(true);
    expectedOutput = 
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isIncludeFile externalDeclarations=\"true\">true</ipxact:isIncludeFile>"
        "</ipxact:file>"
        ;

    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeLogicalName()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeLogicalName()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setLogicalName("logicalTest");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:logicalName>logicalTest</ipxact:logicalName>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testFile_->setLogicalNameDefault(true);

    testFile_->setExternalDeclarations(true);
    expectedOutput = 
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:logicalName default=\"true\">logicalTest</ipxact:logicalName>"
        "</ipxact:file>"
        ;

    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeExportedName()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeExportedName()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->getExportedNames()->append("externalExport");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:exportedName>externalExport</ipxact:exportedName>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeBuildCommand()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeBuildCommand()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<BuildCommand> testBuildCommand(new BuildCommand());
    testBuildCommand->setCommand("testCommand");
    testBuildCommand->setFlags("testFlags");
    testBuildCommand->setFlagsAppend(true);
    testBuildCommand->setReplaceDefaultFlags("4+3-6");
    testBuildCommand->setTargetName("target");

    testFile_->setBuildcommand(testBuildCommand);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:buildCommand>"
                "<ipxact:command>testCommand</ipxact:command>"
                "<ipxact:flags append=\"true\">testFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>4+3-6</ipxact:replaceDefaultFlags>"
                "<ipxact:targetName>target</ipxact:targetName>"
            "</ipxact:buildCommand>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testBuildCommand->setCommand("");
    testBuildCommand->setReplaceDefaultFlags("");
    testBuildCommand->setTargetName("");
    testBuildCommand->clearFlagsAppend();

    expectedOutput =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:buildCommand>"
                "<ipxact:flags>testFlags</ipxact:flags>"
            "</ipxact:buildCommand>"
        "</ipxact:file>"
        ;

    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeDependencies()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeDependencies()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->getDependencies()->append("/path/to/include");
    testFile_->getDependencies()->append("/path/to/other/include");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:dependency>/path/to/include</ipxact:dependency>"
            "<ipxact:dependency>/path/to/other/include</ipxact:dependency>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeDefines()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeDefines()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<NameValuePair> testDefine (new NameValuePair("defined", "value"));
    testDefine->setDisplayName("defineDisplay");
    testDefine->setDescription("defineDescription");
    testDefine->getVendorExtensions()->append(testExtension);

    QSharedPointer<NameValuePair> otherDefine (new NameValuePair("defineNi", "kachiNi"));

    testFile_->getDefines()->append(testDefine);
    testFile_->getDefines()->append(otherDefine);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:define>"
                "<ipxact:name>defined</ipxact:name>"
                "<ipxact:displayName>defineDisplay</ipxact:displayName>"
                "<ipxact:description>defineDescription</ipxact:description>"
                "<ipxact:value>value</ipxact:value>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:define>"
            "<ipxact:define>"
                "<ipxact:name>defineNi</ipxact:name>"
                "<ipxact:value>kachiNi</ipxact:value>"
            "</ipxact:define>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeImageTypes()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeImageTypes()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->getImageTypes()->append("jpg");
    testFile_->getImageTypes()->append("png");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:imageType>jpg</ipxact:imageType>"
            "<ipxact:imageType>png</ipxact:imageType>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeDescription()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeDescription()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setDescription("This is an important file.");

    QString expectedOutput(
        "<ipxact:file>"
        "<ipxact:name>./testFile</ipxact:name>"
        "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "<ipxact:description>This is an important file.</ipxact:description>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeHashExtensions()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeHashExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFile_->setPendingHash("hash");

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:vendorExtensions>"
                "<kactus2:hash>hash</kactus2:hash>"
            "</ipxact:vendorExtensions>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FileWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testFile_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:file>"
        );

    FileWriter fileWriter;
    fileWriter.writeFile(xmlStreamWriter, testFile_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_FileWriter)

#include "tst_FileWriter.moc"
