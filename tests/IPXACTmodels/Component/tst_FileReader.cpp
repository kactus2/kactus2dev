//-----------------------------------------------------------------------------
// File: tst_FileReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Unit test for class FileReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/VendorExtension.h>

#include <IPXACTmodels/Component/FileReader.h>

#include <QtTest>

class tst_FileReader : public QObject
{
    Q_OBJECT

public:
    tst_FileReader();

private slots:

    void readSimpleFile();
    void readFileAttributes();

    void readFileIsPresent();
    void readIsStructural();
    void readIsIncludeFile();
    void readLogicalName();
    void readExportedNames();
    void readBuildCommand();
    void readDependencies();
    void readDefines();
    void readImageTypes();

    void readHashExtension();
    void readVendorExtensions();
    void readDescription();
};

//-----------------------------------------------------------------------------
// Function: tst_FileReader::tst_FileReader()
//-----------------------------------------------------------------------------
tst_FileReader::tst_FileReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readSimpleFile()
//-----------------------------------------------------------------------------
void tst_FileReader::readSimpleFile()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->name(), QString("./testFile"));
    QCOMPARE(testFile->getFileTypes()->size(), 1);
    QCOMPARE(testFile->getFileTypes()->first(), QString("vhdlSource"));

    documentContent =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:fileType user=\"testUserFileType\">user</ipxact:fileType>"
        "</ipxact:file>"
        ;

    document.setContent(documentContent);
    fileNode = document.firstChildElement("ipxact:file");
    testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->name(), QString("./testFile"));
    QCOMPARE(testFile->getFileTypes()->size(), 2);
    QCOMPARE(testFile->getFileTypes()->first(), QString("vhdlSource"));
    QCOMPARE(testFile->getFileTypes()->last(), QString("testUserFileType"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readFileAttributes()
//-----------------------------------------------------------------------------
void tst_FileReader::readFileAttributes()
{
    QString documentContent(
        "<ipxact:file fileId=\"raiden\" otherAttribute=\"lightning\">"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getFileId(), QString("raiden"));
    QCOMPARE(testFile->getAttributes().count(), 1);
    QCOMPARE(testFile->getAttributes().firstKey(), QString("otherAttribute"));
    QCOMPARE(testFile->getAttributes().first(), QString("lightning"));

    QCOMPARE(testFile->name(), QString("./testFile"));
    QCOMPARE(testFile->getFileTypes()->size(), 1);
    QCOMPARE(testFile->getFileTypes()->first(), QString("vhdlSource"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readFileIsPresent()
//-----------------------------------------------------------------------------
void tst_FileReader::readFileIsPresent()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:isPresent>4*4/4-3</ipxact:isPresent>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
        "</ipxact:file>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->name(), QString("./testFile"));
    QCOMPARE(testFile->getIsPresent(), QString("4*4/4-3"));
    QCOMPARE(testFile->getFileTypes()->size(), 1);
    QCOMPARE(testFile->getFileTypes()->first(), QString("vhdlSource"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readIsStructural()
//-----------------------------------------------------------------------------
void tst_FileReader::readIsStructural()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isStructural>true</ipxact:isStructural>"
        "</ipxact:file>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->isStructural(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readIsIncludeFile()
//-----------------------------------------------------------------------------
void tst_FileReader::readIsIncludeFile()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isIncludeFile>true</ipxact:isIncludeFile>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->isIncludeFile(), true);
    QCOMPARE(testFile->hasExternalDeclarations(), false);

    documentContent =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:isIncludeFile externalDeclarations=\"true\">true</ipxact:isIncludeFile>"
        "</ipxact:file>"
        ;

    document.setContent(documentContent);
    fileNode = document.firstChildElement("ipxact:file");
    testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->isIncludeFile(), true);
    QCOMPARE(testFile->hasExternalDeclarations(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readLogicalName()
//-----------------------------------------------------------------------------
void tst_FileReader::readLogicalName()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:logicalName>Spock</ipxact:logicalName>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getLogicalName(), QString("Spock"));
    QCOMPARE(testFile->isLogicalNameDefault(), false);

    documentContent =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:logicalName default=\"true\">Spock</ipxact:logicalName>"
        "</ipxact:file>"
        ;

    document.setContent(documentContent);
    fileNode = document.firstChildElement("ipxact:file");
    testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getLogicalName(), QString("Spock"));
    QCOMPARE(testFile->isLogicalNameDefault(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readExportedNames()
//-----------------------------------------------------------------------------
void tst_FileReader::readExportedNames()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:exportedName>Enterprise</ipxact:exportedName>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getExportedNames()->size(), 1);
    QCOMPARE(testFile->getExportedNames()->first(), QString("Enterprise"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readBuildCommand()
//-----------------------------------------------------------------------------
void tst_FileReader::readBuildCommand()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:buildCommand>"
                "<ipxact:command>engage</ipxact:command>"
                "<ipxact:flags append=\"false\">testFlags</ipxact:flags>"
                "<ipxact:replaceDefaultFlags>4+3-6</ipxact:replaceDefaultFlags>"
                "<ipxact:targetName>target</ipxact:targetName>"
            "</ipxact:buildCommand>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getBuildCommand().isNull(), false);

    QSharedPointer<BuildCommand> buildCommand = testFile->getBuildCommand();
    QCOMPARE(buildCommand->getCommand(), QString("engage"));
    QCOMPARE(buildCommand->getFlags(), QString("testFlags"));
    QCOMPARE(buildCommand->getFlagsAppend(), QString("false"));
    QCOMPARE(buildCommand->getReplaceDefaultFlags(), QString("4+3-6"));
    QCOMPARE(buildCommand->getTargetName(), QString("target"));

    documentContent =
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:buildCommand>"
                "<ipxact:flags>testFlags</ipxact:flags>"
            "</ipxact:buildCommand>"
        "</ipxact:file>"
        ;
    document.setContent(documentContent);
    fileNode = document.firstChildElement("ipxact:file");
    testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getBuildCommand().isNull(), false);

    buildCommand = testFile->getBuildCommand();
    QCOMPARE(buildCommand->getFlags(), QString("testFlags"));
    QCOMPARE(buildCommand->getFlagsAppend(), QString(""));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readDependencies()
//-----------------------------------------------------------------------------
void tst_FileReader::readDependencies()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:dependency>/path/to/include</ipxact:dependency>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getDependencies()->size(), 1);
    QCOMPARE(testFile->getDependencies()->first(), QString("/path/to/include"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readDefines()
//-----------------------------------------------------------------------------
void tst_FileReader::readDefines()
{
    QString documentContent(
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
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getDefines()->size(), 1);
    QSharedPointer<NameValuePair> define = testFile->getDefines()->first();
    QCOMPARE(define->name(), QString("defined"));
    QCOMPARE(define->displayName(), QString("defineDisplay"));
    QCOMPARE(define->description(), QString("defineDescription"));
    QCOMPARE(define->getValue(), QString("value"));

    QCOMPARE(define->getVendorExtensions()->size(), 1);
    QCOMPARE(define->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readImageTypes()
//-----------------------------------------------------------------------------
void tst_FileReader::readImageTypes()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:imageType>jpg</ipxact:imageType>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getImageTypes()->size(), 1);
    QCOMPARE(testFile->getImageTypes()->first(), QString("jpg"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readHashExtension()
//-----------------------------------------------------------------------------
void tst_FileReader::readHashExtension()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:vendorExtensions>"
                "<kactus2:hash>hash</kactus2:hash>"
            "</ipxact:vendorExtensions>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getVendorExtensions()->size(), 1);
    QCOMPARE(testFile->getLastHash(), QString("hash"));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readDescription()
//-----------------------------------------------------------------------------
void tst_FileReader::readDescription()
{
    QString documentContent(
        "<ipxact:file>"
        "<ipxact:name>./testFile</ipxact:name>"
        "<ipxact:fileType>cppSource</ipxact:fileType>"
        "<ipxact:description>This is an important file.</ipxact:description>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getDescription(), QString("This is an important file."));
}

//-----------------------------------------------------------------------------
// Function: tst_FileReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FileReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:file>"
            "<ipxact:name>./testFile</ipxact:name>"
            "<ipxact:fileType>vhdlSource</ipxact:fileType>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:file>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fileNode = document.firstChildElement("ipxact:file");

    FileReader fileReader;
    QSharedPointer<File> testFile = fileReader.createFileFrom(fileNode);

    QCOMPARE(testFile->getVendorExtensions()->size(), 1);
    QCOMPARE(testFile->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_FileReader)

#include "tst_FileReader.moc"
