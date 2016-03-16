//-----------------------------------------------------------------------------
// File: tst_FileSetWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Unit test for class FileSetWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FileSetWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_FileSetWriter : public QObject
{
    Q_OBJECT

public:
    tst_FileSetWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleFileSet();
    void writeGroups();
    void writeFiles();
    void writeDefaultFileBuilders();
    void writeDependencies();
    void writeFunctions();

    void writeID();
    void writeVendorExtensions();

private:

    QSharedPointer<FileSet> testFileSet_;
};

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::tst_FileSetWriter()
//-----------------------------------------------------------------------------
tst_FileSetWriter::tst_FileSetWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::init()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::init()
{
    testFileSet_ = QSharedPointer<FileSet> (new FileSet("testFileSet"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::cleanup()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeSimpleFileSet()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeSimpleFileSet()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    
    testFileSet_->setDisplayName("fileSetDisplay");
    testFileSet_->setDescription("fileSetDescription");

    expectedOutput = 
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:displayName>fileSetDisplay</ipxact:displayName>"
            "<ipxact:description>fileSetDescription</ipxact:description>"
        "</ipxact:fileSet>"
        ;

    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeGroups()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeGroups()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFileSet_->getGroups()->append("firstGroup");

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:group>firstGroup</ipxact:group>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeFiles()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeFiles()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<File> testFile (new File("./test/file", "userFile"));
    testFileSet_->getFiles()->append(testFile);

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:file>"
                "<ipxact:name>./test/file</ipxact:name>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
            "</ipxact:file>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeDefaultFileBuilders()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeDefaultFileBuilders()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<FileBuilder> defaultFileBuilder (new FileBuilder("asmSource"));
    defaultFileBuilder->setCommand("4+5+6-14");

    testFileSet_->getDefaultFileBuilders()->append(defaultFileBuilder);

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType>asmSource</ipxact:fileType>"
                "<ipxact:command>4+5+6-14</ipxact:command>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    defaultFileBuilder->setFileType("userFile");
    defaultFileBuilder->setFlags("2-1");
    defaultFileBuilder->setReplaceDefaultFlags("0");

    expectedOutput =
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
                "<ipxact:command>4+5+6-14</ipxact:command>"
                "<ipxact:flags>2-1</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>0</ipxact:replaceDefaultFlags>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:fileSet>"
        ;

    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeDependencies()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeDependencies()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFileSet_->addDependency("Stan");
    testFileSet_->addDependency("Ollie");

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:dependency>Stan</ipxact:dependency>"
            "<ipxact:dependency>Ollie</ipxact:dependency>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeFunctions()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeFunctions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<NameValuePair> argument (new NameValuePair("ninja", "ninjutsu"));
    argument->setDisplayName("shinobi");
    argument->setDescription("oniwaban");
    argument->getVendorExtensions()->append(testExtension);

    QSharedPointer<Function::SourceFile> sourceFile (new Function::SourceFile("sourcery", "swObject"));

    QSharedPointer<Function> testFunction (new Function("nineCuts"));
    testFunction->setReplicate(true);
    testFunction->setEntryPoint("uguisubari");
    testFunction->setReturnType("void");
    testFunction->getArguments()->append(argument);
    testFunction->setDisabled("4-2*2+1");
    testFunction->getSourceFiles()->append(sourceFile);

    testFileSet_->getFunctions()->append(testFunction);

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:function replicate=\"true\">"
                "<ipxact:entryPoint>uguisubari</ipxact:entryPoint>"
                "<ipxact:fileRef>nineCuts</ipxact:fileRef>"
                "<ipxact:returnType>void</ipxact:returnType>"
                "<ipxact:argument>"
                    "<ipxact:name>ninja</ipxact:name>"
                    "<ipxact:displayName>shinobi</ipxact:displayName>"
                    "<ipxact:description>oniwaban</ipxact:description>"
                    "<ipxact:value>ninjutsu</ipxact:value>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:argument>"
                "<ipxact:disabled>4-2*2+1</ipxact:disabled>"
                "<ipxact:sourceFile>"
                    "<ipxact:sourceName>sourcery</ipxact:sourceName>"
                    "<ipxact:fileType>swObject</ipxact:fileType>"
                "</ipxact:sourceFile>"
            "</ipxact:function>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeID()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeID()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testFileSet_->setFileSetId("GurrenLagann");

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<kactus2:fileSetId>GurrenLagann</kactus2:fileSetId>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FileSetWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testFileSet_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fileSet>"
        );

    FileSetWriter fileSetWriter;
    fileSetWriter.writeFileSet(xmlStreamWriter, testFileSet_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_FileSetWriter)

#include "tst_FileSetWriter.moc"
