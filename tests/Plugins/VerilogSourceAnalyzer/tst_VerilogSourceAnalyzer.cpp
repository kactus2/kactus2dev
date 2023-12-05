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

class tst_VerilogSourceAnalyzer : public QObject
{
    Q_OBJECT

public:
    tst_VerilogSourceAnalyzer();

private slots:
    void cleanup();

    void testFileTypes();

    void testCalculateHashForNonexistingFile();
    void testCalculateHash();
    void testCalculateHash_data();

    void testFileInclude();
    void testFileInclude_data();

    void testFileIncludeFromFilesetReturnsRelativePath();

    void testAbsoluteFilePath();

    void testSimilarFileNamesInFilesets();

	void testSubmoduleInstance();
	void testSubmoduleInstance_data();

	void testSubmodulesAreSearchedInFilesets();

	void testMultipleModulesInFilesetFile();

	void testNamedProcessIsNotSubmodule();

private:
    
    void writeTestFile(QString const& content, QString const& fileName);

	QStringList testFiles_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::tst_VerilogSourceAnalyzer()
//-----------------------------------------------------------------------------
tst_VerilogSourceAnalyzer::tst_VerilogSourceAnalyzer(): testFiles_()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::cleanup()
{
	for (auto& testFileName : testFiles_)
	{
		QFile testInpuFile(testFileName);
		if (testInpuFile.exists())
		{
			testInpuFile.remove();
		}
	}

	testFiles_.clear();
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
    QVERIFY(supportedFileTypes.contains("systemVerilogSource"));
    QCOMPARE(supportedFileTypes.count(), 4);
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

    writeTestFile(input, "test.v");

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

    QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

    writeTestFile(input, "test.v");

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
    QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	QSharedPointer<FileSet> targetFileSet(new FileSet("includes"));
	QSharedPointer<File> targetFile(new File((QStringLiteral("../includes/globals.v"))));
	targetFile->addFileType("verilogSource");
	
	targetFileSet->addFile(targetFile);
	targetComponent->getFileSets()->append( targetFileSet );

    writeTestFile("`include \"globals.v\"\n", "test.v");

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
   QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

    writeTestFile("`include \"globals.v\"\n", "test.v");

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), 
        ".", QFileInfo("test.v").absoluteFilePath());

    QCOMPARE(dependencies.count(), 1);

    FileDependencyDesc includeDependency = dependencies.first();
    QCOMPARE(includeDependency.filename, QString("globals.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testSimilarFileNamesInFilesets()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testSimilarFileNamesInFilesets()
{
    QSettings settings;

    QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets = targetComponent->getFileSets();
	QSharedPointer<FileSet> wrongFileSet( new FileSet("wrong") );
	QSharedPointer<FileSet> targetFileSet( new FileSet("includes") );
	wrongFileSet->addFile("../includes/m_globals.v", settings);
	targetFileSet->addFile("globals.v", settings);
	fileSets->append( wrongFileSet );
	fileSets->append( targetFileSet );

    writeTestFile("`include \"globals.v\"\n", "test.v");

    VerilogSourceAnalyzer analyzer;

    QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(), 
        ".", "test.v");

    QCOMPARE(dependencies.count(), 1);

    FileDependencyDesc includeDependency = dependencies.first();
    QCOMPARE(includeDependency.filename, QString("globals.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testSubmoduleInstance()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testSubmoduleInstance()
{
	QFETCH(QString, input);
	QFETCH(QStringList, fileNames);

	QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	writeTestFile(input, "test.v");

	VerilogSourceAnalyzer analyzer;

	QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(),
		".", QFileInfo("test.v").absoluteFilePath());

	for (auto& instantiationDependency : dependencies)
	{
		QString moduleName = instantiationDependency.filename;
		moduleName.remove(QRegularExpression(".(s)?v$"));
		QString error = "No instance " + moduleName + " found with file " + instantiationDependency.filename;		
		QVERIFY2(fileNames.contains(instantiationDependency.filename), error.toLocal8Bit());
		QCOMPARE(instantiationDependency.description, QString("Submodule instantiation of module " + moduleName));
	}

	QCOMPARE(dependencies.count(), fileNames.count());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testFileInclude_data()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testSubmoduleInstance_data()
{
	QTest::addColumn<QString>("input");	
	QTest::addColumn<QStringList>("fileNames");

	QTest::newRow("Simple submodule, no ports") <<
		"module test(input clk);\n"
		"\n"
		"submodule sub_i();\n"		
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Simple submodule, no port connections") <<
		"module test(input clk);\n"
		"\n"
		"submodule sub_i;\n"
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Simple submodule, one port") << 
		"module test(input clk);\n"
		"\n"
		"submodule sub_i(\n"
		"    .clk(clk)\n"
		");\n"
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Simple submodule, multiple ports") <<
		"module test(input clk, input rst, output enable);\n"
		"\n"
		"submodule sub_i(\n"
		"    .clk(clk),\n"
		"    .reset(rst)\n"
		");\n"
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Simple submodule with parameter and wire declarations") <<
		"module test(input clk, input rst, output enable);\n"
		"\n"
		"localparam TWENTY = 10 * 2;"
		"localparam DATA_WIDTH = TWENTY + 12;"
		"\n"
		"logic [31:0] connector;\n"
		"logic        simple_wire;\n"
		"logic [DATA_WIDTH-1:0] wide_wire;\n"
		"\n"
		"submodule sub_i(\n"
		"    .clk(clk),\n"
		"    .reset(rst)\n"
		");\n"
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Simple submodule with preceding assignments") <<
		"module test(input clk, output enable);\n"
		"\n"
		"logic clock_carrier;\n"
		"logic enable_carrier;\n"
		"\n"
		"assign clock_carrier = clk;\n"
		"assign enable = enable_carrier;\n"
		"\n"
		"submodule sub_i(\n"
		"    .clk(clk),\n"
		"    .enable(enable_carrier)\n"
		");"
		"endmodule\n"
		<< QStringList("submodule.v");

	QTest::newRow("Multiple submodules with logic") <<
		"module top(input logic clk, reset, enable,\n"
		"           output logic RE,\n"
		"           output logic WE,\n"
		"           output types::addr_t addr,\n"
		"           output types::rdata_t rdata,\n"
		"           output types::wdata_t wdata);\n"
		"\n"
		"  logic en_data_gen, en_data_gen_b;\n"
		"\n"
		"  intf #(.nmasters(2), .nslaves(2)) bus(.clk,.reset);\n"
		"  data_gen m1(.enable(en_data_gen_b),.bus(bus.master[0].mport));\n"
		"  master m0(.bus(bus.master[1].mport));\n"
		"  slave #(.asize(7)) s0(.bus(bus.slave[0].mport));\n"
		"  slave #(.asize(7)) s1(.bus(bus.slave[1].mport));\n"
		"  counter c0(.clk,.reset,.enable,.q(en_data_gen));\n"
		"\n"
		"  assign addr = bus.addr_a;\n"
		"  assign rdata = bus.rdata_a;\n"
		"  assign wdata = bus.wdata_a;\n"
		"  assign RE = bus.RE_a;\n"
		"  assign WE = bus.WE_a;\n"
		"  assign en_data_gen_b = !en_data_gen;\n"
		"\n"
		"endmodule: top\n"
		<< QStringList({ "intf.v", "data_gen.v", "master.v", "slave.v", "counter.v" });

	QTest::newRow("Instantiating same submodule multiple times") <<
		"module ripple_adder(X, Y, S, Co);\n"
		" input [3:0] X, Y;// Two 4-bit inputs\n"
		" output [3:0] S;\n"
		" output Co;\n"
		" wire w1, w2, w3;\n"
		"\n"
		"//Instantiating 4 1-bit full adders\n"
		"fulladder u1(X[0], Y[0], 1'b0, S[0], w1);\n"
		"fulladder u2(X[1], Y[1], w1, S[1], w2);\n"
		"fulladder u3(X[2], Y[2], w2, S[2], w3);\n"
		"fulladder u4(X[3], Y[3], w3, S[3], Co);\n"
		"endmodule"
		<< QStringList("fulladder.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testSubmodulesAreSearchedInFilesets()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testSubmodulesAreSearchedInFilesets()
{
	QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	writeTestFile(
		"module test();\n"
		"\n"
		"johnson john_i();\n"		
		"endmodule\n", 
		"top.v");

	VerilogSourceAnalyzer analyzer;

	QList<FileDependencyDesc> dependencies = analyzer.getFileDependencies(targetComponent.data(),
		".", QFileInfo("top.v").absoluteFilePath());

	QCOMPARE(dependencies.count(), 1);
	QCOMPARE(dependencies.first().filename, "johnson.v");

	writeTestFile(
		"module johnson();\n"
		"endmodule\n",
		"johnson_counter.v");

	QSharedPointer<File> submoduleFile(new File("johnson_counter.v", "verilogSource"));
	QSharedPointer<FileSet> rtlFileSet(new FileSet("rtl"));
	rtlFileSet->addFile(submoduleFile);
	targetComponent->getFileSets()->append(rtlFileSet);

	QList<FileDependencyDesc> filesetDependencies = analyzer.getFileDependencies(targetComponent.data(),
		".", QFileInfo("top.v").absoluteFilePath());

	QCOMPARE(filesetDependencies.count(), 1);
	QCOMPARE(filesetDependencies.first().filename, "johnson_counter.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testMultipleModulesInFilesetFile()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testMultipleModulesInFilesetFile()
{
	QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	writeTestFile(
		"module test();\n"
		"\n"
		"johnson john_i();\n"
		"endmodule\n",
		"top.v");

	writeTestFile(
		"module counter();\n"
		"endmodule\n"
		"\n"
		"module reverse_counter();\n"
		"endmodule\n"
		"\n"
		"module johnson();\n"
		"endmodule\n",
		"counters.v");

	QSharedPointer<File> submoduleFile(new File("counters.v", "verilogSource"));
	QSharedPointer<FileSet> rtlFileSet(new FileSet("rtl"));
	rtlFileSet->addFile(submoduleFile);
	targetComponent->getFileSets()->append(rtlFileSet);

	VerilogSourceAnalyzer analyzer;
	QList<FileDependencyDesc> filesetDependencies = analyzer.getFileDependencies(targetComponent.data(),
		".", QFileInfo("top.v").absoluteFilePath());

	QCOMPARE(filesetDependencies.count(), 1);
	QCOMPARE(filesetDependencies.first().filename, "counters.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::testNamedProcessIsNotSubmodule()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::testNamedProcessIsNotSubmodule()
{
	QSharedPointer<Component> targetComponent(new Component(VLNV(), Document::Revision::Std14));

	writeTestFile(
		"module johnson #(\n"
		"    parameter WIDTH = 4\n"
		")(\n"
		"    input logic clk,\n"
		"    input logic rst_n"
		"    output logic [WIDTH-1:0] count_out"
		");"
		"always_ff @(posedge clk or negedge rstn_i) begin : count\n"
		"    if (~rst_n) begin\n"
		"        count_out <= 0;\n"
		"    end else begin\n"
		"        count_out[WIDTH-1] <= ~count_out[0];\n"
		"        for (int i = 0; i < WIDTH-1; i=i+i) begin\n"
		"            count_out[i] <= count_out[i+1];\n"
		"        end\n"
		"    end\n"
		"end"
		"endmodule\n",
		"johnson.v");

	VerilogSourceAnalyzer analyzer;
	QList<FileDependencyDesc> filesetDependencies = analyzer.getFileDependencies(targetComponent.data(),
		".", QFileInfo("johnson.v").absoluteFilePath());

	QCOMPARE(filesetDependencies.count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogSourceAnalyzer::writeTestFile()
//-----------------------------------------------------------------------------
void tst_VerilogSourceAnalyzer::writeTestFile(QString const& content, QString const& fileName)
{
    QFile inputFile(fileName, this);
    QVERIFY(inputFile.open(QIODevice::WriteOnly));
    inputFile.write(content.toLatin1());
    inputFile.close();

	testFiles_.append(fileName);
}

QTEST_MAIN(tst_VerilogSourceAnalyzer)

#include "tst_VerilogSourceAnalyzer.moc"
