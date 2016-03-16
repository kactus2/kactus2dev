//-----------------------------------------------------------------------------
// File: tst_FileSetReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Unit test for class FileSetReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FileSetReader.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_FileSetReader : public QObject
{
    Q_OBJECT

public:
    tst_FileSetReader();

private slots:
    
    void readSimpleFileSet();
    void readGroups();
    void readFiles();
    void readDefaultFileBuilders();
    void readDependencies();
    void readFunctions();

    void readID();
    void readVendorExtensions();
};
 
//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::tst_FileSetReader()
//-----------------------------------------------------------------------------
tst_FileSetReader::tst_FileSetReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readSimpleFileSet()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readSimpleFileSet()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));

    documentContent =
        "<ipxact:fileSet>"
            "<ipxact:name>otherFileSet</ipxact:name>"
            "<ipxact:displayName>fileSetDisplay</ipxact:displayName>"
            "<ipxact:description>fileSetDescription</ipxact:description>"
        "</ipxact:fileSet>"
        ;

    document.setContent(documentContent);
    fileSetNode = document.firstChildElement("ipxact:fileSet");
    testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("otherFileSet"));
    QCOMPARE(testFileSet->displayName(), QString("fileSetDisplay"));
    QCOMPARE(testFileSet->description(), QString("fileSetDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readGroups()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readGroups()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:group>firstGroup</ipxact:group>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));
    QCOMPARE(testFileSet->getGroups()->size(), 1);
    QCOMPARE(testFileSet->getGroups()->first(), QString("firstGroup"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readFiles()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readFiles()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:file>"
                "<ipxact:name>./test/file</ipxact:name>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
                "<ipxact:fileType>executableHdl</ipxact:fileType>"
            "</ipxact:file>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));
    QCOMPARE(testFileSet->getFiles()->size(), 1);

    QSharedPointer<File> testFile = testFileSet->getFiles()->first();
    QCOMPARE(testFile->name(), QString("./test/file"));
    QCOMPARE(testFile->getFileTypes()->size(), 2);
    QCOMPARE(testFile->getFileTypes()->first(), QString("userFile"));
    QCOMPARE(testFile->getFileTypes()->last(), QString("executableHdl"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readDefaultFileBuilders()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readDefaultFileBuilders()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:defaultFileBuilder>"
                "<ipxact:fileType user=\"userFile\">user</ipxact:fileType>"
                "<ipxact:command>4+5+6-14</ipxact:command>"
                "<ipxact:flags>2-1</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>0</ipxact:replaceDefaultFlags>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:defaultFileBuilder>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));

    QCOMPARE(testFileSet->getDefaultFileBuilders()->size(), 1);

    QSharedPointer<FileBuilder> testFileBuilder = testFileSet->getDefaultFileBuilders()->first();
    QCOMPARE(testFileBuilder->getFileType(), QString("user"));
    QCOMPARE(testFileBuilder->getUserFileType(), QString("userFile"));
    QCOMPARE(testFileBuilder->getCommand(), QString("4+5+6-14"));
    QCOMPARE(testFileBuilder->getFlags(), QString("2-1"));
    QCOMPARE(testFileBuilder->getReplaceDefaultFlags(), QString("0"));

    QCOMPARE(testFileBuilder->getVendorExtensions()->size(), 1);
    QCOMPARE(testFileBuilder->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readDependencies()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readDependencies()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:dependency>Stan</ipxact:dependency>"
            "<ipxact:dependency>Ollie</ipxact:dependency>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));
    QCOMPARE(testFileSet->getDependencies()->size(), 2);
    QCOMPARE(testFileSet->getDependencies()->first(), QString("Stan"));
    QCOMPARE(testFileSet->getDependencies()->last(), QString("Ollie"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readFunctions()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readFunctions()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));
    QCOMPARE(testFileSet->getFunctions()->size(), 1);

    QSharedPointer<Function> testFunction = testFileSet->getFunctions()->first();
    QCOMPARE(testFunction->getReplicate(), true);
    QCOMPARE(testFunction->getEntryPoint(), QString("uguisubari"));
    QCOMPARE(testFunction->getFileRef(), QString("nineCuts"));
    QCOMPARE(testFunction->getReturnType(), QString("void"));
    QCOMPARE(testFunction->getDisabled(), QString("4-2*2+1"));

    QCOMPARE(testFunction->getArguments()->size(), 1);
    QSharedPointer<NameValuePair> testArgument = testFunction->getArguments()->first();
    QCOMPARE(testArgument->name(), QString("ninja"));
    QCOMPARE(testArgument->displayName(), QString("shinobi"));
    QCOMPARE(testArgument->description(), QString("oniwaban"));
    QCOMPARE(testArgument->getValue(), QString("ninjutsu"));
    QCOMPARE(testArgument->getVendorExtensions()->size(), 1);
    QCOMPARE(testArgument->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(testFunction->getSourceFiles()->size(), 1);
    QSharedPointer<Function::SourceFile> testSource = testFunction->getSourceFiles()->first();
    QCOMPARE(testSource->getSourceName(), QString("sourcery"));
    QCOMPARE(testSource->getFileType(), QString("swObject"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readID()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readID()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<kactus2:fileSetId>testID</kactus2:fileSetId>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));

    QCOMPARE(testFileSet->getVendorExtensions()->size(), 1);
    QCOMPARE(testFileSet->getVendorExtensions()->first()->type(), QString("kactus2:fileSetId"));
    QCOMPARE(testFileSet->getFileSetId(), QString("testID"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FileSetReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:fileSet>"
            "<ipxact:name>testFileSet</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fileSet>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileSetNode = document.firstChildElement("ipxact:fileSet");

    FileSetReader fileSetReader;
    QSharedPointer<FileSet> testFileSet = fileSetReader.createFileSetFrom(fileSetNode);

    QCOMPARE(testFileSet->name(), QString("testFileSet"));

    QCOMPARE(testFileSet->getVendorExtensions()->size(), 1);
    QCOMPARE(testFileSet->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_FileSetReader)

#include "tst_FileSetReader.moc"
