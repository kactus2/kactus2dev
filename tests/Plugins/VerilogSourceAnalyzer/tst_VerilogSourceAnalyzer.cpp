//-----------------------------------------------------------------------------
// File: tst_VerilogSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.04.2015
//
// Description:
// Unit test for class VerilogSourceAnalyzer.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogSourceAnalyzer/VerilogSourceAnalyzer.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/fileset.h>

class tst_VerilogSourceAnalyzer : public QObject
{
    Q_OBJECT

public:
    tst_VerilogSourceAnalyzer();

private slots:
    void cleanupTestCase();

    void testFileTypes();

    void testCalculateHashForNonexistingFile();
    void testCalculateHash();
    void testCalculateHash_data();

    void testFileInclude();
    void testFileInclude_data();

    void testFileIncludeFromFilesetReturnsRelativePath();

    void testAbsoluteFilePath();

    void testSimilarFileNamesInFilesets();

private:
    
    void writeTestFile(QString const& input);
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::tst_VerilogSourceAnalyzer()
//-----------------------------------------------------------------------------
tst_VerilogSourceAnalyzer::tst_VerilogSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::cleanupTestCase()
{
    QFile testInpuFile("test.v");
    if (testInpuFile.exists())
    {
        testInpuFile.remove();
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testFileTypes()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testFileTypes()
{
    VerilogSourceAnalyzer analyzer;

    QStringList supportedFileTypes = analyzer.getSupportedFileTypes();

    QVERIFY(supportedFileTypes.contains("verilogSource"));
    QVERIFY(supportedFileTypes.contains("verilogSource-95"));
    QVERIFY(supportedFileTypes.contains("verilogSource-2001"));
    QCOMPARE(supportedFileTypes.count(), 3);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testCalculateHashForNonexistingFile()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testCalculateHashForNonexistingFile()
{
    VerilogSourceAnalyzer analyzer;
    QString fileHash = analyzer.calculateHash("noFile.v");
    
    QVERIFY(fileHash.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testCalculateHash()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testCalculateHash()
{   
    QFETCH(QString, input);
    QFETCH(QString, equivalentInput);

    writeTestFile(input);

    QCryptographicHash sha1Hash(QCryptographicHash::Sha1);
    sha1Hash.addData(equivalentInput.toLatin1());
    QString expectedHash = sha1Hash.result().toHex();

    VerilogSourceAnalyzer analyzer;
    QString fileHash = analyzer.calculateHash("test.v");

    QVERIFY(fileHash == expectedHash);

    QVERIFY(QFile::remove("test.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testCalculateHash_data()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testCalculateHash_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("equivalentInput");

    QTest::newRow("module declaration") << 
        "module test(input clk);\n"
        "endmodule\n\n\n" << 
        "module test(input clk); endmodule";

    QTest::newRow("module declaration with whitespaces") << 
        "module   test    (input     clk);    \n\n\n\n"
        "endmodule\n" << 
        "module test (input clk); endmodule";

    QTest::newRow("module declaration with comments") << 
        "// Module starts here.\n"
        "module test(\n"
        "// Ports:\n"
        "input clk);\n"
        "endmodule\n"
        "// Module ends here.\n" << 
        "module test( input clk); endmodule";

    QTest::newRow("module declaration with multi-line comment") << 
        "/* Module test:*/\n"
        "module test(\n"
        "/* Port are defined on the following lines:\n"
        "*/\n"
        "input clk);\n"
        "endmodule\n"
        << 
        "module test( input clk); endmodule";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testFileInclude()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testFileInclude()
{
    QFETCH(QString, input);
    QFETCH(int, expectedDependencyCount);
    QFETCH(QString, firstDependentFileName);

    QSharedPointer<Component> targetComponent(new Component);

    writeTestFile(input);

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), ".", "test.v");

    QCOMPARE(dependencies.count(), expectedDependencyCount);
    
    if (expectedDependencyCount > 0)
    {   
        FileDependencyDesc includeDependency = dependencies.first();
        QCOMPARE(includeDependency.filename, firstDependentFileName);
    }

    QVERIFY(QFile::remove("test.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testFileInclude_data()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testFileInclude_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<int>("expectedDependencyCount");
    QTest::addColumn<QString>("firstDependentFileName");

    QTest::newRow("empty input") << "" << 0 << "";
    
    QTest::newRow("single include in quotes") << "`include \"definitions.v\"" << 1 << "definitions.v";
    
    QTest::newRow("multiple includes in quotes") << 
        "`include \"globals.v\"\n"
        "`include \"variables.v\"\n"
        "`include \"parameters.v\"\n"
        << 3 << "globals.v";

    QTest::newRow("single include in angle brackets") << "`include <definitions.v>" << 1 << "definitions.v";

    QTest::newRow("multiple includes in angle brackets") << 
        "`include <globals.v>\n"
        "`include <variables.v>\n"
        "`include <parameters.v>\n"
        << 3 << "globals.v";

   QTest::newRow("includes in comments") << 
       "//`include \"globals.v\"\n"
       "`include \"variables.v\"\n"
       "/*`include \"parameters.v\"\n"
       "`include \"other.v\"*/"
       << 1 << "variables.v";

   QString absolutePath = QFileInfo("test.v").absoluteFilePath();
   QString unixPath = absolutePath.replace('\\', '/');
   QTest::newRow("full path") << "`include \"" + absolutePath + "\"" << 1 << "test.v";
   QTest::newRow("full unix path") << "`include \"" + unixPath + "\"" << 1 << "test.v";

   QTest::newRow("relative path include") << "`include \"global/definitions.v\"" << 1 << "global/definitions.v";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testFileIncludeWithRelativePath()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testFileIncludeFromFilesetReturnsRelativePath()
{
    QSharedPointer<Component> targetComponent(new Component);
    QSharedPointer<FileSet> targetFileSet = targetComponent->getFileSet("includes");
    targetFileSet->addFile("../includes/globals.v", QSettings());

    writeTestFile("`include \"globals.v\"\n");

    QDir componentDirectory(QFileInfo("test.v").absolutePath());
    componentDirectory.mkpath("ipxact");
    componentDirectory.cd("ipxact");

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), 
        componentDirectory.absolutePath(), "../test.v");

    QCOMPARE(dependencies.count(), 1);

    FileDependencyDesc includeDependency = dependencies.first();
    QCOMPARE(includeDependency.filename, QString("includes/globals.v"));

    QVERIFY(QFile::remove("test.v"));

    componentDirectory.cdUp();
    QVERIFY(componentDirectory.rmdir("ipxact"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testAbsoluteFilePath()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testAbsoluteFilePath()
{
   QSharedPointer<Component> targetComponent(new Component);

    writeTestFile("`include \"globals.v\"\n");

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), 
        ".", QFileInfo("test.v").absoluteFilePath());

    QCOMPARE(dependencies.count(), 1);

    FileDependencyDesc includeDependency = dependencies.first();
    QCOMPARE(includeDependency.filename, QString("globals.v"));

    QVERIFY(QFile::remove("test.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testSimilarFileNamesInFilesets()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testSimilarFileNamesInFilesets()
{
    QSharedPointer<Component> targetComponent(new Component);
    QSharedPointer<FileSet> wrongFileSet = targetComponent->getFileSet("wrong");
    wrongFileSet->addFile("includes/my_globals.v", QSettings());
    QSharedPointer<FileSet> targetFileSet = targetComponent->getFileSet("includes");
    targetFileSet->addFile("globals.v", QSettings());

    writeTestFile("`include \"globals.v\"\n");

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), 
        ".", "test.v");

    QCOMPARE(dependencies.count(), 1);

    FileDependencyDesc includeDependency = dependencies.first();
    QCOMPARE(includeDependency.filename, QString("globals.v"));

    QVERIFY(QFile::remove("test.v"));

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::writeTestFile()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::writeTestFile(QString const& input)
{
    QFile inputFile("test.v", this);
    QVERIFY(inputFile.open(QIODevice::WriteOnly));
    inputFile.write(input.toLatin1());
    inputFile.close();
}

QTEST_MAIN(tst_VerilogSourceAnalyzer)

#include "tst_VerilogSourceAnalyzer.moc"
