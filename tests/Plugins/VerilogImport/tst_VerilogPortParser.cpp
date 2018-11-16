//-----------------------------------------------------------------------------
// File: tst_VerilogPortParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.09.2014
//
// Description:
// Unit test for class VerilogPortParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogImport/VerilogPortParser.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/Component/Component.h>

Q_DECLARE_METATYPE(DirectionTypes::Direction)

class tst_VerilogPortParser : public QObject
{
    Q_OBJECT

public:
    tst_VerilogPortParser();

    private slots:
        void init();

        // Test cases:
        void testNothingIsParsedFromMalformedInput();
        void testNothingIsParsedFromMalformedInput_data();

        void testVerilog2001PortIsParsed();
        void testVerilog2001PortIsParsed_data();

        void testVerilog1995PortIsParsed();
        void testVerilog1995PortIsParsed_data();

        void testMultiplePortsAreParsed();
        void testMultiplePortsAreParsed_data();

        void testMultipleVerilog1995PortsAreParsed();
        void testMultipleVerilog1995PortsAreParsed_data();

        void testPortsInSubmodulesAreNotParsed();
        void testPortsInSubmodulesAreNotParsed_data();

        void testEquationAsPortBound();
        void testEquationAsPortBound_data();

        void testArrayPort();
        void testArrayPort_data();

        void testMacroInBoundsIsParsed();
        void testMacroInBoundsIsParsed_data();
private:

    void runParser(QString const& input);   

	QSharedPointer<Component> importComponent_;
	QSharedPointer<ComponentInstantiation> importComponentInstantiation_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::tst_VerilogPortParser()
//-----------------------------------------------------------------------------
tst_VerilogPortParser::tst_VerilogPortParser(): importComponent_(new Component())
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::init()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::init()
{
	importComponent_ = QSharedPointer<Component>(new Component());

	importComponentInstantiation_ = QSharedPointer<ComponentInstantiation>( new ComponentInstantiation );
	importComponent_->getModel()->getComponentInstantiations()->append(importComponentInstantiation_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getPorts()->count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty input") << "";

    QTest::newRow("no module declared") << "// Comment line.";

    QTest::newRow("module without parathesis around port declarations") << 
        "module test\n"
        "input clk\n"
        "endmodule";

    QTest::newRow("module without closing parathesis for port declarations") << 
        "module test(\n"
        "input clk\n"
        "endmodule";

    QTest::newRow("module without endmodule") << 
        "module test(\n"
        "    input clk\n"
        ");";

    QTest::newRow("Unknown direction") <<
        "module test (\n"
        "    in data\n"
        ");\n"
        "endmodule";


    QTest::newRow("Illegal characters in port name") <<
        "module test (\n"
        "    input data%for\n"
        ");\n"
        "endmodule";

    QTest::newRow("Signed 1995-style port.") <<
        "module test (data);\n"
        "    output wire signed [2:0] data; // 2's complement data.\n"
        "\n"
        "endmodule";

    QTest::newRow("Port after endmodule") <<
        "module test (data);\n"
        "endmodule\n"
        "    input data;";

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::runParser(QString const& input)
{
    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(importComponent_));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(finder));

    VerilogPortParser parser;
    parser.setExpressionParser(expressionParser);

    parser.import(input, importComponent_, importComponentInstantiation_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog2001PortIsParsed()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedPortName);
    QFETCH(QString, expectedType);
    QFETCH(DirectionTypes::Direction, expectedDirection);
    QFETCH(QString, expectedLeft);
    QFETCH(QString, expectedRight);
    QFETCH(QString, expectedDescription);

    runParser(input);

    QCOMPARE(importComponent_->getPorts()->count(), 1);

    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();

    QCOMPARE(createdPort->name(), expectedPortName);
    QCOMPARE(createdPort->getTypeName(), expectedType);
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->description(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog2001PortIsParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedPortName");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<DirectionTypes::Direction>("expectedDirection");
    QTest::addColumn<QString>("expectedLeft");
    QTest::addColumn<QString>("expectedRight");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("Simplest possible port") <<
        "module test (\n"
        "    input clk\n"
        ");\n"
        "endmodule"
        << "clk" << "" << DirectionTypes::IN << "" << "" << "";

    QTest::newRow("Typed port") <<
        "module test (\n"
        "    output reg data\n"
        ");\n"
        "endmodule"
        << "data" << "reg" <<  DirectionTypes::OUT << "" << "" << "";

    QTest::newRow("Little-endian port") <<
        "module test (\n"
        "    input [7:0] data\n"
        ");\n"
        "endmodule"
        << "data" << "" <<  DirectionTypes::IN << "7" << "0" << "";

    QTest::newRow("Big-endian port") <<
        "module test (\n"
        "    input [0:7] data\n"
        ");\n"
        "endmodule"
        << "data" << "" <<  DirectionTypes::IN << "0" << "7" << "";

    QTest::newRow("Typed four-bit port") <<
        "module test (\n"
        "    inout tri [3:0] data\n"
        ");\n"
        "endmodule"
        << "data" << "tri" <<  DirectionTypes::INOUT << "3" << "0" << "";

    QTest::newRow("Comment as a description.") <<
        "module test (\n"
        "    input rst_n // Active low reset.\n"
        ");\n"
        "endmodule"
        << "rst_n" << "" <<  DirectionTypes::IN << "" << "" << "Active low reset.";

    QTest::newRow("Signed port.") <<
        "module test (\n"
        "    output wire signed [2:0] data // 2's complement data.\n"
        ");\n"
        "endmodule"
        << "data" << "wire" <<  DirectionTypes::OUT << "2" << "0" << "2's complement data.";

    QTest::newRow("Additional whitespaces in port declaration") <<
        "module test (output \n"
        "reg\n"
        "\n"
        "    [1:0] \n"
        "    cs    //Chip select\n"
        ");\n"
        "endmodule"
        << "cs" << "reg" <<  DirectionTypes::OUT << "1" << "0" << "Chip select";

    QTest::newRow("Module with parameters.") <<
        "module test #(parameter id = 1) (\n"
        "    input clk\n"
        ");\n"
        "endmodule"
        << "clk" << "" <<  DirectionTypes::IN << "" << "" << "";

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testVerilog1995PortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog1995PortIsParsed()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedPortName);
    QFETCH(QString, expectedType);
    QFETCH(DirectionTypes::Direction, expectedDirection);
    QFETCH(QString, expectedLeft);
    QFETCH(QString, expectedRight);
    QFETCH(QString, expectedDescription);

    runParser(input);

    QCOMPARE(importComponent_->getPorts()->count(), 1);

    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();

    QCOMPARE(createdPort->name(), expectedPortName);
    QCOMPARE(createdPort->getTypeName(), expectedType);
    QCOMPARE(createdPort->getDirection(), expectedDirection);
    QCOMPARE(createdPort->getLeftBound(), expectedLeft);
    QCOMPARE(createdPort->getRightBound(), expectedRight);
    QCOMPARE(createdPort->description(), expectedDescription);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testVerilog1995PortIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testVerilog1995PortIsParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedPortName");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<DirectionTypes::Direction>("expectedDirection");
    QTest::addColumn<QString>("expectedLeft");
    QTest::addColumn<QString>("expectedRight");
    QTest::addColumn<QString>("expectedDescription");

    QTest::newRow("Simplest possible port") <<
        "module test (clk);\n"
        "    input clk;\n"
        "\n"
        "endmodule"
        << "clk" << "" << DirectionTypes::IN << "" << "" << "";

    QTest::newRow("Little-endian port") <<
    "module test (data);\n"
    "    input reg [7:0] data;\n"
    "\n"
    "endmodule"
    << "data" << "reg" <<  DirectionTypes::IN << "7" << "0" << "";

    QTest::newRow("Big-endian port") <<
    "module test (data);\n"
    "    input [0:7] data;\n"
    "\n"
    "endmodule"
    << "data" << "" <<  DirectionTypes::IN << "0" << "7" << "";
    
    QTest::newRow("Comment as a description.") <<
    "module test (rst_n);\n"
    "    input wire rst_n; // Active low reset.\n"
    "\n"
    "endmodule"
    << "rst_n" << "wire" <<  DirectionTypes::IN << "" << "" << "Active low reset.";

    QTest::newRow("Additional whitespaces in port declaration") <<
    "module test (cs);output \n"
    "\n"
    "    [1:0] \n"
    "    cs  ;  //Chip select\n"
    "\n"
    "endmodule"
    << "cs" << "" <<  DirectionTypes::OUT << "1" << "0" << "Chip select";

    QTest::newRow("Module with parameters.") <<
    "module test (clk);\n"
    "    parameter size = 8;"
    "    input clk;\n"
    "\n"
    "endmodule"
    << "clk" << "" <<  DirectionTypes::IN << "" << "" << "";

    QTest::newRow("Keywords in comments within implementation.") <<
        "module test (data);\n"        
        "    input data;\n"
        "\n"
        "wire data;   // Data input\n"
        "wire data_out;  // Data output\n"
        "\n"
        "endmodule"
        << "data" << "" <<  DirectionTypes::IN << "" << "" << "";

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed()
{
    QFETCH(QString, input);
    QFETCH(QStringList, expectedPorts);

    runParser(input);

    QCOMPARE(importComponent_->getPortNames(), expectedPorts);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultiplePortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultiplePortsAreParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Three basic ports") <<
        "module test (\n"
        "    input clk,\n"
        "    input rst_n,\n"
        "    output reg [7:0] data\n"
        ");\n"
        "endmodule"
        << QString("clk, rst_n, data").split(", ");

    QTest::newRow("Comments among ports") <<
        "module test ( // Ports are declared here.\n"
        "    input clk,\n"  
        "    // Comment between ports.\n"
        "    input rst_n\n"
        "    // End ports.\n"
        ");\n"
        "endmodule" 
        << QString("clk, rst_n").split(", ");

    QTest::newRow("Multiple declarations at once") <<
        "module test (\n"
        "    input clk, rst_n, enable,\n" 
        "    output full, ack\n" 
        ");\n"
        "endmodule"
        << QString("clk, rst_n, enable, full, ack").split(", ");

    // TODO: Syntactical error. What is the correct behavior?
    QTest::newRow("last port with tailing colon") << 
        "module test(\n"
        "    input clk,\n"
        "    input rst,\n"
        "    input enable,\n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

    QTest::newRow("Commented out ports") << 
        "module test(\n"
        "    //input clk,\n"
        "    //input rst,\n"
        "    input enable\n"
        ");\n"
        "endmodule"
        << QString("enable").split(", ");

    QTest::newRow("Commented out ports using multiline comment") << 
        "module test(\n"
        "    input clk,\n"        
        "    input rst,\n"
        "/* COMMENT OUT FOR SYNTHESIS\n"
        "    output sim_value,\n"
        "    output sim_debug,\n"
        "*/\n"
        "    input enable \n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

    QTest::newRow("Misplaced comma") << 
        "module test(\n"
        "    input clk,\n" 
        "    input rst\n"
        "    ,input enable \n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

        QTest::newRow("Conditional ports") << 
        "module test(\n"
        "    `ifdef SYNCHRONOUS\n"
        "    input clk,\n" 
        "    `endif\n"
        "    input rst\n"
        "    `ifdef ENABLE_PIN\n"
        "    ,input enable \n"
        "    `endif\n"
        ");\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");

        QTest::newRow("Conditional port and comma on next line") << 
            "module test(\n"
            "    input clk\n" 
            "    `ifdef EXTERNAL_RESET\n"
            "    ,input rst // reset\n"
            "    `endif\n"
            "    `ifdef ENABLE_PIN\n"
            "    ,input enable\n"
            "    `endif\n"
            ");\n"
            "endmodule"
            << QString("clk, rst, enable").split(", ");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed()
{
    testMultiplePortsAreParsed();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMultipleVerilog1995PortsAreParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Three basic ports") <<
        "module test (clk, rst_n, data);\n"
        "    input clk;\n"
        "    input rst_n;\n"
        "    output [7:0] data;\n"
        "\n"
        "endmodule"
        << QString("clk, rst_n, data").split(", ");
    
    QTest::newRow("Comments among ports") <<
        "module test ( // Ports are declared here.\n"
        "clk, // Only names declared here.\n"
        "rst_n);"
        "    // Declare also direction for ports.\n"
        "    input clk;\n"  
        "    // Comment between ports.\n"
        "    input rst_n;\n"
        "    // End ports.\n"
        "\n"
        "endmodule" 
        << QString("clk, rst_n").split(", ");
    
    QTest::newRow("Multiple declarations at once") <<
        "module test (clk, rst_n, enable, full, ack);\n"
        "    input clk, rst_n, enable;\n" 
        "    output full, ack;\n" 
        "\n"
        "endmodule"
        << QString("clk, rst_n, enable, full, ack").split(", ");

    QTest::newRow("last port without tailing semicolon") << 
        "module test(clk, rst, enable);\n"
        "    input clk;\n"
        "    input rst;\n"
        "    input enable\n"
        "\n"
        "endmodule"
        << QString("clk, rst").split(", ");

    QTest::newRow("Commented out ports") << 
        "module test(//clk, rst, \n"
        "            enable);\n"
        "    //input clk;\n"
        "    //input rst;\n"
        "    input enable;\n"
        "\n"
        "endmodule"
        << QString("enable").split(", ");

    QTest::newRow("Commented out ports using multiline commments") << 
        "module test(clk, rst, /* sim_value, sim_debug, */ enable);\n"
        "    input clk;\n"
        "    input rst;\n"
        "/* COMMENT OUT FOR SYNTHESIS\n"
        "    output sim_value;\n"
        "    output sim_debug;\n"
        "*/\n"
        "    input enable;\n"
        "\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");    
    
    QTest::newRow("Port definition in port list commments") << 
        "module test(clk, // input mask,\n"
        "rst,\n"
        "enable);\n"
        "    input clk;\n"
        "    input rst;\n"
        "    input enable;\n"
        "\n"
        "endmodule"
        << QString("clk, rst, enable").split(", ");   

    QTest::newRow("C") << 
        "module  multiport (\n"
        "\n"
        "// output ports\n"
        "port1,                         // Comment on port\n"
        "port2,                         // Comment\n"
        "port3,                         // Comment\n"
        "port4,                         // Comment\n"
        "port5,                         // Comment\n"
        "port6,                         // Comment\n"
        "port7,                         // Comment\n"
        "port8,                         // Comment\n"
        "port9,                         // Comment\n"
        "port10,                        // Comment on port\n"
        "port11,                        // Comment\n"
        "port12,                        // Comment\n"
        "port13,                        // Comment\n"
        "port14,                        // Comment\n"
        "port15,                        // Comment\n"
        "port16,                        // Comment\n"
        "port17,                        // Comment\n"
        "port18,                        // Comment\n"
        "port19,                        // Comment\n"
        "port20,                        // Comment on port\n"
        "port21,                        // Comment\n"
        "port22,                        // Comment\n"
        "port23,                        // Comment\n"
        "port24,                        // Comment\n"
        "port25,                        // Comment\n"
        "port26,                        // Comment\n"
        "port27,                        // Comment\n"
        "\n"
        "// input ports\n"
        "port28,                        // Comment\n"
        "port29,                        // Comment\n"
        "port30,                        // Comment on port\n"
        "port31,                        // Comment\n"
        "port32,                        // Comment\n"
        "port33,                        // Comment\n"
        "port34,                        // Comment\n"
        "port35,                        // Comment\n"
        "port36,                        // Comment\n"
        "port37,                        // Comment\n"
        "port38,                        // Comment\n"
        "port39,                        // Comment\n"
        "port40,                        // Comment on port\n"
        "port41,                        // Comment\n"
        "port42,                        // Comment\n"
        "port43,                        // Comment\n"
        "port44,                        // Comment\n"
        "port45,                        // Comment\n"
        "port46,                        // Comment\n"
        "port47,                        // Comment\n"
        "port48,                        // Comment\n"
        "port49,                        // Comment\n"
        "port50                         // Comment on last port\n"
        ");\n"
        "\n"
        "//=============================================================================\n"
        "// output ports\n"
        "//=============================================================================\n"
        "output               port1;    // Comment on port\n"
        "output               port2;    // Comment\n"
        "output         [1:0] port3;    // Comment\n"
        "output               port4;    // Comment\n"
        "output         [2:0] port5;    // Comment\n"
        "output         [3:0] port6;    // Comment\n"
        "output         [4:0] port7;    // Comment\n"
        "output               port8;    // Comment\n"
        "output         [5:0] port9;    // Comment\n"
        "output         [6:0] port10;   // Comment on port\n"
        "output               port11;   // Comment\n"
        "output         [7:0] port12;   // Comment\n"
        "output               port13;   // Comment\n"
        "output         [8:0] port14;   // Comment\n"
        "output         [9:0] port15;   // Comment\n"
        "output        [10:0] port16;   // Comment\n"
        "output               port17;   // Comment\n"
        "output  [`PARAM-1:0] port18;   // Comment\n"
        "output               port19;   // Comment\n"
        "output               port20;   // Comment on port\n"
        "output               port21;   // Comment\n"
        "output               port22;   // Comment\n"
        "output               port23;   // Comment\n"
        "output               port24;   // Comment\n"
        "output               port25;   // Comment\n"
        "output               port26;   // Comment\n"
        "output               port27;   // Comment\n"
        "\n"
        "//=============================================================================\n"
        "// input ports\n"
        "//=============================================================================\n"
        "input                port28;   // Comment\n"
        "input                port29;   // Comment\n"
        "input                port30;   // Comment on port\n"
        "input                port31;   // Comment\n"
        "input                port32;   // Comment\n"
        "input                port33;   // Comment\n"
        "input                port34;   // Comment\n"
        "input                port35;   // Comment\n"
        "input                port36;   // Comment\n"
        "input                port37;   // Comment\n"
        "input                port38;   // Comment\n"
        "input                port39;   // Comment\n"
        "input                port40;   // Comment on port\n"
        "input                port41;   // Comment\n"
        "input                port42;   // Comment\n"
        "input                port43;   // Comment\n"
        "input                port44;   // Comment\n"
        "input                port45;   // Comment\n"
        "input                port46;   // Comment\n"
        "input                port47;   // Comment\n"
        "input                port48;   // Comment\n"
        "input                port49;   // Comment\n"
        "input                port50;   // Comment on last port\n"
        "\n"
        "//=============================================================================\n"
        "// Module implementation ... \n"
        "//=============================================================================\n"
        "endmodule"

        << QString("port1, port2, port3, port4, port5, port6, port7, port8, port9, port10, "
        "port11, port12, port13, port14, port15, port16, port17, port18, port19, port20, "
        "port21, port22, port23, port24, port25, port26, port27, port28, port29, port30, "
        "port31, port32, port33, port34, port35, port36, port37, port38, port39, port40, "
        "port41, port42, port43, port44, port45, port46, port47, port48, port49, port50").split(", ");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
{
    testMultiplePortsAreParsed();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testPortsInSubmodulesAreNotParsed_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QStringList>("expectedPorts");

    QTest::newRow("Instantiation of submodule") <<
        "module tb_half_adder();\n"
        "    wire A;\n"
        "    wire B;\n"
        "    wire result_out;\n"
        "    wire carry_out;\n"
        "\n"
        "    half_adder dut(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(result_out),\n"
        "        .carry(carry_out)\n"
        "    );\n"    
        "// ... \n"
        "\n"
        "endmodule"
        << QStringList();

    QTest::newRow("Instantiation of multiple submodules") <<
        "module full_adder(\n"
        "    input A, B, carry_in,\n"
        "    output sum, carry_out\n"
        ");\n"                
        "    wire ha1_ha2_sum;\n"
        "    wire c1;\n"
        "    wire c2;\n"
        "\n"
        "    half_adder ha1(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(ha1_ha2_sum),\n"
        "        .carry(c1)\n"
        "    );\n"    
        "\n"
        "    half_adder ha2(\n"
        "        .in1(ha1_ha2_sum),\n"
        "        .in2(carry_in),\n"
        "        .sum(sum),\n"
        "        .carry(c2)\n"
        "    );\n"   
        "\n"
        "    or (carry_out, c1, c2);"
        "\n"
        "endmodule"
        << QString("A, B, carry_in, sum, carry_out").split(", ");

    QTest::newRow("Instantiation of multiple submodules using 1995 style module") <<
        "module full_adder(A, B, carry_in, sum, carry_out);\n"                
        "    input A, B, carry_in;\n"
        "    output sum, carry_out;\n"
        "\n"
        "    wire ha1_ha2_sum;\n"
        "    wire c1;\n"
        "    wire c2;\n"
        "\n"
        "    half_adder ha1(\n"
        "        .in1(A),\n"
        "        .in2(B),\n"
        "        .sum(ha1_ha2_sum),\n"
        "        .carry(c1)\n"
        "    );\n"    
        "\n"
        "    half_adder ha2(\n"
        "        .in1(ha1_ha2_sum),\n"
        "        .in2(carry_in),\n"
        "        .sum(sum),\n"
        "        .carry(c2)\n"
        "    );\n"   
        "\n"
        "    or (carry_out, c1, c2);"
        "\n"
        "endmodule"
        << QString("A, B, carry_in, sum, carry_out").split(", ");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testEquationAsPortBound()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testEquationAsPortBound()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts()->count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();
    QCOMPARE(createdPort->getLeftBound(), expectedLeftExpression);
    QCOMPARE(createdPort->getRightBound(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testEquationAsPortBound_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testEquationAsPortBound_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("Subtraction") <<
        "module test(\n"
        "    input [2-1:0] data\n"
        ");\n"
        "endmodule\n"
        << "2-1" << "0";

    QTest::newRow("Both bounds with multiply") <<
        "module test(\n"
        "    input [2*4-1:2*1] data\n"
        ");\n"
        "endmodule\n"
        << "2*4-1" << "2*1";

    QTest::newRow("Power of two") <<
        "module test(\n"
        "    input [2**4:0] data\n"
        ");\n"
        "endmodule\n"
        << "2**4" << "0";

    QTest::newRow("Power of zero") <<
        "module test(\n"
        "    input [8**0:0] data\n"
        ");\n"
        "endmodule\n"
        << "8**0" << "0";

    QTest::newRow("Multiple operations in mixed order") <<
        "module test(\n"
        "    input [32-2**4+2*4:0] data\n"
        ");\n"
        "endmodule\n"
        << "32-2**4+2*4" << "0";

    QTest::newRow("Division") <<
        "module test(\n"
        "    input [32/2:0/2] data\n"
        ");\n"
        "endmodule\n"
        << "32/2" << "0/2";

    QTest::newRow("Division by zero results zero") <<
        "module test(\n"
        "    input [32/0:0] data\n"
        ");\n"
        "endmodule\n"
        << "32/0" << "0";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testArrayPort()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testArrayPort()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    QSharedPointer<Parameter> macroDefinition(new Parameter());
    macroDefinition->setName("WIDTH");
    macroDefinition->setValue("16");
    macroDefinition->setValueId("macroId");
    importComponent_->getParameters()->append(macroDefinition);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts()->count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();
    QCOMPARE(createdPort->getArrayLeft(), expectedLeftExpression);
    QCOMPARE(createdPort->getArrayRight(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testArrayPort_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testArrayPort_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("No array bounds, only vector bounds") <<
        "module test(\n"
        "    input bit [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "" << "";

    QTest::newRow("Constants as array bounds") <<
        "module test(\n"
        "    input bit [0:0] [7:0] data\n"
        ");\n"
        "endmodule\n"
        << "7" << "0";

    QTest::newRow("Expression as array bounds") <<
        "module test(\n"
        "    input bit [0:0] [2**4:1] data\n"
        ");\n"
        "endmodule\n"
        << "2**4" << "1";

    QTest::newRow("Expression with parenthesis as array bounds") <<
        "module test(\n"
        "    input bit [0:0] [2*(8+8-1):1] data\n"
        ");\n"
        "endmodule\n"
        << "2*(8+8-1)" << "1";

    QTest::newRow("Macro in left array bound") <<
        "module test(\n"
        "    input [7:0] [`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << "macroId" << "0";

    QTest::newRow("Macro in left v expression") <<
        "module test(\n"
        "    input [7:0] [8+`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << "8+macroId" << "0";

    QTest::newRow("Macro in right array bound") <<
        "module test(\n"
        "    input [7:0] [0:`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "macroId";

    QTest::newRow("Macro in right array expression") <<
        "module test(\n"
        "    input [7:0] [0:2*`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "2*macroId";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMacroInBoundsIsParsed()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMacroInBoundsIsParsed()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedLeftExpression);
    QFETCH(QString, expectedRightExpression);

    QSharedPointer<Parameter> macroDefinition(new Parameter());
    macroDefinition->setName("WIDTH");
    macroDefinition->setValue("16");
    macroDefinition->setValueId("macroId");
    importComponent_->getParameters()->append(macroDefinition);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts()->count() != 0, "No ports parsed from input.");

    QSharedPointer<Port> createdPort = importComponent_->getPorts()->first();
    QCOMPARE(createdPort->getLeftBound(), expectedLeftExpression);
    QCOMPARE(createdPort->getRightBound(), expectedRightExpression);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogPortParser::testMacroInBoundsIsParsed_data()
//-----------------------------------------------------------------------------
void tst_VerilogPortParser::testMacroInBoundsIsParsed_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedLeftExpression");
    QTest::addColumn<QString>("expectedRightExpression");

    QTest::newRow("Macro in left vector bound") <<
        "module test(\n"
        "    input [`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << "macroId" << "0";

    QTest::newRow("Macro in left vector expression") <<
        "module test(\n"
        "    input [8+`WIDTH:0] data\n"
        ");\n"
        "endmodule\n"
        << "8+macroId" << "0";

    QTest::newRow("Macro in right vector bound") <<
        "module test(\n"
        "    input [0:`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "macroId";

    QTest::newRow("Macro in right vector expression") <<
        "module test(\n"
        "    input [0:2*`WIDTH] data\n"
        ");\n"
        "endmodule\n"
        << "0" << "2*macroId";
}

QTEST_APPLESS_MAIN(tst_VerilogPortParser)

#include "tst_VerilogPortParser.moc"
