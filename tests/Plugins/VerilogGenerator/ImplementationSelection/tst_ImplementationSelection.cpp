//-----------------------------------------------------------------------------
// File: tst_ImplementationSelection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 19.01.2017
//
// Description:
// Unit test for Verilog implementation selection.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>

class tst_ImplementationSelection : public QObject
{
    Q_OBJECT

public:
    tst_ImplementationSelection();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:

	void testImplementationSelection();
    void testImplementationSelectionWithEvilComments();
	void testImplementationSelectionWithTag();
	void testImplementationSelectionWithoutParameters();
	void testImplementationSelectionWithoutPorts();
	void testImplementationSelectionWithInstantiation();
	void testImplementationSelectionWithPostModule();
	void testImplementationSelectionWithTooManyModules();
	void testImplementationSelectionWithNoModuleHeaderStart();
	void testImplementationSelectionWithNoModuleHeaderEnd();
	void testImplementationSelectionWithNoModuleEnd();

private:


    //! The test mock for library interface.
    LibraryMock library_;
};

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::tst_ImplementationSelection()
//-----------------------------------------------------------------------------
tst_ImplementationSelection::tst_ImplementationSelection(): library_(this)
{

}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::initTestCase()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::init()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::init()
{
    library_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::cleanup()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::cleanup()
{
    QFile::remove("./generatorOutput.v");
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelection()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelection()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithEvilComments()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithEvilComments()
{
    QFile existingFile("./generatorOutput.v");

    existingFile.open(QIODevice::WriteOnly);
    QTextStream outputStream(&existingFile);

    QString content = QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8, // module ending );\n"
        "    parameter                              freq             = 100000 /* ending module header );\n"
        ") (\n"
        "    // These ports are not in any interface\n"         
        "    input                               clk,\n"
        "    input          [7:0]                dataIn,\n"
        "    input                               rst_n,\n"
        "    output         [7:0]                dataOut\n"
        ");\n"
        "foo\n"
        "bar\n"
        "endmodule\n"
        ); 

    outputStream << content;

    existingFile.close();

    

    QString implementation;
    QString postModule;
    QString error;

    VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

    implementation = implementation.trimmed();

    QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithTag()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithTag()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n" 
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithoutParameters()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithoutParameters()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent(\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"dataOut <= rst_n & clk\n"
		"bar\n"
		"baz\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("dataOut <= rst_n & clk\nbar\nbaz"));
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithoutPorts()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithoutPorts()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent();\n"
		"dataOut <= rst_n & clk\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("dataOut <= rst_n & clk"));
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithInstantiation()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithInstantiation()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n");

	QString expectedImplementation = QString(
		"foo\n"
		"bar\n"
		"\n"
		"// IP-XACT VLNV: tut.fi:ip.hw:TestIntitializer:1.0\n"
		"TestIntitializer #(\n"
		".WAIT_TIME           (2100))\n"
		"TestIntitializer_0(\n"
		"// These ports are not in any interface\n"
		".done                (SampleHW_0_done_to_TestIntitializer_0_done),\n"
		".clk                 (TestIntitializer_0_clk_to_SampleHW_0_clk),\n"
		".rst                 (SampleHW_0_rst_to_TestIntitializer_0_rst),\n"
		".start\n"
		");");

	outputStream << content << expectedImplementation << "endmodule";

	existingFile.close();

	

	QString actualImplementation;
    QString postModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", actualImplementation, postModule, error);

	actualImplementation = actualImplementation.trimmed();

	QCOMPARE(actualImplementation,expectedImplementation);
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithPostModule()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithPostModule()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	QString expectedPostModule = QString(
		"lrem ipsum\n"
		"    parameter                              bbb        = 64,\n"
		"    parameter                              two        = bbb * 2 \n"
		"// Bogus copy paste stuff\n"
		"foo\n"
		"bar"
		); 

	outputStream << content << endl << expectedPostModule;

	existingFile.close();

	

	QString implementation;
    QString actualPostModule;
    QString error;

	VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, actualPostModule, error);

	implementation = implementation.trimmed();
	actualPostModule = actualPostModule.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
	QCOMPARE(actualPostModule,expectedPostModule);
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithTooManyModules()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithTooManyModules()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	QString extraModule = QString(
		"module MasterComponent #(\n"
		"    parameter                              bbb        = 64,\n"
		"    parameter                              two        = bbb * 2 \n"
		") (\n"        
		"    input          [7:0]                iii,\n"
		"    output         [7:0]                aaa\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	outputStream << content << endl << extraModule;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	bool success = VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithNoModuleHeaderStart()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithNoModuleHeaderStart()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	bool success = VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithNoModuleHeaderEnd()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithNoModuleHeaderEnd()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		"\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	bool success = VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_ImplementationSelection::testImplementationSelectionWithNoModuleEnd()
//-----------------------------------------------------------------------------
void tst_ImplementationSelection::testImplementationSelectionWithNoModuleEnd()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"\n"
		"foo\n"
		"bar\n"
		); 

	outputStream << content;

	existingFile.close();

	

	QString implementation;
    QString postModule;
    QString error;

	bool success = VerilogSyntax::selectImplementation("./generatorOutput.v", implementation, postModule, error);

	QVERIFY(!success);
}

QTEST_APPLESS_MAIN(tst_ImplementationSelection)

#include "tst_ImplementationSelection.moc"
